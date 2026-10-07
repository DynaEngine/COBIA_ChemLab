# COBIA 物性包开发指南

> 以本项目的 `WaterPP`（IAPWS-97 纯水物性包）和 `IdealGasPP`（多组分理想气体物性包）为双参考示例。读完本文后，你应能独立编写、调试并注册一个可被 COFE 加载的 COBIA 物性包——无论它是单组分还是多组分。

---

## 一、背景：什么是 COBIA

### 1.1 CAPE-OPEN 简介

**CAPE-OPEN**（Computer-Aided Process Engineering Open Interface）是一套流程模拟软件的互操作标准。它定义了一系列 COM 接口，让不同厂商的物性包（Property Package）、单元操作（Unit Operation）和求解器可以在同一个流程模拟环境（PME，如 COFE、Aspen Plus）中协同工作。

核心角色：

```
┌──────────────┐     ┌─────────────────────┐     ┌──────────────────┐
│   PME (COFE) │────▶│ PropertyPackage (PP) │────▶│ Material Object  │
│ 流程模拟环境  │     │ 物性包 (本指南主角)    │     │ 物料对象          │
└──────────────┘     └─────────────────────┘     └──────────────────┘
       │                        │
       ▼                        ▼
┌──────────────┐     ┌─────────────────────┐
│  Unit Op     │     │ 物性引擎 (IAPWS-97)  │
│ 单元操作     │     │ 纯数学计算，无接口依赖  │
└──────────────┘     └─────────────────────┘
```

PME 通过 CAPE-OPEN 标准接口向 PP 查询物性，PP 通过 Material 对象读取 T/P/组成，返回计算结果。

### 1.2 COBIA vs 传统 COM

| 维度 | 传统 COM（如原始 Water 包） | COBIA SDK（WaterPP / IdealGasPP） |
|------|--------------------------|---------------------|
| **编译工具链** | Visual C++ / MSVC only | MinGW-w64 / CMake（跨平台） |
| **COM 实现** | ATL + IDispatch + VARIANT（手写） | `CapeInterfaceAdapters`（模板自动生成） |
| **注册机制** | `regsvr32 / DllRegisterServer` | `cobiaRegister.exe -a` |
| **字符串** | `BSTR` / `CBStr` | `CapeStringImpl` / `ICapeString` |
| **错误处理** | `HRESULT` + `BEGIN_TRY`/`END_TRY` 宏 | C++ 异常 `throw cape_open_error()` |
| **代码量** | ~8000 行（含大量 COM 样板） | WaterPP ~1200 行 / IdealGasPP ~750 行 |

> COBIA 的核心价值：**自动生成 COM 样板代码**。你只需关注物性计算逻辑，不用手写 `IDispatch::Invoke`、`VARIANT` 转换、`DllRegisterServer` 等。

### 1.3 单组分 vs 多组分

本指南覆盖两种最常见的物性包类型：

| 维度 | 单组分（WaterPP） | 多组分（IdealGasPP） |
|------|------------------|---------------------|
| **化合物数** | 1（Water） | N（N₂, O₂, CO₂...） |
| **相数** | 2（Vapor + Liquid） | 1（Vapor only） |
| **物性引擎** | IAPWS-97 方程组 | Cp 多项式 + 理想气体定律 |
| **组分管理** | 无需 | `std::vector<CompoundData>` |
| **闪蒸复杂度** | 需算相平衡（Psat/Tsat） | 仅气相，无相变 |
| **校验关键** | phaseFraction 范围 | 组分 fraction sum≈1 |

### 1.4 8 个必需的 Adapter

一个 COBIA 物性包必须继承以下 8 个 Adapter（CRTP 模式）：

| # | Adapter | 对应 CAPE-OPEN 接口 | 作用 |
|---|---------|-------------------|------|
| 1 | `CapeThermoPropertyPackageManagerAdapter` | ICapeThermoPropertyPackageManager | PME 通过它获取 PP 实例 |
| 2 | `CapeThermoMaterialContextAdapter` | ICapeThermoMaterialContext | 接收/释放 Material 对象 |
| 3 | `CapeThermoCompoundsAdapter` | ICapeThermoCompounds | 化合物列表、常数、T/P 依赖属性 |
| 4 | `CapeThermoPhasesAdapter` | ICapeThermoPhases | 相定义（Vapor/Liquid） |
| 5 | `CapeThermoPropertyRoutineAdapter` | ICapeThermoPropertyRoutine | 单相/两相物性计算 |
| 6 | `CapeThermoEquilibriumRoutineAdapter` | ICapeThermoEquilibriumRoutine | 相平衡/闪蒸计算 |
| 7 | `CapeThermoUniversalConstantAdapter` | ICapeThermoUniversalConstant | 通用常数（R, Avogadro...） |
| 8 | `CapeUtilitiesAdapter` | ICapeUtilities | Initialize/Terminate/Edit |

缺少任何一个都会导致编译失败。COBIA SDK 自带的 `CapeWizard.exe` 可以生成包含全部 8 个 Adapter 的模板代码。

---

## 二、从零创建 COBIA 物性包

### 2.1 环境准备

| 组件 | 用途 | 路径/来源 |
|------|------|----------|
| **COBIA SDK** | 头文件 + 注册工具 | `C:\Program Files (x86)\Common Files\CAPE-OPEN Laboratories Network\Include\` |
| **cobiaRegister.exe** | DLL 注册/反注册 | `C:\Program Files\Common Files\CAPE-OPEN Laboratories Network\COBIA\` |
| **MSYS2 MinGW-w64** | GCC 编译器 | `D:\msys64\mingw64\bin\` |
| **CMake** | 构建系统 | 3.16+ |
| **COFE** | 测试用 PME | https://www.amsterchem.com/cofe.html |

验证环境：

```powershell
Test-Path "C:\Program Files (x86)\Common Files\CAPE-OPEN Laboratories Network\Include\COBIA.h"
& "D:\msys64\mingw64\bin\g++.exe" --version
Test-Path "C:\Program Files\Common Files\CAPE-OPEN Laboratories Network\COBIA\cobiaRegister.exe"
```

### 2.2 项目文件结构

```
MyPP/
├── CMakeLists.txt        # 构建配置
├── MyPP.h                # 类声明 + 8 个 Adapter 继承
├── MyPP.cpp              # 所有接口实现
└── Engine.h/cpp          # 物性引擎（纯数学，无 COBIA 依赖）
```

**设计原则**：物性引擎（Engine）与 CAPE-OPEN 接口**完全解耦**。Engine 只负责数学计算，不依赖任何 COM/COBIA 头文件。这让你可以单独编译 Engine 为命令行工具进行单元测试。

### 2.3 Step 1：CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.16)
project(MyPP VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(COBIA_INC "C:/Program Files (x86)/Common Files/CAPE-OPEN Laboratories Network/Include")

add_library(MyPP SHARED MyPP.cpp Engine.cpp)
target_include_directories(MyPP PRIVATE ${COBIA_INC} .)
target_compile_definitions(MyPP PRIVATE COBIA_NOAUTOLINK)

# MinGW 静态链接
if(MINGW)
    target_link_options(MyPP PRIVATE
        -static-libgcc -static-libstdc++
        -Wl,-Bstatic,--whole-archive -lwinpthread
        -Wl,--no-whole-archive,-Bdynamic)
endif()
target_link_libraries(MyPP PRIVATE advapi32 shell32 user32 ole32 oleaut32 uuid)
set_target_properties(MyPP PROPERTIES PREFIX "" SUFFIX ".dll")
```

### 2.4 Step 2：头文件（MyPP.h）— 最小模板

```cpp
#pragma once
#include <COBIA.h>
#include <CapeInterfaces_1_2.h>
#include <CapeInterfaceAdapters_1_2.h>

namespace MyNS {

class MyPP :
    public COBIA::CapeOpenObject<MyPP>,
    public COBIA::CapeThermoPropertyPackageManagerAdapter<MyPP>,
    public COBIA::CapeThermoMaterialContextAdapter<MyPP>,
    public COBIA::CapeThermoCompoundsAdapter<MyPP>,
    public COBIA::CapeThermoPhasesAdapter<MyPP>,
    public COBIA::CapeThermoPropertyRoutineAdapter<MyPP>,
    public COBIA::CapeThermoEquilibriumRoutineAdapter<MyPP>,
    public COBIA::CapeThermoUniversalConstantAdapter<MyPP>,
    public COBIA::CapeUtilitiesAdapter<MyPP>
{
public:
    MyPP();
    ~MyPP();

    // === PropertyPackageManager ===
    void getPropertyPackageList(COBIA::CapeArrayString names);
    COBIA::CapeInterface GetPropertyPackage(COBIA::CapeString name);

    // === MaterialContext ===
    void SetMaterial(CapeThermoMaterial mat);
    void UnsetMaterial();

    // === Compounds ===
    COBIA::CapeInteger getNumCompounds();
    void GetCompoundList(COBIA::CapeArrayString ids, COBIA::CapeArrayString formulas,
        COBIA::CapeArrayString names, COBIA::CapeArrayReal boilT,
        COBIA::CapeArrayReal mw, COBIA::CapeArrayString cas);
    void getConstPropList(COBIA::CapeArrayString props);
    void GetCompoundConstant(COBIA::CapeArrayString props,
        COBIA::CapeArrayString compIds, COBIA::CapeBoolean& missing,
        COBIA::CapeArrayValue vals);
    void getTDependentPropList(COBIA::CapeArrayString props);
    void GetTDependentProperty(COBIA::CapeArrayString props, COBIA::CapeReal T,
        COBIA::CapeArrayString compIds, COBIA::CapeBoolean& missing,
        COBIA::CapeArrayReal vals);
    void getPDependentPropList(COBIA::CapeArrayString props);
    void GetPDependentProperty(COBIA::CapeArrayString props, COBIA::CapeReal P,
        COBIA::CapeArrayString compIds, COBIA::CapeBoolean& missing,
        COBIA::CapeArrayReal vals);

    // === Phases ===
    COBIA::CapeInteger getNumPhases();
    void GetPhaseList(COBIA::CapeArrayString labels, COBIA::CapeArrayString states,
        COBIA::CapeArrayString keyIds, COBIA::CapeArrayString exclIds,
        COBIA::CapeArrayString density, COBIA::CapeArrayString user,
        COBIA::CapeArrayEnumeration solidTypes);
    void GetPhaseInfo(COBIA::CapeString label, COBIA::CapeString attr,
        COBIA::CapeValue value);

    // === PropertyRoutine ===
    void CalcAndGetLnPhi(COBIA::CapeString phase, COBIA::CapeReal T,
        COBIA::CapeReal P, COBIA::CapeArrayReal x, COBIA::CapeInteger flags,
        COBIA::CapeArrayReal lnPhi, COBIA::CapeArrayReal lnPhiDT,
        COBIA::CapeArrayReal lnPhiDP, COBIA::CapeArrayReal lnPhiDn);
    void getSinglePhasePropList(COBIA::CapeArrayString props);
    COBIA::CapeBoolean CheckSinglePhasePropSpec(COBIA::CapeString prop,
        COBIA::CapeString phaseLabel);
    void CalcSinglePhaseProp(COBIA::CapeArrayString props,
        COBIA::CapeString phaseLabel);
    void getTwoPhasePropList(COBIA::CapeArrayString props);
    COBIA::CapeBoolean CheckTwoPhasePropSpec(COBIA::CapeString prop,
        COBIA::CapeArrayString phaseLabels);
    void CalcTwoPhaseProp(COBIA::CapeArrayString props,
        COBIA::CapeArrayString phaseLabels);

    // === EquilibriumRoutine ===
    COBIA::CapeBoolean CheckEquilibriumSpec(
        COBIA::CapeArrayString spec1, COBIA::CapeArrayString spec2,
        COBIA::CapeString solutionType);
    void CalcEquilibrium(COBIA::CapeArrayString spec1,
        COBIA::CapeArrayString spec2, COBIA::CapeString solutionType);

    // === UniversalConstant ===
    void getUniversalConstantList(COBIA::CapeArrayString list);
    void GetUniversalConstant(COBIA::CapeArrayString props,
        COBIA::CapeArrayValue values);

    // === Utilities ===
    COBIA::CapeEditResult Edit(COBIA::CapeWindowId);
    void Initialize();
    void Terminate();
    COBIA::CapeCollection<COBIA::CapeParameter> getParameters();
    void putSimulationContext(COBIA::CapeSimulationContext);

private:
    CapeThermoMaterial material;
    bool hasMaterial = false;
    bool compoundsChecked = false;

    static const COBIA::CapeUUID getObjectUUID() {
        return COBIA::CapeUUID{{0xA1,0xB2,0xC3,0xD4,0xE5,0xF6,0x07,0x18,
                                 0x29,0x3A,0x4B,0x5C,0x6D,0x7E,0x8F,0x90}};
    }
};

} // namespace MyNS
```

> ⚠️ GUID 必须全局唯一。开发阶段可用固定值，**发布前必须生成新的**。重复 GUID 会导致 COBIA 注册冲突。

### 2.5 Step 3：最小实现（MyPP.cpp）— 单相理想气体

下面是一个**只支持单相理想气体**的最小实现。它在 COFE 中加载后可以计算 density 和 enthalpy：

```cpp
#include "MyPP.h"
#include "Engine.h"   // 你的物性引擎
#include <cmath>

namespace MyNS {

// ========== 1. PropertyPackageManager ==========
void MyPP::getPropertyPackageList(COBIA::CapeArrayString names) {
    names[0] = COBIATEXT("MyPP");
}
COBIA::CapeInterface MyPP::GetPropertyPackage(COBIA::CapeString) {
    return static_cast<CapeThermoPropertyPackageManager*>(new MyPP());
}

// ========== 2. MaterialContext ==========
void MyPP::SetMaterial(CapeThermoMaterial mat) {
    material = mat;
    hasMaterial = true;
    compoundsChecked = false;
}
void MyPP::UnsetMaterial() { hasMaterial = false; }

// ========== 3. Compounds ==========
MyPP::MyPP() {}
MyPP::~MyPP() {}

COBIA::CapeInteger MyPP::getNumCompounds() { return 1; }

void MyPP::GetCompoundList(COBIA::CapeArrayString ids,
    COBIA::CapeArrayString formulas, COBIA::CapeArrayString names,
    COBIA::CapeArrayReal boilT, COBIA::CapeArrayReal mw,
    COBIA::CapeArrayString cas)
{
    ids[0]      = COBIATEXT("MyGas");
    formulas[0] = COBIATEXT("MG");
    names[0]    = COBIATEXT("MyGas");
    boilT[0]    = 100.0;
    mw[0]       = 28.0;
    cas[0]      = COBIATEXT("");
}

void MyPP::getConstPropList(COBIA::CapeArrayString props) {
    const CapeCharacter* list[] = {
        COBIATEXT("molecularWeight"),
        COBIATEXT("criticalTemperature"),
        COBIATEXT("criticalPressure"),
    };
    props.setsize(3);
    for (int i = 0; i < 3; ++i)
        props[i] = list[i];
}

void MyPP::GetCompoundConstant(COBIA::CapeArrayString props,
    COBIA::CapeArrayString, COBIA::CapeBoolean& missing,
    COBIA::CapeArrayValue vals)
{
    missing = false;
    for (size_t i = 0; i < props.size(); ++i) {
        std::wstring pn = static_cast<std::wstring>(props[i]);
        if (pn == L"molecularWeight")
            vals[i].Set(CapeDouble(28.0));
        else if (pn == L"criticalTemperature")
            vals[i].Set(CapeDouble(500.0));
        else if (pn == L"criticalPressure")
            vals[i].Set(CapeDouble(3.5e6));
        else
            missing = true;
    }
}

void MyPP::getTDependentPropList(COBIA::CapeArrayString) {}
void MyPP::GetTDependentProperty(COBIA::CapeArrayString, COBIA::CapeReal,
    COBIA::CapeArrayString, COBIA::CapeBoolean& missing, COBIA::CapeArrayReal)
{
    missing = true;
}
void MyPP::getPDependentPropList(COBIA::CapeArrayString props) {
    props.setsize(0);  // 必须显式设空
}
void MyPP::GetPDependentProperty(COBIA::CapeArrayString, COBIA::CapeReal,
    COBIA::CapeArrayString, COBIA::CapeBoolean& missing, COBIA::CapeArrayReal)
{
    missing = true;
}

// ========== 4. Phases ==========
COBIA::CapeInteger MyPP::getNumPhases() { return 1; }

void MyPP::GetPhaseList(COBIA::CapeArrayString labels,
    COBIA::CapeArrayString states, COBIA::CapeArrayString keyIds,
    COBIA::CapeArrayString exclIds, COBIA::CapeArrayString density,
    COBIA::CapeArrayString user, COBIA::CapeArrayEnumeration solidTypes)
{
    labels[0]  = COBIATEXT("Vapor");
    states[0]  = COBIATEXT("Vapor");
    keyIds[0]  = COBIATEXT("MyGas");
    exclIds[0] = COBIATEXT("");
    density[0] = COBIATEXT("");
    user[0]    = COBIATEXT("");
    solidTypes[0] = CapePhaseStatus(CAPE_NOTSOLID);
}

void MyPP::GetPhaseInfo(COBIA::CapeString label, COBIA::CapeString attr,
    COBIA::CapeValue value)
{
    std::wstring phase = static_cast<std::wstring>(label);
    std::wstring a     = static_cast<std::wstring>(attr);
    if (phase == L"Vapor") {
        if (a == L"StateOfAggregation")
            value.Set(COBIATEXT("Vapor"));
        else if (a == L"KeyCompoundId")
            value.Set(COBIATEXT("MyGas"));
        else
            throw COBIA::cape_open_error(COBIAERR_InvalidArgument);
    } else {
        throw COBIA::cape_open_error(COBIAERR_InvalidArgument);
    }
}

// ========== 5. PropertyRoutine — 核心物性计算 ==========
void MyPP::CalcAndGetLnPhi(COBIA::CapeString, COBIA::CapeReal,
    COBIA::CapeReal, COBIA::CapeArrayReal, COBIA::CapeInteger,
    COBIA::CapeArrayReal, COBIA::CapeArrayReal, COBIA::CapeArrayReal,
    COBIA::CapeArrayReal) {}

void MyPP::getSinglePhasePropList(COBIA::CapeArrayString props) {
    const CapeCharacter* list[] = {
        COBIATEXT("density"),
        COBIATEXT("enthalpy"),
    };
    props.setsize(2);
    for (int i = 0; i < 2; ++i) props[i] = list[i];
}

COBIA::CapeBoolean MyPP::CheckSinglePhasePropSpec(
    COBIA::CapeString, COBIA::CapeString) { return true; }

void MyPP::CalcSinglePhaseProp(COBIA::CapeArrayString props,
    COBIA::CapeString phaseLabel)
{
    if (!hasMaterial) return;

    // 1. 从相中读取 T/P
    COBIA::CapeStringImpl tempStr(COBIATEXT("temperature"));
    COBIA::CapeStringImpl presStr(COBIATEXT("pressure"));
    COBIA::CapeArrayRealImpl t(1);
    COBIA::CapeArrayRealImpl p(1);

    material.GetSinglePhaseProp(
        static_cast<ICapeString*>(&tempStr), phaseLabel,
        static_cast<ICapeString*>(nullptr),  // T/P 的 basis 必须是 nullptr
        static_cast<ICapeArrayReal*>(&t));
    material.GetSinglePhaseProp(
        static_cast<ICapeString*>(&presStr), phaseLabel,
        static_cast<ICapeString*>(nullptr),
        static_cast<ICapeArrayReal*>(&p));

    double T = t[0], P = p[0];

    // 2. 校验
    if (!std::isfinite(T) || T <= 0.0)
        throw COBIA::cape_open_error(COBIAERR_InvalidOperation);
    if (!std::isfinite(P) || P <= 0.0)
        throw COBIA::cape_open_error(COBIAERR_InvalidOperation);

    // 3. 调用物性引擎
    double density = P * 28.0e-3 / (8.314462618 * T);  // PM/RT
    double enthalpy = 1.005 * 28.0 * T;                 // Cp*M*T (J/mol)

    // 4. 写回 Material
    COBIA::CapeStringImpl propStr;
    COBIA::CapeStringImpl basisMole(COBIATEXT("mole"));
    COBIA::CapeArrayRealImpl val(1);

    for (size_t i = 0; i < props.size(); ++i) {
        std::wstring pn = static_cast<std::wstring>(props[i]);
        propStr = pn;
        if (pn == L"density") {
            val[0] = density;
            material.SetSinglePhaseProp(
                static_cast<ICapeString*>(&propStr), phaseLabel,
                static_cast<ICapeString*>(&basisMole),
                static_cast<ICapeArrayReal*>(&val));
        } else if (pn == L"enthalpy") {
            val[0] = enthalpy;
            material.SetSinglePhaseProp(
                static_cast<ICapeString*>(&propStr), phaseLabel,
                static_cast<ICapeString*>(&basisMole),
                static_cast<ICapeArrayReal*>(&val));
        }
    }
}

void MyPP::getTwoPhasePropList(COBIA::CapeArrayString props) {
    props.setsize(0);
}
COBIA::CapeBoolean MyPP::CheckTwoPhasePropSpec(
    COBIA::CapeString, COBIA::CapeArrayString) { return false; }
void MyPP::CalcTwoPhaseProp(COBIA::CapeArrayString, COBIA::CapeArrayString) {}

// ========== 6. EquilibriumRoutine ==========
COBIA::CapeBoolean MyPP::CheckEquilibriumSpec(
    COBIA::CapeArrayString, COBIA::CapeArrayString,
    COBIA::CapeString) { return true; }
void MyPP::CalcEquilibrium(COBIA::CapeArrayString,
    COBIA::CapeArrayString, COBIA::CapeString) {}

// ========== 7. UniversalConstant ==========
void MyPP::getUniversalConstantList(COBIA::CapeArrayString) {}
void MyPP::GetUniversalConstant(COBIA::CapeArrayString,
    COBIA::CapeArrayValue) {}

// ========== 8. Utilities ==========
COBIA::CapeEditResult MyPP::Edit(COBIA::CapeWindowId) {
    throw COBIA::cape_open_error(COBIAERR_NotImplemented);
}
void MyPP::Initialize() {}
void MyPP::Terminate() {}
COBIA::CapeCollection<COBIA::CapeParameter> MyPP::getParameters() {
    throw COBIA::cape_open_error(COBIAERR_NotImplemented);
}
void MyPP::putSimulationContext(COBIA::CapeSimulationContext) {}

// ========== Register ==========
static void Register(COBIA::CapePMCRegistrar registrar) {
    registrar.putName(COBIATEXT("MyPP"));
    registrar.putDescription(COBIATEXT("Minimal COBIA Property Package"));
    registrar.putCapeVersion(COBIATEXT("1.2"));
    registrar.putComponentVersion(COBIATEXT("1.0.0.0"));
    registrar.putProgId(COBIATEXT("MyNS.MyPP"));
    registrar.putVersionIndependentProgId(COBIATEXT("MyNS.MyPP"));
    registrar.addCatID(CAPEOPEN::categoryId_PropertyPackageManager);
}

} // namespace MyNS
```

### 2.6 Step 4：编译、注册、测试

```powershell
# 编译
$env:PATH = "D:\msys64\mingw64\bin;" + $env:PATH
cd build/mingw-release
cmake --build . --target MyPP

# 注册
& "C:\Program Files\Common Files\CAPE-OPEN Laboratories Network\COBIA\cobiaRegister.exe" ^
    -a "build/mingw-release/MyPP.dll"

# 反注册（更新 DLL 前必须先执行）
& "C:\Program Files\Common Files\CAPE-OPEN Laboratories Network\COBIA\cobiaRegister.exe" ^
    -u "build/mingw-release/MyPP.dll"
```

测试流程：**完全关闭 COFE → 编译 → 注册 → 打开 COFE → Configure → Property Packages → 添加 MyPP**

> ⚠️ **COM 缓存是新手最常见陷阱**：修改 DLL 后必须完全关闭 COFE 再重新打开，否则进程中的 COM 缓存仍会加载旧 DLL。

### 2.7 最小例子检查清单

填写下面清单确认你的实现是否完整：

| # | 检查项 | ✓ |
|---|--------|---|
| 1 | 8 个 Adapter 全部声明 | ☐ |
| 2 | `getPropertyPackageList` 返回包名 | ☐ |
| 3 | `GetPropertyPackage` 创建并返回 PP 实例 | ☐ |
| 4 | `getNumCompounds` 返回 ≥1 | ☐ |
| 5 | `GetCompoundList` 填写至少一个化合物 | ☐ |
| 6 | `getConstPropList` 至少返回 `molecularWeight` | ☐ |
| 7 | `GetCompoundConstant` 处理 `molecularWeight` | ☐ |
| 8 | `getPDependentPropList` 显式 `setsize(0)` | ☐ |
| 9 | `getNumPhases` 返回 ≥1 | ☐ |
| 10 | `GetPhaseList` 填写至少一个相 | ☐ |
| 11 | `getSinglePhasePropList` 至少返回 `"enthalpy"` | ☐ |
| 12 | `CalcSinglePhaseProp` 能用 `nullptr` 读 T/P 并写出结果 | ☐ |
| 13 | `CheckEquilibriumSpec` 返回 `true` | ☐ |
| 14 | `Register` 填写包名 + `addCatID` | ☐ |
| 15 | COFE 中能加载、能看到化合物和属性 | ☐ |

以上 15 项全部通过，你的 COBIA 物性包就站稳了脚跟。接下来可以逐步添加更多功能。

---

## 三、接口方法详解（以 WaterPP / IdealGasPP 为双参考）

每节按 "最短可用代码 → WaterPP 完整实现 → 常见错误" 三层递进。

### 3.1 化合物数据库（ICapeThermoCompounds）

**最小要求**：至少注册一种化合物，提供 `molecularWeight`。

**WaterPP 实现**：一种化合物 Water，12 个常数，10 个 T 依赖属性。

```cpp
// getConstPropList — 声明常数列表
void WaterPP::getConstPropList(COBIA::CapeArrayString props) {
    const CapeCharacter* list[] = {
        COBIATEXT("molecularWeight"),
        COBIATEXT("criticalTemperature"),
        COBIATEXT("criticalPressure"),
        COBIATEXT("criticalVolume"),
        COBIATEXT("acentricFactor"),
        COBIATEXT("boilingTemperature"),
        COBIATEXT("formationReactionHeat"),
        COBIATEXT("heatOfVaporization"),
        COBIATEXT("idealGasCpFactor"),
        COBIATEXT("dipoleMoment"),
        COBIATEXT("radiusOfGyration"),
        COBIATEXT("iupacName"),
    };
    props.setsize(12);
    for (int i = 0; i < 12; ++i) props[i] = list[i];
}

// GetCompoundConstant — 逐属性返回
void WaterPP::GetCompoundConstant(...) {
    for (size_t i = 0; i < props.size(); i++) {
        std::wstring pn = static_cast<std::wstring>(props[i]);
        if (pn == L"molecularWeight")
            vals[i].Set(CapeDouble(Water::MOLWT));       // 18.0153 g/mol
        else if (pn == L"criticalTemperature")
            vals[i].Set(CapeDouble(Water::TCRIT));        // 647.096 K
        else if (pn == L"criticalPressure")
            vals[i].Set(CapeDouble(Water::PCRIT * 1e6));  // 22.064e6 Pa
        else {
            auto it = constValues.find(pn);               // map 查找
            if (it != constValues.end())
                vals[i].Set(CapeDouble(it->second));
            else missing = true;
        }
    }
}
```

> 💡 使用 `std::map<std::wstring, double>` 存储次要常量，避免冗长的 if-else 链。

**T 依赖属性列表**（`getTDependentPropList`）：

| 属性 ID | 说明 |
|---------|------|
| `vaporPressure` | 饱和蒸气压 Pa |
| `surfaceTensionSatLiquid` | 表面张力 N/m |
| `thermalConductivityLiquid` / `Vapor` | 饱和线导热系数 W/(m·K) |
| `viscosityLiquid` / `Vapor` | 饱和线粘度 Pa·s |
| `volumeLiquid` / `volumeChangeVaporization` | 饱和线比容 / 蒸发比容差 m³/mol |
| `idealGasEnthalpy` / `idealGasEntropy` | 理想气体焓/熵 J/mol |

### 3.2 单相属性计算（CalcSinglePhaseProp）⭐ 最常用

PME 在每次迭代中调用此方法计算密度、焓、熵等单相物性。

**标准流程**（5 步）：

```
1. GetSinglePhaseProp("temperature", phaseLabel, nullptr) → T
2. GetSinglePhaseProp("pressure",    phaseLabel, nullptr) → P
3. isfinite(T) && T>0,  isfinite(P) && P>0 验证 → 失败即抛
4. Engine.compute(T, P) → 物性值
5. SetSinglePhaseProp(propName, phaseLabel, basis, value)
```

**单位转换 — WaterPP 的教训**：

Water 引擎内部单位是 **kJ/kg**，COBIA 要求 **mole basis (J/mol)**。

```
w.enthalpy()  = kJ/kg

J/mol = kJ/kg × 1000(J/kJ) × (MOLWT_g/1000)(kg/mol)
      = kJ/kg × MOLWT                    ← ×1000 和 ÷1000 抵消！
```

因此正确转换是 `w.enthalpy() * Water::MOLWT`（**不需要** `* 1e3`）。

> ⚠️ WaterPP 之前曾错误地写为 `w.enthalpy() * MOLWT * 1e3`，导致所有 thermo 属性放大 1000 倍。

**Basis 参数速查表**：

| 属性类型 | basis 参数 | 示例 |
|---------|:---------:|------|
| temperature | `nullptr` | `GetSinglePhaseProp("temperature", phase, nullptr, &t)` |
| pressure | `nullptr` | `GetSinglePhaseProp("pressure", phase, nullptr, &p)` |
| enthalpy/entropy/Cp/Cv... | `"mole"` | `SetSinglePhaseProp("enthalpy", phase, "mole", &v)` |
| thermalConductivity | `nullptr` | `SetSinglePhaseProp("thermalConductivity", phase, nullptr, &v)` |
| viscosity | `nullptr` | `SetSinglePhaseProp("viscosity", phase, nullptr, &v)` |
| fraction | `"mole"` | `GetSinglePhaseProp("fraction", phase, "mole", &v)` |

> ❌ **不要**传空字符串 `""` 作为 basis — 这会导致 "Invalid base specified"。
> ✅ **务必**对 T/P 传 `nullptr` — 这是 CAPE-OPEN 标准规定的无量纲属性。

**WaterPP 支持的 11 个单相属性**：

| 属性 | Water 引擎输入 | COBIA 输出 | 转换公式 |
|------|:---:|------|------|
| density | m³/kg | mol/m³ | `1 / (v × M × 1e-3)` |
| enthalpy | kJ/kg | J/mol | `h × M` |
| entropy | kJ/(kg·K) | J/(mol·K) | `s × M` |
| gibbsEnergy | kJ/kg | J/mol | `g × M` |
| heatCapacityCp | kJ/(kg·K) | J/(mol·K) | `cp × M` |
| heatCapacityCv | kJ/(kg·K) | J/(mol·K) | `cv × M` |
| internalEnergy | kJ/kg | J/mol | `u × M` |
| molecularWeight | — | g/mol | 直接 `Water::MOLWT` |
| thermalConductivity | W/(m·K) | W/(m·K) | 直接使用 |
| volume | m³/kg | m³/mol | `v × M × 1e-3` |
| viscosity | Pa·s | Pa·s | 直接使用 |

**多组分扩展**（IdealGasPP）：逐组分计算混合性质：

```cpp
// 读取组分 fractions
material.GetSinglePhaseProp(&fracStr, phaseLabel, &basisMole, &fractions);

double mixH = 0.0, mixCp = 0.0, mixDensity = 0.0;
for (size_t ci = 0; ci < compoundDB.size(); ++ci) {
    double xi = fractions[ci];
    if (xi <= 0.0) continue;

    double cp = comp.cpA + comp.cpB * T + comp.cpC * T * T + ...;
    double h = comp.H0 + (cp) * (T - Tref);       // J/mol
    double s0 = comp.S0 + cp * log(T / Tref);     // J/(mol·K)
    mixH  += xi * h;
    mixCp += xi * cp;
}
mixDensity = P / (8.314462618 * T) * molarMassMix;  // PM/RT
```

> 💡 理想气体混合性质 = Σ xᵢ × 纯组分性质，density 不过分子量混合。

### 3.3 相平衡/闪蒸（CalcEquilibrium）— 最复杂

COBIA `CapeThermoEquilibriumRoutineAdapter` 的内部调用顺序：

```
PME 调用 CalcEquilibrium(spec1, spec2, solutionType)
  └── COBIA Adapter 内部:
       ├─ ① CheckEquilibriumSpec(spec1, spec2, solutionType)
       │    └─ false → throw "CalcEquilibrium failed: Invalid operation"
       │               （CalcEquilibrium 不会被调用！）
       └─ ② CalcEquilibrium(spec1, spec2, solutionType)
            （仅在 ① 返回 true 时执行）
```

> ⚠️ **致命陷阱**：如果在 `CheckEquilibriumSpec` 中返回 `false`，`CalcEquilibrium` 根本不会执行。异常消息前缀却是 "CalcEquilibrium failed"，极易误导。WaterPP 改为 `return true` 后问题解决。

**Spec 解析**（5 个属性 × 9 种组合）：

```
specification[i] = [propertyName, basis, phase, compoundId]
                      [0]         [1]    [2]       [3]
```

| propertyName | 含义 | phase | 支持组合 |
|:-----------:|------|:----:|------|
| `temperature` | T | `overall` | TP, TVF, TH, TS |
| `pressure` | P | `overall` | TP, PVF, PH, PS |
| `phasefraction` | VF | `vapor`/`liquid` | TVF, PVF, HVF, SVF |
| `vaporfraction` | VF 别名 | `vapor`/`liquid` | 同上 |
| `enthalpy` | H | `overall` | PH, TH, HVF |
| `entropy` | S | `overall` | PS, TS, SVF |

9 种闪蒸的 spec 组合：

| 闪蒸 | spec1 | spec2 |
|:---:|-------|-------|
| **TP** | temperature | pressure |
| **TVF** | temperature | phasefraction |
| **PVF** | pressure | phasefraction |
| **PH** | pressure | enthalpy |
| **TH** | temperature | enthalpy |
| **PS** | pressure | entropy |
| **TS** | temperature | entropy |
| **HVF** | enthalpy | phasefraction |
| **SVF** | entropy | phasefraction |

**Spec 解析的安全写法**（大小写兼容 + 错误标签）：

```cpp
void WaterPP::CalcEquilibrium(
    COBIA::CapeArrayString specification1,
    COBIA::CapeArrayString specification2,
    COBIA::CapeString solutionType)
{
    if (!hasMaterial) return;

    // 1. 转换为小写（大小写兼容）
    std::wstring spec1 = static_cast<std::wstring>(specification1[0]);
    std::wstring spec2 = static_cast<std::wstring>(specification2[0]);
    for (auto& c : spec1) c = towlower(c);
    for (auto& c : spec2) c = towlower(c);

    // 2. 解析 spec，设置标志
    bool haveT = false, haveP = false, haveVF = false, haveH = false, haveS = false;
    for (int i = 0; i < 2; ++i) {
        std::wstring s     = (i == 0) ? spec1 : spec2;
        auto& specArr      = (i == 0) ? specification1 : specification2;
        std::wstring phase = static_cast<std::wstring>(specArr[2]);
        std::wstring basis = static_cast<std::wstring>(specArr[1]);
        for (auto& c : phase) c = towlower(c);  // ← 大小写兼容
        for (auto& c : basis) c = towlower(c);

        if (s == L"temperature") {
            if (phase != L"overall" || !basis.empty() || haveT)
                throw cape_open_error("EQ-T: invalid temperature spec");
            haveT = true;
        }
        else if (s == L"pressure") {
            if (phase != L"overall" || !basis.empty() || haveP)
                throw cape_open_error("EQ-P: invalid pressure spec");
            haveP = true;
        }
        else if (s == L"phasefraction" || s == L"vaporfraction") {
            // ↑ 注意：比较字符串必须和 towlower 后一致（全小写）
            if (phase != L"vapor" && phase != L"liquid")
                throw cape_open_error("EQ-VF-PHASE");
            haveVF = true;
        }
        else if (s == L"enthalpy")  { haveH = true; }
        else if (s == L"entropy")   { haveS = true; }
        else {
            std::wstring msg = L"EQ-UNKNOWN: " + s;  // ← 诊断标签
            throw cape_open_error(msg.c_str());
        }
    }

    // 3. 根据标志组合分发到对应闪存逻辑
    if      (haveT && haveP)  { /* TP  */ }
    else if (haveT && haveVF) { /* TVF */ }
    else if (haveP && haveVF) { /* PVF */ }
    else if (haveP && haveH)  { /* PH  */ }
    else if (haveT && haveH)  { /* TH  */ }
    else if (haveP && haveS)  { /* PS  */ }
    else if (haveT && haveS)  { /* TS  */ }
    else if (haveH && haveVF) { /* HVF */ }
    else if (haveS && haveVF) { /* SVF */ }
    else throw cape_open_error(COBIAERR_InvalidOperation);
}
```

### 3.4 通用常数（ICapeThermoUniversalConstant）

```cpp
void getUniversalConstantList(COBIA::CapeArrayString list) {
    const CapeCharacter* names[] = {
        COBIATEXT("universalGasConstant"),
        COBIATEXT("avogadroConstant"),
        COBIATEXT("boltzmannConstant"),
    };
    list.setsize(3);
    for (int i = 0; i < 3; ++i) list[i] = names[i];
}

void GetUniversalConstant(COBIA::CapeArrayString props, COBIA::CapeArrayValue vals) {
    for (size_t i = 0; i < props.size(); ++i) {
        std::wstring n = static_cast<std::wstring>(props[i]);
        if      (n == L"universalGasConstant") vals[i].Set(CapeDouble(8.314462618));
        else if (n == L"avogadroConstant")     vals[i].Set(CapeDouble(6.02214076e23));
        else if (n == L"boltzmannConstant")    vals[i].Set(CapeDouble(1.380649e-23));
    }
}
```

---

## 四、调试与排错 — 实战经验

### 4.1 错误标签系统（最重要的调试技巧）

COBIA 抛出的异常消息在 COFE 日志中显示。但如果你 20 个 `throw` 全都用相同的 `COBIAERR_InvalidOperation`，你无法判断是哪个位置出错。

**正确做法**：每个 `throw` 带上唯一标签：

```cpp
// ❌ 无标签 — 无法定位
throw cape_open_error(COBIAERR_InvalidOperation);

// ✅ 有标签 — 日志直接告诉你问题在哪
throw cape_open_error(COBIATEXT("EQ-T: invalid temperature spec"));
throw cape_open_error(COBIATEXT("EQ-FLASH-T: invalid T for TP flash"));
throw cape_open_error(COBIATEXT("EQ-UNKNOWN: ") + propertyName);
```

标签命名约定：`{模块}-{子模块}: {描述}`

| 标签 | 含义 |
|------|------|
| `EQ-T` | CalcEquilibrium — T spec 解析失败 |
| `EQ-READ-T` | CalcEquilibrium — T 值读取无效 |
| `EQ-FLASH-T` | CalcEquilibrium — TP flash T 无效 |
| `EQ-VF-PHASE` | CalcEquilibrium — phaseFraction 的 phase 不对 |
| `EQ-UNKNOWN: xxx` | CalcEquilibrium — 未知属性名（日志会显示属性名） |
| `EQ-SOL` | CalcEquilibrium — solutionType 不支持 |
| `EQ-COMPID` | CalcEquilibrium — compoundId 非空（纯组分不接受） |

### 4.2 COFE 日志解读

COFE 日志中的错误消息格式：

```
warning: Material object error in CalcEquilibrium:
CalcEquilibrium failed: in ICapeThermoEquilibriumRoutine::CalcEquilibrium
of WaterPPPropertyPackage: EQ-UNKNOWN: phasefraction
```

解读方法：

| 日志片段 | 告诉你的信息 |
|---------|------------|
| `in ICapeThermoEquilibriumRoutine::CalcEquilibrium` | 出错的接口和方法 |
| `of WaterPPPropertyPackage` | 出错的 PP 类 |
| `EQ-UNKNOWN: phasefraction` | 你的诊断标签：spec 属性名 "phasefraction" 未被识别 |
| `(4x)` | 同一错误连续出现了 4 次 |

> "phasefraction" 未识别 → 进入 else 分支抛出 `EQ-UNKNOWN` → 检查 spec 解析代码，发现比较字符串 `L"phaseFraction"`（大写F）和 towlower 后的 "phasefraction"（小写f）不匹配。

### 4.3 单位验证

COFE 输出物性表后，用少量手算验证单位是否正确：

以水在 99°C / 0.1 MPa 液态为例：

| 属性 | 近似理论值 | 如果显示 1000× | 问题 |
|------|:---:|:---:|------|
| enthalpy (J/mol) | ~7,550 | ~7,550,000 | `* MOLWT * 1e3` 多乘了 1000 |
| Cp (J/(mol·°C)) | ~76 | ~76,000 | 同上 |
| density (mol/m³) | ~53,200 | — | 这个一般正确 |
| viscosity (Pa·s) | ~2.8×10⁻⁴ | — | 这个一般正确 |

**快速心算验证法**：水 100°C 液态质量焓 ≈ 419 kJ/kg，摩尔质量 ≈ 0.018 kg/mol：
```
J/mol = 419 × 1000 × 0.018 = 7,542 J/mol
```
如果你的输出是 7,542,000，就知道多乘了 1000。

### 4.4 常见错误速查

| 症状 | 可能原因 | 检查 |
|------|---------|------|
| COFE 中看不到 PP | Register 未调用 addCatID | 确认 `registrar.addCatID(CAPEOPEN::categoryId_PropertyPackageManager)` |
| 加载后 T 变 100°C, P 变 1atm | try-catch 静默吞没异常使用默认值 | 移除所有 `catch(...){}` |
| "Invalid base specified" | T/P 的 basis 传了空字符串 | 改为 `nullptr` |
| "invalid basis for fraction" | fraction 的 basis 传了空字符串 | 改为 `"mole"` |
| "Invalid operation" (无标签) | CheckEquilibriumSpec 返回 false | 改为 `return true` |
| EQ-UNKNOWN (4x) | spec 属性名大小写不匹配 | spec 字符串和比较常量都做 `towlower` |
| 所有 thermo 值偏大 1000 倍 | `* MOLWT * 1e3` 写成了 `* MOLWT` 的正确形式 | 参考 3.2 节单位推导 |

### 4.5 Spec 解析大小写陷阱

不同 PME 发送的字符串大小写可能不同：

| PME 可能发送 | 你的代码中 | 后果 |
|:---:|:---:|------|
| `"overall"` | `L"Overall"` | phase 匹配失败 |
| `"temperature"` | `L"Temperature"` | 属性名匹配失败 |
| `"Unspecified"` | `L"unspecified"` | solutionType 匹配失败 |
| `"Normal"` | `L"normal"` | solutionType 匹配失败 |

**唯一解**：对**所有**参与比较的字符串都做 `towlower`，且**比较常量也用全小写**。

```cpp
// ✅ 全小写方案
std::wstring s = static_cast<std::wstring>(specArr[0]);
for (auto& c : s) c = towlower(c);       // 输入 → 小写
// ...
if (s == L"temperature") { ... }          // 常量也是小写
if (s == L"phasefraction") { ... }        // 常量也是小写（不能写成 phaseFraction）
if (s == L"vaporfraction") { ... }        // 常量也是小写
```

### 4.6 开发工作流

```
┌──────────────────────────────────────────────────┐
│  1. 关闭 COFE（COM 缓存必须释放）                    │
│  2. 编译 DLL                                       │
│  3. 反注册旧 DLL (cobiaRegister -u)                 │
│  4. 注册新 DLL (cobiaRegister -a)                   │
│  5. 打开 COFE → 添加 PP → 新建 Stream → 测试          │
│  6. 查看 COFE 日志（View → Log）                     │
│  7. 根据日志中的诊断标签定位问题                       │
│  8. 返回步骤 1                                      │
└──────────────────────────────────────────────────┘
```

> ⚠️ 如果只编译不注册，COFE 加载的还是旧 DLL。反注册再注册是最安全的方式。

---

## 五、通用实现模式

以下模式在 WaterPP 和 IdealGasPP 中均被验证有效，适用于任意 COBIA 物性包。

### 5.1 Adapter 继承全景

无论单组分还是多组分，必须继承同一套 8 个 Adapter：

```cpp
// 通用模板 — 将 WaterPPPropertyPackage 替换为你的类名
class YourPP :
    public COBIA::CapeOpenObject<YourPP>,
    public CapeThermoPropertyPackageManagerAdapter<YourPP>,  // ①
    public CapeThermoMaterialContextAdapter<YourPP>,        // ②
    public CapeThermoCompoundsAdapter<YourPP>,             // ③
    public CapeThermoPhasesAdapter<YourPP>,                // ④
    public CapeThermoPropertyRoutineAdapter<YourPP>,       // ⑤
    public CapeThermoEquilibriumRoutineAdapter<YourPP>,    // ⑥
    public CapeThermoUniversalConstantAdapter<YourPP>,     // ⑦
    public CapeUtilitiesAdapter<YourPP>;                   // ⑧
```

均为 CRTP 模式（`Adapter<T>`），Adapter 在编译期通过 `static_cast<T*>` 调用你的重写方法。不需要手动实现 `QueryInterface`、`AddRef`、`Release`。

### 5.2 化合物数据库模式（多组分专用）

IdealGasPP 使用 `std::vector<CompoundData>` 存储化合物参数，适用于 N 种组分的任意组合：

```cpp
struct CompoundData {
    std::wstring id;       // "Nitrogen"
    std::wstring formula;  // "N2"
    double mw;             // 28.0134 g/mol
    double cpA, cpB, cpC, cpD, cpE;  // Cp = A + B*T + C*T² + ...
    double H0;             // 标准焓 J/mol @ Tref
    double S0;             // 标准熵 J/(mol·K) @ Tref,Pref
};
std::vector<CompoundData> compoundDB;  // 化合物数据库
```

**组分校验函数** — 每次属性计算前验证 Material 中的 fraction 与数据库一致：

```cpp
void CheckCompounds() {
    if (compoundsChecked) return;
    // 从 Material 读取 fraction，验证 count 匹配 + sum≈1
    ...
    compoundsChecked = true;
}
```

> 💡 `CheckCompounds` 只在首次调用时执行，后续 `compoundsChecked=true` 跳过。UnsetMaterial 时重置。

### 5.3 内部枚举 — 避免字符串比较

```cpp
enum SinglePhaseProp {
    SPP_DENSITY, SPP_ENTHALPY, SPP_ENTROPY,
    SPP_GIBBS_ENERGY, SPP_HEAT_CAPACITY_CP, SPP_HEAT_CAPACITY_CV,
    SPP_INTERNAL_ENERGY, SPP_MOLECULAR_WEIGHT,
    SPP_THERMAL_CONDUCTIVITY, SPP_VOLUME, SPP_VISCOSITY
};

// 字符串 → 枚举映射
bool getSinglePhasePropEnum(const std::wstring& name, SinglePhaseProp& result) {
    if (name == L"density")           { result = SPP_DENSITY; return true; }
    if (name == L"enthalpy")          { result = SPP_ENTHALPY; return true; }
    // ...
    return false;
}
```

> 💡 只在入口处做一次字符串→枚举转换，后续 switch-case 直接比较整数，比反复比较字符串高效。

### 5.4 单位链全景图

```
             ┌─────────────────────────────────────┐
             │          COFE / PME                  │
             │   COBIA 接口，mole basis (J/mol)     │
             └──────────────┬──────────────────────┘
                            │
    GetOverallProp          │          SetSinglePhaseProp
    ("enthalpy","mass")     │          ("enthalpy","mole")
    → J/kg                  │          ← J/mol
            ↓               │               ↑
    ┌───────────────┐       │       ┌───────────────┐
    │  × 1e-3       │       │       │  × MOLWT      │
    │  J/kg→kJ/kg   │       │       │  kJ/kg→J/mol  │
    └───────┬───────┘       │       └───────┬───────┘
            ↓               │               ↑
    ┌───────────────────────────────────────────────┐
    │        Water 引擎 (IAPWS-97)                  │
    │        内部单位：kJ/kg, MPa, K                 │
    │        w.enthalpy() → kJ/kg                   │
    │        w.SetStatePH(P_MPa, kJ/kg)             │
    └───────────────────────────────────────────────┘
```

**关键转换点**：
- **入读**：Mass basis 读入 `GetOverallProp("enthalpy", "mass")` → J/kg → ×1e-3 → kJ/kg
- **出入桥**：IAPWS 内部是 kJ/kg
- **输出**：`w.enthalpy()` → kJ/kg → ×MOLWT → J/mol (mole basis)

### 5.5 CheckEquilibriumSpec 的正确策略

```cpp
COBIA::CapeBoolean CheckEquilibriumSpec(
    COBIA::CapeArrayString, COBIA::CapeArrayString, COBIA::CapeString)
{
    return true;  // 不做阻拦，验证逻辑放在 CalcEquilibrium 内部
}
```

原因：COBIA Adapter 在 `CalcEquilibrium` 之前调用 `CheckEquilibriumSpec`。如果此处返回 `false`，CalcEquilibrium 不会执行。首次调用时 PresentPhases 可能尚未设置，做 phase 检查会导致误判。

### 5.6 闪蒸分发模式

无论单组分还是多组分，`CalcEquilibrium` 都遵循同一模板：

```
1. 解析 spec1/spec2 → 设置 haveT/haveP/haveVF/haveH/haveS 标志
2. 验证 solutionType
3. 按 haveX 组合读取 T/P/fraction（TP 用 GetOverallTPFraction，其他用 GetOverallProp）
4. 分发闪蒸类型 → 计算 resultT / resultP / resultVapFrac
5. SetPresentPhases → SetSinglePhaseProp(T/P/fraction/phaseFraction) → SetOverallProp(T/P)
```

**TP flash vs VF-based flash 的分支逻辑**：

```cpp
// 单组分 2 相（WaterPP）：需算 Psat/Tsat 确定相边界
if (haveT && haveP) {      /* TP: 算相平衡，得 vapFrac */ }
else if (haveT && haveVF) { /* TVF: T 已知，P=Psat(T) */ }
else if (haveP && haveVF) { /* PVF: P 已知，T=Tsat(P) */ }

// 多组分 1 相（IdealGasPP）：无需相平衡，直接设全气相
if (haveT && haveP) { /* TP: 直接使用 T/P */ }
if (haveH && !haveT) { /* PH: T += (Htarget-Href)/Cp */ }
// VF-based flash 同单相，读取 VF 后直接写入
```

**写入 Material 的标准顺序**（两种 PP 一致）：
```
1. SetPresentPhases(phaseLabels, phaseStatus)
2. SetSinglePhaseProp("fraction",      phase, "mole",   x)
3. SetSinglePhaseProp("phaseFraction", phase, "mole",   vf)
4. SetSinglePhaseProp("temperature",   phase, nullptr,  T)
5. SetSinglePhaseProp("pressure",      phase, nullptr,  P)
6. SetOverallProp("temperature",       nullptr, T)
7. SetOverallProp("pressure",          nullptr, P)
```

### 5.7 多组分 GetCompoundConstant 模式

单组分 PP 用 if-else 逐属性返回即可，多组分则需要循环 + 映射表：

```cpp
void GetCompoundConstant(CapeArrayString props, CapeArrayString compIds,
    CapeBoolean& missing, CapeArrayValue vals)
{
    missing = false;
    for (size_t j = 0; j < compIds.size(); ++j) {
        std::wstring cid = static_cast<std::wstring>(compIds[j]);
        int ci = findCompound(cid);  // 在 compoundDB 中查找
        if (ci < 0) { missing = true; continue; }
        const CompoundData& comp = compoundDB[ci];
        for (size_t i = 0; i < props.size(); ++i) {
            std::wstring pn = static_cast<std::wstring>(props[i]);
            size_t idx = i * compIds.size() + j;  // CAPE-OPEN 行优先排列
            if (pn == L"molecularWeight")
                vals[idx].Set(CapeDouble(comp.mw));
            else if (pn == L"boilingTemperature")
                vals[idx].Set(CapeDouble(comp.boilT));
            // ... 其他属性
            else missing = true;
        }
    }
}
```

> ⚠️ 多组分属性值的索引是 `i * compIds.size() + j`（行优先），不是 `j * props.size() + i`。

---

## 六、修复历史与分析

从初始版本到稳定经历的关键 Bug 及其通用教训。适用于 WaterPP 和 IdealGasPP 两类。

### 通用教训速查

| 版本/来源 | 关键修复 | 通用教训 |
|:--:|------|------|
| WaterPP v1.0.1 | CheckCompounds 空实现→完整校验 | **初始化方法不能留空** |
| WaterPP v1.0.2 | 移除所有 try-catch 静默吞没 | **永远不要让异常静默** |
| WaterPP v1.0.3 | getPDependentPropList 显式 setsize(0) | **空数组也要初始化（resize(0)）** |
| WaterPP v1.0.4 | CheckEquilibriumSpec 改为 `return true` | **理解 Adapter 调用链** |
| WaterPP v1.0.4 | H/S 读取改为 mass basis | **输入输出单位要一致** |
| WaterPP v1.0.4 | spec 解析加 towlower | **大小写兼容** |
| WaterPP v1.0.5 | 比较常量同步改小写 | **towlower 是全有或全无** |
| WaterPP v1.0.6 | `* MOLWT * 1e3` → `* MOLWT` | **推导公式，不要猜** |
| IdealGasPP v1.0 | T/P basis `""` → `nullptr` | **"Invalid base specified" = basis 错了** |
| IdealGasPP v1.0 | CalcEquilibrium 只支持 TP/PH | **9 种闪蒸类型都应支持，否则 PME 会反复 fallback** |
| IdealGasPP v1.0 | fraction sum≈1 未校验 | **多组分必须校验 fraction 的 count 和 sum** |
| IdealGasPP v1.0 | 缺少 CapeUtilitiesAdapter | **8 个 Adapter 缺一不可** |
| IdealGasPP v1.0 | GetOverallTPFraction 用于 VF flash | **TP flash 和 VF flash 应走不同读路径** |

---

## 七、参考资源

### 本项目源码

| 资源 | 路径 | 类型 |
|------|------|------|
| WaterPP 完整实现 | `ChemProp\WaterPP\WaterPP.cpp` + `WaterPP.h` | 单组分 2 相示例 |
| Water 物性引擎 | `ChemProp\WaterPP\Water.cpp` + `Water.h` | IAPWS-97 纯数学 |
| IdealGasPP 完整实现 | `ChemProp\IdealGasPP\IdealGasPP.cpp` + `IdealGasPP.h` | 多组分 1 相示例 |
| CMake 构建脚本 | `ChemProp\WaterPP\CMakeLists.txt` / `ChemProp\IdealGasPP\CMakeLists.txt` | 构建模板 |

### 外部资源

| 资源 | 路径/链接 |
|------|---------|
| COBIA SDK 头文件 | `C:\Program Files (x86)\Common Files\CAPE-OPEN Laboratories Network\Include\` |
| cobiaRegister 注册工具 | `C:\Program Files\Common Files\CAPE-OPEN Laboratories Network\COBIA\` |
| CapeWizard 代码生成器 | 同上目录 |
| COFE (PME 测试) | https://www.amsterchem.com/cofe.html |
| CAPE-OPEN 标准 | https://www.colan.org/ |
| IAPWS-97 | http://www.iapws.org/ |
| MinGW-w64 (MSYS2) | https://www.msys2.org/ |