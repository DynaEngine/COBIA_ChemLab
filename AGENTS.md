# Agents — ChemLab 项目开发指南

本文档为 AI 编程助手 (Agent) 提供 ChemLab 项目的完整上下文，确保生成的代码与项目架构、规范和工具链保持一致。

---

## 1. 项目概述

**ChemLab** — 基于 COBIA (CAPE-OPEN Binary Interop Architecture) 的现代化化工过程模拟平台。跨平台 (Windows / Linux / macOS)，C++17 / Qt6 实现。

**核心目标**: 提供与 DWSIM 等商业软件兼容的 CAPE-OPEN 物性包和单元操作互操作能力，同时构建原生的流程模拟 GUI。

### 1.1 项目结构

```
ChemLab/
├── AGENTS.md                  ← 本文档
├── CMakeLists.txt             ← 根 CMake
├── CMakePresets.json          ← 构建预设
├── ChemEngine/                ← 核心引擎 (静态库)
│   ├── include/ChemEngine/    ← 公开头文件
│   │   ├── COBIA/             ← COBIA 封装层
│   │   ├── Flowsheet/         ← 流程管理
│   │   ├── Solver/            ← 求解器
│   │   ├── Base/              ← 基础类型
│   │   ├── Interfaces/        ← 接口定义
│   │   ├── Units/             ← 内建单元
│   │   └── Utils/             ← 工具函数
│   └── src/                   ← 源文件
├── ChemLab/                   ← Qt6 GUI 应用
│   ├── main.cpp               ← 入口点
│   ├── mainwindow.h/.cpp      ← 主窗口
│   ├── ribbonbar.h/.cpp       ← Ribbon 工具栏
│   ├── simulationmanager.h/.cpp ← Engine ↔ GUI 桥接
│   ├── panels/                ← 停靠面板
│   │   ├── propertypackagepanel   ← 物性包选择
│   │   ├── propertyresearchpanel  ← 物性研究
│   │   ├── flowsheetpanel         ← 流程树
│   │   ├── solverpanel            ← 求解控制
│   │   ├── resultspanel           ← 结果展示
│   │   ├── logpanel               ← 日志输出
│   │   └── unitoperationpanel     ← 单元操作列表
│   ├── theme/theme.h          ← 全局暗色主题
│   └── resources/             ← SVG 图标 / QRC
├── ChemProp/                  ← COBIA 物性包模块
│   ├── IdealGasPP/            ← 理想气体物性包
│   └── WaterPP/               ← 水 IAPWS-97 物性包
├── ChemUnit/                  ← COBIA 单元操作模块
│   └── MHExch/                ← 多流股换热器
└── Docs/                      ← 项目文档
    ├── COBIA.md               ← COBIA 协议参考
    ├── ChemLab_Road_Map.md    ← 开发路线图
    └── dwsim-windows/         ← DWSIM 二进制参考
```

---

## 2. 构建环境

### 2.1 工具链

| 组件 | 路径 | 版本 |
|------|------|------|
| CMake | `D:\program\QT6\Tools\mingw1310_64\bin\cmake.exe` | 3.20+ |
| GCC/MinGW64 | `D:\program\QT6\Tools\mingw1310_64\bin\g++.exe` | 13.1.0 |
| Ninja | `D:\program\QT6\Tools\mingw1310_64\bin\ninja.exe` | — |
| Qt 6 | `D:\program\QT6\6.9.1\mingw_64` | 6.9.1 |

### 2.2 COBIA SDK

| 组件 | 路径 | 版本 |
|------|------|------|
| 32位头文件 | `C:\Program Files (x86)\Common Files\CAPE-OPEN Laboratories Network\Include\` | 1.2.2.1 |
| 64位运行库 | `C:\Program Files\Common Files\CAPE-OPEN Laboratories Network\COBIA\` | 1.2.2.1 |
| 64位 SDK | `C:\Program Files\Common Files\CAPE-OPEN Laboratories Network\COBIA-SDK\` | 1.2.2.1 |
| 注册工具 | `C:\Program Files\Common Files\CAPE-OPEN Laboratories Network\COBIA\cobiaRegister.exe` | — |

### 2.3 第三方库

| 库 | 路径 | 用途 |
|------|------|------|
| QWindowKit | `third_party/QWindowKit/` | 无边框窗口 + 亚克力效果 |
| Qwt 7 | `third_party/qwt7/` | 物性曲线 / 图表绘制 |

---

## 3. 构建命令

### 3.1 配置 (首次或清理后)

```powershell
# 默认使用 qt6-minsizerel preset
cmake --preset qt6-minsizerel

# 其他 Presets:
cmake --preset qt6-debug
cmake --preset qt6-release
```

### 3.2 编译

```powershell
# 编译 ChemLab GUI (主目标)
cmake --build --preset qt6-minsizerel --target ChemLab -j8

# 编译引擎
cmake --build --preset qt6-minsizerel --target ChemEngine -j8

# 编译所有物性包和单元操作
cmake --build --preset qt6-minsizerel -j8
```

### 3.3 安装

```powershell
cmake --build --preset qt6-minsizerel --target install
# 产物位于: build/qt6-minsizerel/install/ChemLab/
```

### 3.4 注册 COBIA 模块

```powershell
# 注册所有模块 (需要管理员权限)
C:\Program Files\Common Files\CAPE-OPEN Laboratories Network\COBIA\cobiaRegister.exe /register .../install/ChemLab/ChemProp/IdealGasPP.dll
```

### 3.5 Presets 说明

| Preset | 构建类型 | 编译器 |
|--------|----------|--------|
| `qt6-minsizerel` | MinSizeRel | MinGW 13.1.0 |
| `qt6-debug` | Debug | MinGW 13.1.0 |
| `qt6-release` | Release | MinGW 13.1.0 |

---

## 4. 架构设计

### 4.1 整体分层

```
┌────────────────────────────────────────────┐
│  ChemLab GUI (Qt6 Widgets + QWindowKit)     │
│  ├─ MainWindow (Ribbon + MDI + Docks)       │
│  ├─ Panels (DockWidget 停靠面板)             │
│  └─ SimulationManager (GUI ↔ Engine 桥接)   │
├────────────────────────────────────────────┤
│  ChemEngine (静态库, C++17)                  │
│  ├─ Flowsheet / FlowsheetSolver             │
│  ├─ CapeMaterialStream (COBIA 物料流封装)    │
│  ├─ CapeUnitWrapper (COBIA 单元操作封装)     │
│  ├─ PropertyPackageManager                  │
│  ├─ UnitOperationManager                    │
│  └─ BuiltInUnits (内建 Mixer/Heater/Flash)   │
├────────────────────────────────────────────┤
│  ChemProp (COBIA Property Packages, DLL)     │
│  └─ IdealGasPP / WaterPP                    │
├────────────────────────────────────────────┤
│  ChemUnit (COBIA Unit Operations, DLL)       │
│  └─ MHExch                                  │
└────────────────────────────────────────────┘
```

### 4.2 命名空间

| 命名空间 | 用途 | 示例 |
|----------|------|------|
| `ChemEngine::COBIA` | COBIA 封装层 | `CapeMaterialStream`, `PropertyPackageManager` |
| `ChemEngine` | 通用引擎类型 | `Flowsheet`, `FlowsheetSolver` |
| `COBIA::` | COBIA SDK 原生接口 | `CapeInterface`, `CapeStringImpl` |
| `CAPEOPEN_1_2::` | CAPE-OPEN 1.2 接口 | `CapeUnit`, `CapeThermoMaterial` |
| `ChemLabTheme` | GUI 主题样式 | `applyDarkTheme()` |

### 4.3 关键类型与接口

| 类型 | 文件 | 说明 |
|------|------|------|
| `SimulationManager` | [simulationmanager.h](file:///d:/LABS/CODE/0001C/CodeEnv/QT/ChemLab/ChemLab/simulationmanager.h) | GUI ↔ Engine 桥接，管理求解生命周期 |
| `Flowsheet` | [Flowsheet.h](file:///d:/LABS/CODE/0001C/CodeEnv/QT/ChemLab/ChemEngine/include/ChemEngine/Flowsheet/Flowsheet.h) | 流程拓扑、化合物、单元操作管理 |
| `CapeMaterialStream` | [CapeMaterialStream.h](file:///d:/LABS/CODE/0001C/CodeEnv/QT/ChemLab/ChemEngine/include/ChemEngine/COBIA/CapeMaterialStream.h) | COBIA 物料流封装 (flash, 物性计算) |
| `PropertyPackageManager` | [PropertyPackageManager.h](file:///d:/LABS/CODE/0001C/CodeEnv/QT/ChemLab/ChemEngine/include/ChemEngine/COBIA/PropertyPackageManager.h) | 物性包枚举、加载、平衡/物性计算 |
| `CapeUnitWrapper` | [CapeUnitWrapper.h](file:///d:/LABS/CODE/0001C/CodeEnv/QT/ChemLab/ChemEngine/include/ChemEngine/COBIA/CapeUnitWrapper.h) | COBIA 单元操作封装 |
| `SimulationObject` | [SimulationObject.h](file:///d:/LABS/CODE/0001C/CodeEnv/QT/ChemLab/ChemEngine/include/ChemEngine/Base/SimulationObject.h) | 所有模拟对象的基类 |
| `CompoundConstantProperties` | [CompoundConstantProperties.h](file:///d:/LABS/CODE/0001C/CodeEnv/QT/ChemLab/ChemEngine/include/ChemEngine/Base/CompoundConstantProperties.h) | 纯组分常数属性 (Tc, Pc, ω 等) |

---

## 5. 编码规范

### 5.1 文件组织

- 每个新 Panel 类放在 `ChemLab/panels/` 目录下，包含 `.h` + `.cpp` 一对一
- Engine 头文件放在 `ChemEngine/include/ChemEngine/` 下按功能分子目录
- Engine 源文件放在 `ChemEngine/src/` 下平铺

### 5.2 命名约定

- **类名**: PascalCase (如 `PropertyResearchPanel`, `CapeMaterialStream`)
- **成员变量**: `m_` 前缀 + camelCase (如 `m_packageTree`, `m_simMgr`)
- **方法**: camelCase (如 `refreshCompounds()`, `calculateEquilibrium()`)
- **Qt slot**: `on` + 骆驼命名 (如 `onCalculate()`, `onPackageToggled()`)
- **文件**: snake_case 或 PascalCase，与类名对应

### 5.3 C++ 标准与工具

- **C++ 标准**: C++17 (`CMAKE_CXX_STANDARD 17`)
- **智能指针**: 使用 `std::unique_ptr` / `std::make_unique` 管理所有权
- **Qt 内存管理**: 使用 `new QWidget(this)` 让 Qt parent-child 管理生命周期
- **字符串**: GUI 层使用 `QString` / `QStringLiteral`，Engine 层使用 `std::wstring`

### 5.4 GUI 面板模式

所有面板 (Panel) 遵循统一模式：

```cpp
class SomePanel : public QWidget {
    Q_OBJECT
public:
    explicit SomePanel(SimulationManager* simMgr, QWidget* parent = nullptr);
    void refreshXxx();  // 刷新数据

private slots:
    void onSomeAction();  // 用户交互

private:
    void setupUi();  // 构建 UI
    SimulationManager* m_simMgr;  // 通过此桥接访问 Engine
};
```

### 5.5 Qt 信号/槽

- 使用新式连接: `connect(sender, &SenderClass::signal, receiver, &ReceiverClass::slot)`
- QComboBox 索引变更用: `QOverload<int>::of(&QComboBox::currentIndexChanged)`

### 5.6 暗色主题

所有 UI 必须适配暗色主题。调用 [theme.h](file:///d:/LABS/CODE/0001C/CodeEnv/QT/ChemLab/ChemLab/theme/theme.h) 中的 `ChemLabTheme::applyDarkTheme()` 应用全局样式。Dock 面板的:

- 背景色: `#1e1e2e`
- 文本色: `#cdd6f4`
- 强调色: `#4a9eff`
- 表格/列表组件: 遵循全局 QSS 样式

---

## 6. COBIA 开发要点

### 6.1 编译定义

所有 COBIA 相关模块必须定义:
```cmake
target_compile_definitions(target PRIVATE COBIA_NOAUTOLINK)
```

### 6.2 头文件包含

```cpp
// 主头文件 (包含所有子模块)
#include "COBIA.h"
// 或按需包含:
#include "COBIA_Error.h"
#include "COBIA_BasicDataTypes.h"
#include "COBIA_Interfaces.h"
#include "CapeInterfaces_1_2.h"
```

### 6.3 物性包开发

COBIA 物性包需要实现:
- `ICapeThermoPropertyPackageManager` (入口点)
- `ICapeThermoMaterialTemplate` (组分管理)
- `ICapeThermoMaterial` (物性计算)
- `ICapeThermoPhases` (相态信息)

详见 [COBIA物性包开发指南.md](file:///d:/LABS/CODE/0001C/CodeEnv/QT/ChemLab/ChemProp/docs/COBIA物性包开发指南.md)

### 6.4 单元操作开发

COBIA 单元操作需要实现:
- `ICapeUnit` (主要接口: `Calculate`, `Validate`, `ports`)
- `ICapeUtilities` (初始化/终止)
- `ICapeIdentification` (组件名称/描述)

详见 [COBIA模块开发指南.md](file:///d:/LABS/CODE/0001C/CodeEnv/QT/ChemLab/ChemUnit/docs/COBIA模块开发指南.md)

### 6.5 注册与发现

- Windows: 使用 `cobiaRegister.exe` 注册 DLL
- Engine 中使用 `PropertyPackageManager::enumeratePackages()` 发现已注册物性包
- Engine 中使用 `UnitOperationManager::enumerateUnits()` 发现单元操作

### 6.6 Engine 中的计算流程

```
1. PropertyPackageManager::loadPackage(progId)  → 加载物性包
2. CapeMaterialStream::createMaterial(name)     → 创建物料对象
3. CapeMaterialStream::setTemperature(T)        → 设置状态
4. CapeMaterialStream::setPressure(P)
5. CapeMaterialStream::flashTP()                → 闪蒸计算
6. CapeMaterialStream::getOverallProp(...)      → 读取物性
```

---

## 7. ChemLab Window System

### 7.1 无边框窗口

使用 **QWindowKit** 提供无边框窗口效果。MainWindow 在构造函数中调用:
```cpp
installWindowAgent();  // 安装 QWindowKit 窗口代理
```

### 7.2 Ribbon 工具栏

`RibbonBar` 是自定义的 Ribbon 式工具栏:
- `addTab(name)` — 添加标签页
- `addGroup(tabIndex, groupName)` — 添加功能组
- `addLargeButton(...)` / `addSmallButton(...)` — 添加按钮

### 7.3 Dock 面板系统

所有功能面板通过 `QDockWidget` 停靠:
- `m_dockSolver` ← SolverPanel (右侧，默认显示)
- `m_dockPropertyResearch` ← PropertyResearchPanel (与 Solver Tab 堆叠)
- `m_dockUnitOps` ← UnitOperationPanel (左侧)
- `m_dockFlow` ← FlowsheetPanel (左侧，与 UnitOps Tab 堆叠)
- `m_dockLog` ← LogPanel (底部)
- `m_dockResults` ← ResultsPanel (右侧，与 Solver Tab 堆叠)

新面板创建模式:
```cpp
m_dockNew = new QDockWidget("Title", this);
m_dockNew->setObjectName("newDock");
m_newPanel = new NewPanel(m_simManager, m_dockNew);
m_dockNew->setWidget(m_newPanel);
addDockWidget(Qt::RightDockWidgetArea, m_dockNew);
tabifyDockWidget(m_dockSome, m_dockNew);  // 如需合并 Tab
```

---

## 8. 常见开发任务

### 8.1 添加新的 GUI 面板

1. 在 `ChemLab/panels/` 下创建 `MyNewPanel.h` + `MyNewPanel.cpp`
2. 使用 `SomePanel(SimulationManager*, QWidget* parent)` 构造函数
3. 在 `setupUi()` 中构建 UI 布局
4. 在 [mainwindow.h](file:///d:/LABS/CODE/0001C/CodeEnv/QT/ChemLab/ChemLab/mainwindow.h) 中添加前向声明和成员变量
5. 在 [mainwindow.cpp](file:///d:/LABS/CODE/0001C/CodeEnv/QT/ChemLab/ChemLab/mainwindow.cpp) 的 `setupDockPanels()` 中创建和停靠
6. 在 [CMakeLists.txt](file:///d:/LABS/CODE/0001C/CodeEnv/QT/ChemLab/ChemLab/CMakeLists.txt) 的 `SOURCES` 中添加新文件

### 8.2 添加新的 Engine 功能

1. 在 `ChemEngine/include/ChemEngine/` 下添加头文件
2. 在 `ChemEngine/src/` 下添加源文件
3. 在 [ChemEngine/CMakeLists.txt](file:///d:/LABS/CODE/0001C/CodeEnv/QT/ChemLab/ChemEngine/CMakeLists.txt) 中添加源文件
4. 如需 GUI 访问，在 `SimulationManager` 中添加桥接方法

### 8.3 添加新的物性包 / 单元操作

参照 [ChemProp/CMakeLists.txt](file:///d:/LABS/CODE/0001C/CodeEnv/QT/ChemLab/ChemProp/CMakeLists.txt) 或 [ChemUnit/CMakeLists.txt](file:///d:/LABS/CODE/0001C/CodeEnv/QT/ChemLab/ChemUnit/CMakeLists.txt) 中的 `IdealGasPP` / `WaterPP` / `MHExch` 模式。

### 8.4 算物性 (Research Mode)

物性研究面板支持独立的物性包加载和对比。详见 [ChemLab_Road_Map.md#17](file:///d:/LABS/CODE/0001C/CodeEnv/QT/ChemLab/Docs/ChemLab_Road_Map.md) Phase 1.7。

物性计算关键 API:
```cpp
auto mat = std::make_unique<ChemEngine::COBIA::CapeMaterialStream>();
mat->createMaterial(L"Calc");
mat->setTemperature(298.15);
mat->setPressure(101325);
mat->flashTP();
double density = mat->getOverallProp(L"density", L"mole");
```

---

## 9. 文档参考

| 文档 | 路径 | 内容 |
|------|------|------|
| COBIA 协议 | [Docs/COBIA.md](file:///d:/LABS/CODE/0001C/CodeEnv/QT/ChemLab/Docs/COBIA.md) | COBIA 中间件、SDK、接口、注册表 |
| 开发路线图 | [Docs/ChemLab_Road_Map.md](file:///d:/LABS/CODE/0001C/CodeEnv/QT/ChemLab/Docs/ChemLab_Road_Map.md) | 7个 Phase 开发计划 |
| 物性包开发 | [ChemProp/docs/COBIA物性包开发指南.md](file:///d:/LABS/CODE/0001C/CodeEnv/QT/ChemLab/ChemProp/docs/COBIA物性包开发指南.md) | COBIA PP 开发指南 |
| 模块开发 | [ChemUnit/docs/COBIA模块开发指南.md](file:///d:/LABS/CODE/0001C/CodeEnv/QT/ChemLab/ChemUnit/docs/COBIA模块开发指南.md) | COBIA Unit 开发指南 |

---

## 10. Windows 特定注意事项

- 链接需要 `ole32`, `oleaut32`, `advapi32`, `shell32`, `uuid`
- MinGW 需要额外的链接选项: `-Wl,--enable-stdcall-fixup`, `-static-libgcc`, `-static-libstdc++`
- COBIA Windows 注册需要管理员权限
- 物性包/单元操作 DLL 放在 `build/qt6-minsizerel/install/ChemLab/ChemProp/` 和 `ChemUnit/`