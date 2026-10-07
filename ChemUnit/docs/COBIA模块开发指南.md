# COBIA 模块开发指南

> 以本项目的 `MHExch`（多股流换热器单元操作）为参考示例。读完本文后，你应能独立编写、调试并注册一个可被 COFE 加载的 COBIA 单元操作模块（Unit Operation）。

---

## 一、背景：什么是 COBIA 单元操作

### 1.1 CAPE-OPEN 中的角色定位

流程模拟系统中有两大可插拔组件：

```
┌──────────────────────┐          ┌──────────────────────┐
│   Property Package   │          │    Unit Operation     │
│   物性包 (PP)        │          │   单元操作 (UO)        │
│                      │          │                      │
│  职责：回答"是什么"    │          │  职责：回答"做什么"     │
│  - 密度、焓、熵...    │          │  - 换热、精馏、反应... │
│  - 给 Material 对象   │          │  - 操作 Material 对象  │
└──────────────────────┘          └──────────────────────┘
         │                                  │
         └────────────┬─────────────────────┘
                      ▼
              ┌──────────────┐
              │   PME (COFE) │
              │  流程模拟环境  │
              └──────────────┘
```

- **物性包**：纯计算组件，PME 问"这个 T/P 下密度是多少"，PP 返回数值
- **单元操作**：流程组件，有**端口**（Ports）和**参数**（Parameters），PME 连接物料流股后调用 `Validate` → `Calculate`

### 1.2 单元操作 vs 物性包

| 维度 | 物性包 (PP) | 单元操作 (UO) |
|------|-----------|-------------|
| **输入** | Material 对象（T/P/组成） | 物料端口 + 参数 |
| **输出** | 数值（密度、焓...） | 修改出口 Material 对象 |
| **有端口吗** | 无 | 有（Inlet/Outlet） |
| **有参数吗** | 可选 | 有（如换热器冷/热侧选择） |
| **核心方法** | `CalcSinglePhaseProp` | `Validate` + `Calculate` |
| **典型实现** | 1 类 8 Adapter | 1 主类 + N 辅助类（端口、参数、校验、求解） |

### 1.3 5 个必需的 Adapter

一个 COBIA 单元操作必须继承以下 5 个 Adapter：

| # | Adapter | 对应 CAPE-OPEN 接口 | 作用 |
|---|---------|-------------------|------|
| 1 | `CapeIdentificationAdapter` | ICapeIdentification | 名称和描述 |
| 2 | `CapeUnitAdapter` | ICapeUnit | 端口集合、验证、计算 |
| 3 | `CapeUtilitiesAdapter` | ICapeUtilities | 参数集合、Initialize/Terminate |
| 4 | `CapePersistAdapter` | ICapePersist | 保存/加载状态 |
| 5 | `CapeOpenObject` | IUnknown 基础 | COBIA 对象基类 |

> 对比物性包的 8 个 Adapter，单元操作只需要 5 个，因为单元操作不直接计算物性——它通过 Material 对象委托给物性包。

---

## 二、核心概念

### 2.1 端口（Port）

端口是单元操作与外部世界的连接点。MHExch 有 10 个物料端口（5 进 5 出）：

```cpp
// MaterialPort.h — 物料端口的最小实现
class MaterialPort :
    public CapeOpenObject<MaterialPort>,
    public CapeIdentificationAdapter<MaterialPort>,
    public CapeUnitPortAdapter<MaterialPort>
{
    CapeStringImpl portName;
    CapePortDirection direction;   // CAPE_INLET 或 CAPE_OUTLET
    CapeBoolean primary;           // 是否必须连接
    CapeThermoMaterial connectedMaterial;

public:
    // ICapeIdentification
    void getComponentName(CapeString name) { name = portName; }

    // ICapeUnitPort
    CapePortType getPortType()   { return CAPE_MATERIAL; }
    CapePortDirection getDirection() { return direction; }
    CapeInterface getConnectedObject() { return connectedMaterial; }

    void Connect(CapeInterface objectToConnect) {
        connectedMaterial = objectToConnect;
        unitValidationStatus = CAPE_NOT_VALIDATED;
    }
    void Disconnect() {
        connectedMaterial.clear();
        unitValidationStatus = CAPE_NOT_VALIDATED;
    }
};
```

三种端口类型：
- `CAPE_MATERIAL` — 物料流股（最常用）
- `CAPE_ENERGY` — 能量流股（热量/功）
- `CAPE_INFORMATION` — 信息流股

端口方向：
- `CAPE_INLET` — 进料
- `CAPE_OUTLET` — 出料

端口必要性：
- `primary=true` — 必须连接，否则验证失败
- `primary=false` — 可选连接

### 2.2 参数（Parameter）

参数让用户在 PME 中配置单元操作的行为。MHExch 有 5 个参数控制每路进料的冷/热侧：

```cpp
// 参数类型继承链（以字符串参数为例）
class ParameterOption :
    public CapeIdentificationAdapter,   // 名称
    public CapeParameterAdapter,        // 类型/模式/验证
    public CapeParameterSpecificationAdapter,  // 默认值/上下界
    public CapeStringParameterAdapter,  // getValue/putValue
    public CapeStringParameterSpecificationAdapter  // 选项列表
{
    CapeStringImpl value, defaultValue;
    CapeArrayStringImpl optionNames;    // {"Ignore", "Hot", "Cold"}
};
```

三种参数类型：
| 类型 | Adapter | 示例 |
|------|---------|------|
| Real | `CapeRealParameterAdapter` | 温度设定值、压降 |
| Integer | `CapeIntegerParameterAdapter` | 理论板数 |
| String/Option | `CapeStringParameterAdapter` | 冷/热侧选择 |

### 2.3 集合（Collection）

端口和参数通过集合暴露给 PME。COBIA 提供泛型模板简化集合实现：

```cpp
template <typename Interface, typename Item>
class Collection :
    public CapeIdentificationAdapter<Collection<Interface, Item>>,
    public CapeCollectionAdapter<Interface, Collection<Interface, Item>>
{
    std::vector<Item> items;

    Interface Item(CapeInteger index) { return items[index]; }
    Interface Item(CapeString name) {
        for (auto& item : items)
            if (getName(item) == name) return item;
        throw cape_open_error(COBIAERR_NoSuchItem);
    }
    CapeInteger getCount() { return items.size(); }
};

using PortCollectionPtr = CollectionPtr<CapeUnitPort, MaterialPortPtr>;
using ParameterCollectionPtr = CollectionPtr<CapeParameter, CapeParameter>;
```

关键操作：
- `addItem(item)` — 在构造函数中注册所有端口和参数
- `Item(index)` — PME 按索引遍历
- `Item(name)` — PME 按名称查找

### 2.4 生命周期：验证 → 计算

单元操作的运行分为两个阶段：

```
用户操作              PME 调用              UO 实现
─────────           ──────────            ────────
连接流股            Connect()
修改参数            putValue()            → 置 validationStatus=NOT_VALIDATED
点击运行            Validate(message)     → 检查端口连接、参数合法性
                                         → 返回 true/false
                    Calculate()           → 从 inlet 读取 T/P/组成
                                         → 计算 → 写入 outlet
```

**关键规则**：
1. `Validate` 只在状态为 `NOT_VALIDATED` 时执行，通过后置 `CAPE_VALID`
2. 任何端口变化或参数变化都会重置状态为 `NOT_VALIDATED`
3. `Calculate` 前必须检查 `validationStatus == CAPE_VALID`
4. 禁止修改进料 Material 对象（无副作用原则）

---

## 三、从零创建 COBIA 单元操作

### 3.1 环境准备

| 组件 | 路径/来源 |
|------|----------|
| **COBIA SDK** | `C:\Program Files (x86)\Common Files\CAPE-OPEN Laboratories Network\Include\` |
| **cobiaRegister.exe** | `C:\Program Files\Common Files\CAPE-OPEN Laboratories Network\COBIA\` |
| **MSYS2 MinGW-w64** | `D:\msys64\mingw64\bin\` |
| **CMake** | 3.16+ |
| **COFE** | https://www.amsterchem.com/cofe.html |

验证：

```powershell
Test-Path "C:\Program Files (x86)\Common Files\CAPE-OPEN Laboratories Network\Include\COBIA.h"
& "D:\msys64\mingw64\bin\g++.exe" --version
```

### 3.2 项目文件结构

以下是一个**单文件最小实现**的推荐结构。与物性包一样，单元操作也可以把所有逻辑放在 2 个文件中：

```
MyUnit/
├── CMakeLists.txt        # 构建配置
├── MyUnit.h              # 类声明 + 5 个 Adapter 继承 + 端口/参数/验证/计算
├── MyUnit.cpp            # Register 函数 + COBIA 入口
└── Engine.h              # 计算引擎（纯数学，无 COBIA 依赖，可选）
```

> **设计原则**：计算引擎与 CAPE-OPEN 接口完全解耦，可以单独编译为命令行工具进行单元测试。

### 3.3 Step 1：CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.16)
project(MyUnit VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(MyUnit SHARED
    MyUnit.cpp
)

target_include_directories(MyUnit PRIVATE
    ${COBIA_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_definitions(MyUnit PRIVATE COBIA_NOAUTOLINK)

if(WIN32)
    target_link_libraries(MyUnit PRIVATE
        advapi32 shell32 shlwapi user32 ole32 oleaut32 uuid)
endif()

if(MINGW)
    target_link_options(MyUnit PRIVATE
        -static-libgcc -static-libstdc++
        -Wl,-Bstatic,--whole-archive -lwinpthread
        -Wl,--no-whole-archive,-Bdynamic)
endif()

set_target_properties(MyUnit PROPERTIES PREFIX "" SUFFIX ".dll")

# 安装到 install/ChemUnit/
install(TARGETS MyUnit
    RUNTIME DESTINATION ChemUnit
    LIBRARY DESTINATION ChemUnit)
```

### 3.4 Step 2：最小头文件（MyUnit.h）

下面的模板实现了一个**最简单的单元操作**：1 个进料口 + 1 个出料口，不做任何计算，出口 = 入口的一个副本。

```cpp
#pragma once
#include <COBIA.h>

#ifdef _WIN64
#define unitName COBIATEXT("My Unit x64")
#define unitUUID 0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,\
                 0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F
#else
#define unitName COBIATEXT("My Unit x86")
#define unitUUID 0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,\
                 0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0E
#endif
#define unitDesc COBIATEXT("Minimal Unit Operation")

using namespace COBIA;

class MyUnit :
    public CapeOpenObject<MyUnit>,
    public CAPEOPEN_1_2::CapeIdentificationAdapter<MyUnit>,
    public CAPEOPEN_1_2::CapeUnitAdapter<MyUnit>,
    public CAPEOPEN_1_2::CapeUtilitiesAdapter<MyUnit>,
    public CAPEOPEN_1_2::CapePersistAdapter<MyUnit>
{
    // --- 成员变量 ---
    CapeStringImpl name, description;
    CapeBoolean dirty;
    CapeValidationStatus validationStatus;

    // 端口
    CapeStringImpl inName, outName;
    CapeThermoMaterial inlet, outlet;

    // 集合
    std::vector<CapeUnitPort> portList;
    std::vector<CapeParameter> paramList;

public:
    static const CapeUUID getObjectUUID() {
        return CapeUUID{ unitUUID };
    }

    MyUnit() :
        name(unitName), description(unitDesc),
        dirty(false), validationStatus(CAPE_NOT_VALIDATED),
        inName(COBIATEXT("Inlet")), outName(COBIATEXT("Outlet"))
    {}

    // ================================================================
    // ICapeIdentification
    // ================================================================
    void getComponentName(CapeString name) { name = this->name; }
    void putComponentName(CapeString name) { this->name = name; dirty = true; }
    void getComponentDescription(CapeString d) { d = description; }
    void putComponentDescription(CapeString d) { description = d; dirty = true; }

    // ================================================================
    // ICapeUnit — 端口
    // ================================================================
    CapeCollection<CapeUnitPort> ports() {
        CapeArrayCollection<CapeUnitPort> coll;
        for (auto& p : portList) coll.addItem(p);
        return coll;
    }

    // ================================================================
    // ICapeUnit — 验证
    // ================================================================
    CapeValidationStatus getValStatus() { return validationStatus; }

    CapeBoolean Validate(CapeString message) {
        if (validationStatus == CAPE_VALID) return true;

        // 检查进料是否连接
        if (!inlet) {
            message = COBIATEXT("Inlet is not connected");
            validationStatus = CAPE_INVALID;
            return false;
        }
        // 检查进出料化合物列表一致
        CapeArrayStringImpl refIDs, ids, dummy1, dummy2, dummy3;
        CapeArrayRealImpl dummy4, dummy5;
        CAPEOPEN_1_2::CapeThermoCompounds compsIn(inlet);
        CAPEOPEN_1_2::CapeThermoCompounds compsOut(outlet);
        compsIn.GetCompoundList(refIDs, dummy1, dummy2, dummy4, dummy5, dummy3);
        compsOut.GetCompoundList(ids, dummy1, dummy2, dummy4, dummy5, dummy3);
        if (refIDs.size() != ids.size()) {
            message = COBIATEXT("Inlet/Outlet compound lists differ");
            validationStatus = CAPE_INVALID;
            return false;
        }

        validationStatus = CAPE_VALID;
        return true;
    }

    // ================================================================
    // ICapeUnit — 计算
    // ================================================================
    void Calculate() {
        if (validationStatus != CAPE_VALID)
            throw cape_open_error(COBIATEXT("Unit is not in a valid state"));

        // 复制进料到出料（最简实现：直通）
        outlet.CopyFromMaterial(inlet);

        // 在此添加你的计算逻辑...
    }

    // ================================================================
    // ICapeUtilities
    // ================================================================
    CapeCollection<CapeParameter> getParameters() {
        CapeArrayCollection<CapeParameter> coll;
        for (auto& p : paramList) coll.addItem(p);
        return coll;
    }
    void putSimulationContext(CapeSimulationContext) {}
    void Initialize() {}
    void Terminate() {
        inlet.clear();
        outlet.clear();
    }
    CapeEditResult Edit(CapeWindowId) {
        throw cape_open_error(COBIAERR_NotImplemented);
    }

    // ================================================================
    // ICapePersist — 保存/加载
    // ================================================================
    void Save(CapePersistWriter writer, CapeBoolean clearDirty) {
        writer.Add(ConstCapeString(COBIATEXT("name")), name);
        writer.Add(ConstCapeString(COBIATEXT("description")), description);
        if (clearDirty) dirty = false;
    }
    void Load(CapePersistReader reader) {
        reader.Get(ConstCapeString(COBIATEXT("name")), name);
        reader.Get(ConstCapeString(COBIATEXT("description")), description);
        dirty = false;
        validationStatus = CAPE_NOT_VALIDATED;
    }
    CapeBoolean getDirty() { return dirty; }

    // ================================================================
    // 注册信息
    // ================================================================
    static void Register(CapePMCRegistrar registrar) {
        registrar.putName(unitName);
        registrar.putDescription(unitDesc);
        registrar.putCapeVersion(COBIATEXT("1.1"));
        registrar.putComponentVersion(COBIATEXT("1.0.0"));
        registrar.putAbout(COBIATEXT("Minimal Unit Operation using COBIA."));
        registrar.addCatID(CAPEOPEN::categoryId_UnitOperation);
        registrar.addCatID(CAPEOPEN_1_2::categoryId_Component_1_2);
    }
};
```

### 3.5 Step 3：入口文件（MyUnit.cpp）

单元操作的 `.cpp` 文件非常简洁——只需要包含头文件并注册：

```cpp
#define COBIA_PMC_ENTRY_POINTS       // 生成 DllRegisterServer 等 COM 入口
#define PMC_REGISTERFORALLUSERS      // 注册到 HKLM（所有用户可见）
#include <COBIA_PMC.h>
#include "MyUnit.h"

COBIA_PMC_REGISTER(MyUnit);
```

几个**必须**知道的宏：
- `COBIA_PMC_ENTRY_POINTS` — 不定义则不会生成 `capeRegisterObjects` / `DllRegisterServer` 等导出函数，注册时会报 `entry point could not be located`
- `PMC_REGISTERFORALLUSERS` — 注册到 `HKLM` 而非 `HKCU`。缺少则链接时报 `undefined reference to isPMCRegistrationForAllUsers`
- `COBIA_PMC_REGISTER(ClassName)` — 宏自动关联 `ClassName::Register()` 和 `ClassName::getObjectUUID()`

### 3.6 Step 4：编译、安装、注册

```powershell
# 配置（使用项目已有的 CMakePresets）
cmake --preset mingw-minsizerel

# 仅编译 MyUnit
cmake --build build/mingw-minsizerel --target MyUnit

# 安装到 install/ChemUnit/
cmake --install build/mingw-minsizerel --config MinSizeRel

# 注册
& "C:\Program Files\Common Files\CAPE-OPEN Laboratories Network\COBIA\cobiaRegister.exe" `
    -a "install/ChemUnit/MyUnit.dll"

# 反注册（更新 DLL 前必须先执行）
& "C:\Program Files\Common Files\CAPE-OPEN Laboratories Network\COBIA\cobiaRegister.exe" `
    -u "install/ChemUnit/MyUnit.dll"
```

---

## 四、进阶：以 MHExch 为参考

### 4.1 MHExch 单类架构解读

MHExch 经过重构，采用与物性包一致的**单类模式**，将全部逻辑浓缩在 2 个文件中：

```
MHExch/
├── MHExch.h              ← 类声明 + 内联 getter + 辅助声明
└── MHExch.cpp            ← 全部实现 + COBIA 注册入口
```

**类关系图**：

```
┌──────────────────────────────────────────────┐
│         MHeatExchangerUnit                   │
│  (单核心类，直接继承 5 个 Adapter)             │
│                                              │
│  CapeOpenObject                               │
│  CapeIdentificationAdapter   ← 名称/描述       │
│  CapeUnitAdapter             ← 端口/验证/计算   │
│  CapeUtilitiesAdapter        ← 参数/初始化      │
│  CapePersistAdapter          ← 保存/加载        │
│                                              │
│  持有:                                        │
│    MaterialPort (×10)       ← 物料端口         │
│    PortCollection           ← 端口集合适配器    │
│                                              │
│  私有方法:                                    │
│    validateMaterialPorts()  ← 端口连接校验      │
│    validateMSHEXSides()     ← 冷/热侧校验       │
│    flashOutlet()            ← 出口闪蒸          │
└──────────────────────────────────────────────┘
```

**调用关系**：

```
Validate(message)
  └→ validateMaterialPorts()   // 主进料口必连 + 化合物一致性
  └→ validateMSHEXSides()      // 冷/热侧不能相同

Calculate()
  └→ flashOutlet(i) × nPorts  // 逐出口 T/P 闪蒸
      └→ CopyFromMaterial(inlet)
      └→ CalcEquilibrium(TP)
```

### 4.2 单类模式 vs 多文件拆分

单类模式将参数/校验/求解逻辑作为核心类的**私有方法**，而非独立类：

```cpp
class MHeatExchangerUnit : public /* 5 个 Adapter */ {
    // 校验：私有方法，不是 Validator 类
    CapeBoolean validateMaterialPorts(CapeString msg);
    CapeBoolean validateMSHEXSides(CapeString msg);

    // 求解：私有方法，不是 Solver 类
    void flashOutlet(size_t idx);

public:
    CapeBoolean Validate(CapeString msg) {  // 直接调用私有方法
        return validateMaterialPorts(msg) && validateMSHEXSides(msg);
    }
    void Calculate() {
        for (size_t i = 0; i < nPorts; i++) flashOutlet(i);
    }
};
```

| 维度 | 多文件拆分 | 单类模式 |
|------|-----------|---------|
| 文件数 | 7~11 个 | **2 个** (.h + .cpp) |
| 可读性 | 需跳转多个文件 | **一眼看清全貌** |
| 复用性 | Validator/Solver 可复用 | 仅复用拷贝代码 |
| 编译速度 | 改动小文件编译快 | 改一行全编译 |
| 适合规模 | 精馏塔、反应器等 | 换热器、泵、混合器等 |
| 物性包风格 | 不适用 | ✅ 与 IdealGasPP 一致 |
| **推荐** | 复杂单元 | **大多数情况** |

> 本项目的 MHExch 已从 11 文件重构为 2 文件单类模式。经验表明：对于换热器级别复杂度的模块，单类模式在减少过度设计、提升可读性方面效果显著。

### 4.3 添加真实计算：直通 → 换热器

最小模板中的 `Calculate()` 只是直通复制。要变成真正的换热器，核心逻辑是：

```cpp
void Calculate() {
    // 1. 从进料读取 T, P, 组成, 流量
    CapeReal T_in, P_in;
    CapeArrayReal X, flow;
    inlet.GetOverallTPFraction(T_in, P_in, X);
    inlet.GetOverallProp(COBIATEXT("totalFlow"),
        COBIATEXT("mole"), flow);

    // 2. 调用计算引擎（你的数学模型）
    CapeReal T_out = myHeatExchangerModel(T_in, P_in, flow[0], X);

    // 3. 写入出料
    outlet.SetOverallTPFraction(T_out, P_in, X);
    outlet.SetOverallProp(COBIATEXT("totalFlow"),
        COBIATEXT("mole"), flow);

    // 4. 闪蒸出料（如果物性包支持相平衡）
    CAPEOPEN_1_2::CapeThermoEquilibriumRoutine eq(outlet);
    eq.CalcEquilibrium(flashCond_TP, COBIATEXT(""));
}
```

### 4.4 带端口的完整构造函数

最小模板省略了端口的构造。完整版需要在构造函数中创建端口并注册：

```cpp
MyUnit() : name(unitName), description(unitDesc),
           dirty(false), validationStatus(CAPE_NOT_VALIDATED),
           inName(COBIATEXT("Inlet")), outName(COBIATEXT("Outlet"))
{
    // 创建端口
    inlet  = new MyMaterialPort(name, validationStatus,
               inName, CAPE_INLET, true);
    outlet = new MyMaterialPort(name, validationStatus,
               outName, CAPE_OUTLET, true);

    // 注册到端口列表（进料必须在出料前面，PME 按索引配对）
    portList.push_back(inlet);
    portList.push_back(outlet);
}
```

---

## 五、常见问题与调试

### 5.1 UUID 冲突

```powershell
# 症状：注册时报错 "already registered" 或 COFE 加载了旧 DLL
# 解决：生成新 UUID
powershell -c "[guid]::NewGuid()"
```

### 5.2 COFE 中看不到你的模块

检查清单：
1. DLL 是否注册成功？（`cobiaRegister -a` 输出 `OK`）
2. `categoryId_UnitOperation` 是否正确添加？
3. COFE 是否已重启？（COFE 在启动时扫描已注册组件）
4. 32位/64位匹配？（COFE 64位需要 64位 DLL）

### 5.3 Validate 每次都被调用

这是正常的。`Connect/Disconnect/putValue` 都会重置 `validationStatus` 为 `NOT_VALIDATED`，PME 随后会调用 `Validate`。确保 `Validate` 开头检查：

```cpp
if (validationStatus == CAPE_VALID) return true;  // 幂等
```

### 5.4 编译警告：`control reaches end of non-void function`

当函数有多个 `return` 路径但某一分支未覆盖时触发。确保所有分支都有返回值，或在循环外添加兜底 `return`。

### 5.5 COBIA vs 传统 COM 迁移

| 传统 COM | COBIA |
|----------|-------|
| `BSTR` / `CBStr` | `CapeStringImpl` / `CapeString` |
| `VARIANT` | `CapeValue` / `CapeArrayValue` |
| `HRESULT` 返回 | C++ 异常 `throw cape_open_error()` |
| `DllRegisterServer` | `cobiaRegister.exe -a` |
| ATL `CComObject` | `CapeOpenObject<T>` |
| `BEGIN_TRY`/`END_TRY` | 不需要 |

### 5.6 CMake 一键注册

在 CMakeLists.txt 中创建 `register_*` 自定义目标，使编译+安装+注册一步完成：

```cmake
set(MYUNIT_INSTALL_PATH
    "${CMAKE_SOURCE_DIR}/../install/ChemUnit/MyUnit.dll")

add_custom_target(register_MyUnit
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different
        "$<TARGET_FILE:MyUnit>"  "${MYUNIT_INSTALL_PATH}"
    COMMAND "${COBIA_REGISTER}" -a "${MYUNIT_INSTALL_PATH}"
    DEPENDS MyUnit
    COMMENT "Installing and registering MyUnit..."
)
```

使用：
```powershell
cmake --build build --target register_MyUnit
```

> `copy_if_different` 只在 DLL 有变化时才复制，避免不必要的文件时间戳更新。

### 5.7 Debug / Release 不同 UUID

调试时经常需要同时安装 Debug 和 Release 版本到 COFE。给两个版本不同的 UUID（末尾字节不同）即可共存：

```cpp
#ifdef _DEBUG
  #ifdef _WIN64
    #define unitUUID 0xAA,...,0xA9   // Debug x64
  #else
    #define unitUUID 0xAA,...,0xA8   // Debug x86
  #endif
#else
  #ifdef _WIN64
    #define unitUUID 0xAA,...,0xFF   // Release x64
  #else
    #define unitUUID 0xAA,...,0xAA   // Release x86
  #endif
#endif
```

COFE 会将其视为 4 个独立组件，你可以同时对比调试版和发布版的行为。

### 5.8 关键资源

| 资源 | 链接 |
|------|------|
| COBIA SDK 下载 | https://colan.repositoryhosting.com/trac/colan_cobia/downloads |
| CO-LaN 官网 | https://www.colan.org/ |
| COFE 下载 | https://www.amsterchem.com/cofe.html |
| 本项目的物性包开发指南 | `../ChemProp/docs/COBIA物性包开发指南.md` |
| 本项目 ChemLab 路线图 | `../../Docs/ChemLab_Road_Map.md` |