# COBIA Property Package Development Guide

> Using the project's `WaterPP` (IAPWS-97 pure water property package) and `IdealGasPP` (multi-component ideal gas property package) as dual reference examples. After reading this guide, you should be able to independently write, debug, and register a COBIA Property Package that can be loaded by COFE — whether it's single-component or multi-component.

---

## 1. Background: What is COBIA

### 1.1 CAPE-OPEN Overview

**CAPE-OPEN** (Computer-Aided Process Engineering Open Interface) is a set of interoperability standards for process simulation software. It defines a series of COM interfaces that allow Property Packages, Unit Operations, and Solvers from different vendors to work together within the same Process Modeling Environment (PME, such as COFE, Aspen Plus).

Core roles:

```
┌──────────────┐     ┌─────────────────────┐     ┌──────────────────┐
│   PME (COFE) │────▶│ PropertyPackage (PP) │────▶│ Material Object  │
│   Process     │     │ Property Package      │     │                  │
│   Modeling    │     │ (hero of this guide)  │     │                  │
│   Environment │     └─────────────────────┘     └──────────────────┘
└──────────────┘                │
       │                        ▼
       ▼              ┌─────────────────────┐
┌──────────────┐      │ Property Engine      │
│  Unit Op     │      │ (IAPWS-97)           │
│              │      │ Pure math, no         │
│              │      │ interface dependency  │
└──────────────┘      └─────────────────────┘
```

The PME queries the PP for properties through CAPE-OPEN standard interfaces. The PP reads T/P/composition through Material objects and returns calculation results.

### 1.2 COBIA vs Traditional COM

| Dimension | Traditional COM (e.g., original Water package) | COBIA SDK (WaterPP / IdealGasPP) |
|-----------|-----------------------------------------------|----------------------------------|
| **Build toolchain** | Visual C++ / MSVC only | MinGW-w64 / CMake (cross-platform) |
| **COM implementation** | ATL + IDispatch + VARIANT (hand-written) | `CapeInterfaceAdapters` (template auto-generated) |
| **Registration** | `regsvr32 / DllRegisterServer` | `cobiaRegister.exe -a` |
| **Strings** | `BSTR` / `CBStr` | `CapeStringImpl` / `ICapeString` |
| **Error handling** | `HRESULT` + `BEGIN_TRY`/`END_TRY` macros | C++ exceptions `throw cape_open_error()` |
| **Code volume** | ~8,000 lines (lots of COM boilerplate) | WaterPP ~1,200 lines / IdealGasPP ~750 lines |

> The core value of COBIA: **auto-generates COM boilerplate code**. You only need to focus on property calculation logic — no hand-writing `IDispatch::Invoke`, `VARIANT` conversions, `DllRegisterServer`, etc.

### 1.3 Single-Component vs Multi-Component

This guide covers the two most common property package types:

| Dimension | Single-Component (WaterPP) | Multi-Component (IdealGasPP) |
|-----------|---------------------------|------------------------------|
| **Compound count** | 1 (Water) | N (N₂, O₂, CO₂...) |
| **Phase count** | 2 (Vapor + Liquid) | 1 (Vapor only) |
| **Property engine** | IAPWS-97 equations | Cp polynomial + ideal gas law |
| **Compound management** | Not needed | `std::vector<CompoundData>` |
| **Flash complexity** | Must compute phase equilibrium (Psat/Tsat) | Vapor only, no phase change |
| **Validation focus** | phaseFraction range | Compound fraction sum≈1 |

### 1.4 The 8 Required Adapters

A COBIA property package must inherit from the following 8 Adapters (CRTP pattern):

| # | Adapter | Corresponding CAPE-OPEN Interface | Purpose |
|---|---------|----------------------------------|---------|
| 1 | `CapeThermoPropertyPackageManagerAdapter` | ICapeThermoPropertyPackageManager | PME obtains PP instances through this |
| 2 | `CapeThermoMaterialContextAdapter` | ICapeThermoMaterialContext | Receives/releases Material objects |
| 3 | `CapeThermoCompoundsAdapter` | ICapeThermoCompounds | Compound list, constants, T/P-dependent properties |
| 4 | `CapeThermoPhasesAdapter` | ICapeThermoPhases | Phase definitions (Vapor/Liquid) |
| 5 | `CapeThermoPropertyRoutineAdapter` | ICapeThermoPropertyRoutine | Single-phase/two-phase property calculation |
| 6 | `CapeThermoEquilibriumRoutineAdapter` | ICapeThermoEquilibriumRoutine | Phase equilibrium/flash calculation |
| 7 | `CapeThermoUniversalConstantAdapter` | ICapeThermoUniversalConstant | Universal constants (R, Avogadro...) |
| 8 | `CapeUtilitiesAdapter` | ICapeUtilities | Initialize/Terminate/Edit |

Missing any one of these will cause a compilation failure. The COBIA SDK's built-in `CapeWizard.exe` can generate template code containing all 8 Adapters.

---

## 2. Creating a COBIA Property Package from Scratch

### 2.1 Environment Setup

| Component | Purpose | Path/Source |
|-----------|---------|-------------|
| **COBIA SDK** | Headers + registration tool | `C:\Program Files (x86)\Common Files\CAPE-OPEN Laboratories Network\Include\` |
| **cobiaRegister.exe** | DLL registration/unregistration | `C:\Program Files\Common Files\CAPE-OPEN Laboratories Network\COBIA\` |
| **MSYS2 MinGW-w64** | GCC compiler | `D:\msys64\mingw64\bin\` |
| **CMake** | Build system | 3.16+ |
| **COFE** | PME for testing | https://www.amsterchem.com/cofe.html |

Verify environment:

```powershell
Test-Path "C:\Program Files (x86)\Common Files\CAPE-OPEN Laboratories Network\Include\COBIA.h"
& "D:\msys64\mingw64\bin\g++.exe" --version
Test-Path "C:\Program Files\Common Files\CAPE-OPEN Laboratories Network\COBIA\cobiaRegister.exe"
```

### 2.2 Project File Structure

```
MyPP/
├── CMakeLists.txt        # Build configuration
├── MyPP.h                # Class declaration + 8 Adapter inheritance
├── MyPP.cpp              # All interface implementations
└── Engine.h/cpp          # Property engine (pure math, no COBIA dependency)
```

**Design principle**: The property engine is **completely decoupled** from CAPE-OPEN interfaces. The Engine only handles mathematical calculations and depends on no COM/COBIA headers. This allows you to compile the Engine separately as a command-line tool for unit testing.

### 2.3 Step 1: CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.16)
project(MyPP VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(COBIA_INC "C:/Program Files (x86)/Common Files/CAPE-OPEN Laboratories Network/Include")

add_library(MyPP SHARED MyPP.cpp Engine.cpp)
target_include_directories(MyPP PRIVATE ${COBIA_INC} .)
target_compile_definitions(MyPP PRIVATE COBIA_NOAUTOLINK)

# MinGW static linking
if(MINGW)
    target_link_options(MyPP PRIVATE
        -static-libgcc -static-libstdc++
        -Wl,-Bstatic,--whole-archive -lwinpthread
        -Wl,--no-whole-archive,-Bdynamic)
endif()
target_link_libraries(MyPP PRIVATE advapi32 shell32 user32 ole32 oleaut32 uuid)
set_target_properties(MyPP PROPERTIES PREFIX "" SUFFIX ".dll")
```

### 2.4 Step 2: Header File (MyPP.h) — Minimal Template

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

> ⚠️ The GUID must be globally unique. You can use a fixed value during development, but **must generate a new one before release**. Duplicate GUIDs will cause COBIA registration conflicts.

### 2.5 Step 3: Minimal Implementation (MyPP.cpp) — Single-Phase Ideal Gas

Below is a minimal implementation supporting only **single-phase ideal gas**. Once loaded in COFE, it can compute density and enthalpy:

```cpp
#include "MyPP.h"
#include "Engine.h"   // Your property engine
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
    props.setsize(0);  // must explicitly set empty
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

// ========== 5. PropertyRoutine — Core property calculation ==========
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

    // 1. Read T/P from phase
    COBIA::CapeStringImpl tempStr(COBIATEXT("temperature"));
    COBIA::CapeStringImpl presStr(COBIATEXT("pressure"));
    COBIA::CapeArrayRealImpl t(1);
    COBIA::CapeArrayRealImpl p(1);

    material.GetSinglePhaseProp(
        static_cast<ICapeString*>(&tempStr), phaseLabel,
        static_cast<ICapeString*>(nullptr),  // T/P basis must be nullptr
        static_cast<ICapeArrayReal*>(&t));
    material.GetSinglePhaseProp(
        static_cast<ICapeString*>(&presStr), phaseLabel,
        static_cast<ICapeString*>(nullptr),
        static_cast<ICapeArrayReal*>(&p));

    double T = t[0], P = p[0];

    // 2. Validate
    if (!std::isfinite(T) || T <= 0.0)
        throw COBIA::cape_open_error(COBIAERR_InvalidOperation);
    if (!std::isfinite(P) || P <= 0.0)
        throw COBIA::cape_open_error(COBIAERR_InvalidOperation);

    // 3. Call property engine
    double density = P * 28.0e-3 / (8.314462618 * T);  // PM/RT
    double enthalpy = 1.005 * 28.0 * T;                 // Cp*M*T (J/mol)

    // 4. Write back to Material
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

### 2.6 Step 4: Build, Register, Test

```powershell
# Build
$env:PATH = "D:\msys64\mingw64\bin;" + $env:PATH
cd build/mingw-release
cmake --build . --target MyPP

# Register
& "C:\Program Files\Common Files\CAPE-OPEN Laboratories Network\COBIA\cobiaRegister.exe" ^
    -a "build/mingw-release/MyPP.dll"

# Unregister (must be done before updating the DLL)
& "C:\Program Files\Common Files\CAPE-OPEN Laboratories Network\COBIA\cobiaRegister.exe" ^
    -u "build/mingw-release/MyPP.dll"
```

Test workflow: **Completely close COFE → Build → Register → Open COFE → Configure → Property Packages → Add MyPP**

> ⚠️ **COM caching is the most common rookie trap**: After modifying the DLL, you must completely close COFE and reopen it, otherwise the in-process COM cache will still load the old DLL.

### 2.7 Minimal Example Checklist

Fill out the checklist below to confirm your implementation is complete:

| # | Check Item | ✓ |
|---|------------|---|
| 1 | All 8 Adapters declared | ☐ |
| 2 | `getPropertyPackageList` returns package name | ☐ |
| 3 | `GetPropertyPackage` creates and returns PP instance | ☐ |
| 4 | `getNumCompounds` returns ≥1 | ☐ |
| 5 | `GetCompoundList` fills at least one compound | ☐ |
| 6 | `getConstPropList` returns at least `molecularWeight` | ☐ |
| 7 | `GetCompoundConstant` handles `molecularWeight` | ☐ |
| 8 | `getPDependentPropList` explicitly `setsize(0)` | ☐ |
| 9 | `getNumPhases` returns ≥1 | ☐ |
| 10 | `GetPhaseList` fills at least one phase | ☐ |
| 11 | `getSinglePhasePropList` returns at least `"enthalpy"` | ☐ |
| 12 | `CalcSinglePhaseProp` can read T/P with `nullptr` and write results | ☐ |
| 13 | `CheckEquilibriumSpec` returns `true` | ☐ |
| 14 | `Register` fills package name + `addCatID` | ☐ |
| 15 | Can load in COFE, can see compounds and properties | ☐ |

Once all 15 items pass, your COBIA property package has a solid foundation. You can then gradually add more features.

---

## 3. Interface Method Details (Dual Reference: WaterPP / IdealGasPP)

Each section follows a three-tier progression: "shortest usable code → WaterPP full implementation → common mistakes".

### 3.1 Compound Database (ICapeThermoCompounds)

**Minimum requirement**: Register at least one compound, provide `molecularWeight`.

**WaterPP implementation**: One compound Water, 12 constants, 10 T-dependent properties.

```cpp
// getConstPropList — declare constant list
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

// GetCompoundConstant — return per property
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
            auto it = constValues.find(pn);               // map lookup
            if (it != constValues.end())
                vals[i].Set(CapeDouble(it->second));
            else missing = true;
        }
    }
}
```

> 💡 Use `std::map<std::wstring, double>` to store secondary constants, avoiding long if-else chains.

**T-dependent property list** (`getTDependentPropList`):

| Property ID | Description |
|-------------|-------------|
| `vaporPressure` | Saturation vapor pressure Pa |
| `surfaceTensionSatLiquid` | Surface tension N/m |
| `thermalConductivityLiquid` / `Vapor` | Saturation line thermal conductivity W/(m·K) |
| `viscosityLiquid` / `Vapor` | Saturation line viscosity Pa·s |
| `volumeLiquid` / `volumeChangeVaporization` | Saturation line specific volume / vaporization volume difference m³/mol |
| `idealGasEnthalpy` / `idealGasEntropy` | Ideal gas enthalpy/entropy J/mol |

### 3.2 Single-Phase Property Calculation (CalcSinglePhaseProp) ⭐ Most Common

The PME calls this method in every iteration to compute density, enthalpy, entropy, and other single-phase properties.

**Standard workflow** (5 steps):

```
1. GetSinglePhaseProp("temperature", phaseLabel, nullptr) → T
2. GetSinglePhaseProp("pressure",    phaseLabel, nullptr) → P
3. isfinite(T) && T>0,  isfinite(P) && P>0 validate → throw on failure
4. Engine.compute(T, P) → property values
5. SetSinglePhaseProp(propName, phaseLabel, basis, value)
```

**Unit conversion — WaterPP lessons**:

The Water engine's internal unit is **kJ/kg**, while COBIA requires **mole basis (J/mol)**.

```
w.enthalpy()  = kJ/kg

J/mol = kJ/kg × 1000(J/kJ) × (MOLWT_g/1000)(kg/mol)
      = kJ/kg × MOLWT                    ← ×1000 and ÷1000 cancel!
```

Therefore the correct conversion is `w.enthalpy() * Water::MOLWT` (**does not need** `* 1e3`).

> ⚠️ WaterPP previously had the bug `w.enthalpy() * MOLWT * 1e3`, causing all thermo properties to be inflated 1000×.

**Basis parameter quick reference**:

| Property type | basis parameter | Example |
|---------------|:---------------:|---------|
| temperature | `nullptr` | `GetSinglePhaseProp("temperature", phase, nullptr, &t)` |
| pressure | `nullptr` | `GetSinglePhaseProp("pressure", phase, nullptr, &p)` |
| enthalpy/entropy/Cp/Cv... | `"mole"` | `SetSinglePhaseProp("enthalpy", phase, "mole", &v)` |
| thermalConductivity | `nullptr` | `SetSinglePhaseProp("thermalConductivity", phase, nullptr, &v)` |
| viscosity | `nullptr` | `SetSinglePhaseProp("viscosity", phase, nullptr, &v)` |
| fraction | `"mole"` | `GetSinglePhaseProp("fraction", phase, "mole", &v)` |

> ❌ **Do not** pass empty string `""` as basis — this causes "Invalid base specified".
> ✅ **Always** pass `nullptr` for T/P — this is the CAPE-OPEN standard for dimensionless properties.

**WaterPP's 11 supported single-phase properties**:

| Property | Water engine input | COBIA output | Conversion formula |
|----------|:------------------:|--------------|--------------------|
| density | m³/kg | mol/m³ | `1 / (v × M × 1e-3)` |
| enthalpy | kJ/kg | J/mol | `h × M` |
| entropy | kJ/(kg·K) | J/(mol·K) | `s × M` |
| gibbsEnergy | kJ/kg | J/mol | `g × M` |
| heatCapacityCp | kJ/(kg·K) | J/(mol·K) | `cp × M` |
| heatCapacityCv | kJ/(kg·K) | J/(mol·K) | `cv × M` |
| internalEnergy | kJ/kg | J/mol | `u × M` |
| molecularWeight | — | g/mol | direct `Water::MOLWT` |
| thermalConductivity | W/(m·K) | W/(m·K) | direct use |
| volume | m³/kg | m³/mol | `v × M × 1e-3` |
| viscosity | Pa·s | Pa·s | direct use |

**Multi-component extension** (IdealGasPP): compute mixture properties per component:

```cpp
// Read component fractions
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

> 💡 Ideal gas mixture property = Σ xᵢ × pure component property. Density goes through molar mass mixing, not summation.

### 3.3 Phase Equilibrium / Flash (CalcEquilibrium) — Most Complex

The internal call sequence of COBIA `CapeThermoEquilibriumRoutineAdapter`:

```
PME calls CalcEquilibrium(spec1, spec2, solutionType)
  └── Inside COBIA Adapter:
       ├─ ① CheckEquilibriumSpec(spec1, spec2, solutionType)
       │    └─ false → throw "CalcEquilibrium failed: Invalid operation"
       │               (CalcEquilibrium is never called!)
       └─ ② CalcEquilibrium(spec1, spec2, solutionType)
            (only executes if ① returns true)
```

> ⚠️ **Fatal trap**: If `CheckEquilibriumSpec` returns `false`, `CalcEquilibrium` will never execute. But the exception message prefix is "CalcEquilibrium failed", which is highly misleading. WaterPP was fixed by changing to `return true`.

**Spec parsing** (5 properties × 9 combinations):

```
specification[i] = [propertyName, basis, phase, compoundId]
                      [0]         [1]    [2]       [3]
```

| propertyName | Meaning | phase | Supported combinations |
|:------------:|---------|:-----:|------------------------|
| `temperature` | T | `overall` | TP, TVF, TH, TS |
| `pressure` | P | `overall` | TP, PVF, PH, PS |
| `phasefraction` | VF | `vapor`/`liquid` | TVF, PVF, HVF, SVF |
| `vaporfraction` | VF alias | `vapor`/`liquid` | same as above |
| `enthalpy` | H | `overall` | PH, TH, HVF |
| `entropy` | S | `overall` | PS, TS, SVF |

9 flash spec combinations:

| Flash | spec1 | spec2 |
|:-----:|-------|-------|
| **TP** | temperature | pressure |
| **TVF** | temperature | phasefraction |
| **PVF** | pressure | phasefraction |
| **PH** | pressure | enthalpy |
| **TH** | temperature | enthalpy |
| **PS** | pressure | entropy |
| **TS** | temperature | entropy |
| **HVF** | enthalpy | phasefraction |
| **SVF** | entropy | phasefraction |

**Safe spec parsing** (case-insensitive + error labels):

```cpp
void WaterPP::CalcEquilibrium(
    COBIA::CapeArrayString specification1,
    COBIA::CapeArrayString specification2,
    COBIA::CapeString solutionType)
{
    if (!hasMaterial) return;

    // 1. Convert to lowercase (case-insensitive)
    std::wstring spec1 = static_cast<std::wstring>(specification1[0]);
    std::wstring spec2 = static_cast<std::wstring>(specification2[0]);
    for (auto& c : spec1) c = towlower(c);
    for (auto& c : spec2) c = towlower(c);

    // 2. Parse specs, set flags
    bool haveT = false, haveP = false, haveVF = false, haveH = false, haveS = false;
    for (int i = 0; i < 2; ++i) {
        std::wstring s     = (i == 0) ? spec1 : spec2;
        auto& specArr      = (i == 0) ? specification1 : specification2;
        std::wstring phase = static_cast<std::wstring>(specArr[2]);
        std::wstring basis = static_cast<std::wstring>(specArr[1]);
        for (auto& c : phase) c = towlower(c);  // ← case-insensitive
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
            // ↑ Note: comparison strings must match towlower result (all lowercase)
            if (phase != L"vapor" && phase != L"liquid")
                throw cape_open_error("EQ-VF-PHASE");
            haveVF = true;
        }
        else if (s == L"enthalpy")  { haveH = true; }
        else if (s == L"entropy")   { haveS = true; }
        else {
            std::wstring msg = L"EQ-UNKNOWN: " + s;  // ← diagnostic label
            throw cape_open_error(msg.c_str());
        }
    }

    // 3. Dispatch to corresponding flash logic based on flag combinations
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

### 3.4 Universal Constants (ICapeThermoUniversalConstant)

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

## 4. Debugging & Troubleshooting — Real-World Experience

### 4.1 Error Label System (Most Important Debugging Tip)

Exception messages thrown by COBIA appear in the COFE log. But if all 20 of your `throw` statements use the same `COBIAERR_InvalidOperation`, you won't be able to tell which location errored.

**Correct approach**: Attach a unique label to every `throw`:

```cpp
// ❌ No label — can't locate
throw cape_open_error(COBIAERR_InvalidOperation);

// ✅ With label — log directly tells you where the problem is
throw cape_open_error(COBIATEXT("EQ-T: invalid temperature spec"));
throw cape_open_error(COBIATEXT("EQ-FLASH-T: invalid T for TP flash"));
throw cape_open_error(COBIATEXT("EQ-UNKNOWN: ") + propertyName);
```

Label naming convention: `{Module}-{SubModule}: {Description}`

| Label | Meaning |
|-------|---------|
| `EQ-T` | CalcEquilibrium — T spec parse failed |
| `EQ-READ-T` | CalcEquilibrium — T value read invalid |
| `EQ-FLASH-T` | CalcEquilibrium — TP flash T invalid |
| `EQ-VF-PHASE` | CalcEquilibrium — phaseFraction phase wrong |
| `EQ-UNKNOWN: xxx` | CalcEquilibrium — unknown property name (log shows the name) |
| `EQ-SOL` | CalcEquilibrium — solutionType not supported |
| `EQ-COMPID` | CalcEquilibrium — compoundId non-empty (pure component rejects this) |

### 4.2 COFE Log Interpretation

Error message format in COFE log:

```
warning: Material object error in CalcEquilibrium:
CalcEquilibrium failed: in ICapeThermoEquilibriumRoutine::CalcEquilibrium
of WaterPPPropertyPackage: EQ-UNKNOWN: phasefraction
```

How to interpret:

| Log fragment | What it tells you |
|--------------|-------------------|
| `in ICapeThermoEquilibriumRoutine::CalcEquilibrium` | The interface and method that errored |
| `of WaterPPPropertyPackage` | The PP class that errored |
| `EQ-UNKNOWN: phasefraction` | Your diagnostic label: spec property name "phasefraction" was not recognized |
| `(4x)` | Same error appeared 4 consecutive times |

> "phasefraction" not recognized → entered else branch throwing `EQ-UNKNOWN` → check spec parsing code, found that the comparison string `L"phaseFraction"` (capital F) didn't match the towlower'd "phasefraction" (lowercase f).

### 4.3 Unit Verification

After COFE outputs a property table, verify units with a quick hand calculation:

Using water at 99°C / 0.1 MPa liquid as an example:

| Property | Approximate theoretical value | If showing 1000× | Issue |
|----------|:----------------------------:|:----------------:|-------|
| enthalpy (J/mol) | ~7,550 | ~7,550,000 | `* MOLWT * 1e3` multiplied by extra 1000 |
| Cp (J/(mol·°C)) | ~76 | ~76,000 | same as above |
| density (mol/m³) | ~53,200 | — | this one is generally correct |
| viscosity (Pa·s) | ~2.8×10⁻⁴ | — | this one is generally correct |

**Quick mental verification**: Water at 100°C liquid mass enthalpy ≈ 419 kJ/kg, molar mass ≈ 0.018 kg/mol:
```
J/mol = 419 × 1000 × 0.018 = 7,542 J/mol
```
If your output is 7,542,000, you know you multiplied by an extra 1000.

### 4.4 Common Error Quick Reference

| Symptom | Likely Cause | Check |
|---------|--------------|-------|
| PP not visible in COFE | Register didn't call addCatID | Verify `registrar.addCatID(CAPEOPEN::categoryId_PropertyPackageManager)` |
| After loading, T becomes 100°C, P becomes 1atm | try-catch silently swallowing exceptions using defaults | Remove all `catch(...){}` |
| "Invalid base specified" | T/P basis passed as empty string | Change to `nullptr` |
| "invalid basis for fraction" | fraction basis passed as empty string | Change to `"mole"` |
| "Invalid operation" (no label) | CheckEquilibriumSpec returned false | Change to `return true` |
| EQ-UNKNOWN (4x) | spec property name case mismatch | Apply `towlower` to both spec strings and comparison constants |
| All thermo values 1000× too large | Bug `* MOLWT * 1e3` instead of correct `* MOLWT` | See §3.2 unit derivation |

### 4.5 Spec Parsing Case-Sensitivity Trap

Different PMEs may send strings with different casing:

| PME may send | Your code has | Consequence |
|:------------:|:------------:|-------------|
| `"overall"` | `L"Overall"` | phase match fails |
| `"temperature"` | `L"Temperature"` | property name match fails |
| `"Unspecified"` | `L"unspecified"` | solutionType match fails |
| `"Normal"` | `L"normal"` | solutionType match fails |

**The only solution**: Apply `towlower` to **all** strings involved in comparison, and use **all lowercase** for comparison constants.

```cpp
// ✅ All-lowercase approach
std::wstring s = static_cast<std::wstring>(specArr[0]);
for (auto& c : s) c = towlower(c);       // input → lowercase
// ...
if (s == L"temperature") { ... }          // constants also lowercase
if (s == L"phasefraction") { ... }        // constants also lowercase (not phaseFraction)
if (s == L"vaporfraction") { ... }        // constants also lowercase
```

### 4.6 Development Workflow

```
┌──────────────────────────────────────────────────┐
│  1. Close COFE (COM cache must be released)        │
│  2. Build DLL                                       │
│  3. Unregister old DLL (cobiaRegister -u)           │
│  4. Register new DLL (cobiaRegister -a)             │
│  5. Open COFE → Add PP → New Stream → Test         │
│  6. View COFE log (View → Log)                     │
│  7. Locate issue from diagnostic labels in log      │
│  8. Return to step 1                               │
└──────────────────────────────────────────────────┘
```

> ⚠️ If you only build without re-registering, COFE still loads the old DLL. Unregister then re-register is the safest approach.

---

## 5. Universal Implementation Patterns

The following patterns have been verified effective in both WaterPP and IdealGasPP, and apply to any COBIA property package.

### 5.1 Adapter Inheritance Panorama

Regardless of single-component or multi-component, you must inherit the same set of 8 Adapters:

```cpp
// Universal template — replace WaterPPPropertyPackage with your class name
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

All are CRTP pattern (`Adapter<T>`). The Adapter calls your overridden methods at compile time via `static_cast<T*>`. No need to manually implement `QueryInterface`, `AddRef`, `Release`.

### 5.2 Compound Database Pattern (Multi-Component Specific)

IdealGasPP uses `std::vector<CompoundData>` to store compound parameters, suitable for arbitrary combinations of N components:

```cpp
struct CompoundData {
    std::wstring id;       // "Nitrogen"
    std::wstring formula;  // "N2"
    double mw;             // 28.0134 g/mol
    double cpA, cpB, cpC, cpD, cpE;  // Cp = A + B*T + C*T² + ...
    double H0;             // Standard enthalpy J/mol @ Tref
    double S0;             // Standard entropy J/(mol·K) @ Tref,Pref
};
std::vector<CompoundData> compoundDB;  // Compound database
```

**Compound validation function** — Verify that the Material's fractions match the database before every property calculation:

```cpp
void CheckCompounds() {
    if (compoundsChecked) return;
    // Read fractions from Material, verify count match + sum≈1
    ...
    compoundsChecked = true;
}
```

> 💡 `CheckCompounds` only executes on first call; subsequent calls skip via `compoundsChecked=true`. Reset on `UnsetMaterial`.

### 5.3 Internal Enums — Avoid String Comparisons

```cpp
enum SinglePhaseProp {
    SPP_DENSITY, SPP_ENTHALPY, SPP_ENTROPY,
    SPP_GIBBS_ENERGY, SPP_HEAT_CAPACITY_CP, SPP_HEAT_CAPACITY_CV,
    SPP_INTERNAL_ENERGY, SPP_MOLECULAR_WEIGHT,
    SPP_THERMAL_CONDUCTIVITY, SPP_VOLUME, SPP_VISCOSITY
};

// String → enum mapping
bool getSinglePhasePropEnum(const std::wstring& name, SinglePhaseProp& result) {
    if (name == L"density")           { result = SPP_DENSITY; return true; }
    if (name == L"enthalpy")          { result = SPP_ENTHALPY; return true; }
    // ...
    return false;
}
```

> 💡 Only do string→enum conversion once at entry; subsequent switch-case compares integers directly, much faster than repeated string comparisons.

### 5.4 Complete Unit Chain Diagram

```
             ┌─────────────────────────────────────┐
             │          COFE / PME                  │
             │   COBIA interface, mole basis (J/mol)│
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
    │        Water Engine (IAPWS-97)                │
    │        Internal units: kJ/kg, MPa, K          │
    │        w.enthalpy() → kJ/kg                   │
    │        w.SetStatePH(P_MPa, kJ/kg)             │
    └───────────────────────────────────────────────┘
```

**Key conversion points**:
- **Input read**: Mass basis read `GetOverallProp("enthalpy", "mass")` → J/kg → ×1e-3 → kJ/kg
- **Internal bridge**: IAPWS internal is kJ/kg
- **Output**: `w.enthalpy()` → kJ/kg → ×MOLWT → J/mol (mole basis)

### 5.5 Correct Strategy for CheckEquilibriumSpec

```cpp
COBIA::CapeBoolean CheckEquilibriumSpec(
    COBIA::CapeArrayString, COBIA::CapeArrayString, COBIA::CapeString)
{
    return true;  // Don't block; put validation logic inside CalcEquilibrium
}
```

Reason: The COBIA Adapter calls `CheckEquilibriumSpec` before `CalcEquilibrium`. If this returns `false`, CalcEquilibrium never executes. On first call, PresentPhases may not be set yet, so phase checks here would produce false negatives.

### 5.6 Flash Dispatch Pattern

Regardless of single-component or multi-component, `CalcEquilibrium` follows the same template:

```
1. Parse spec1/spec2 → set haveT/haveP/haveVF/haveH/haveS flags
2. Validate solutionType
3. Read T/P/fraction by haveX combination (TP uses GetOverallTPFraction, others use GetOverallProp)
4. Dispatch flash type → compute resultT / resultP / resultVapFrac
5. SetPresentPhases → SetSinglePhaseProp(T/P/fraction/phaseFraction) → SetOverallProp(T/P)
```

**TP flash vs VF-based flash branch logic**:

```cpp
// Single-component 2-phase (WaterPP): must compute Psat/Tsat to determine phase boundary
if (haveT && haveP) {      /* TP: compute phase equilibrium, get vapFrac */ }
else if (haveT && haveVF) { /* TVF: T known, P=Psat(T) */ }
else if (haveP && haveVF) { /* PVF: P known, T=Tsat(P) */ }

// Multi-component 1-phase (IdealGasPP): no phase equilibrium, directly set all-vapor
if (haveT && haveP) { /* TP: use T/P directly */ }
if (haveH && !haveT) { /* PH: T += (Htarget-Href)/Cp */ }
// VF-based flash same as single-phase, read VF then write directly
```

**Standard order for writing to Material** (consistent across both PP types):
```
1. SetPresentPhases(phaseLabels, phaseStatus)
2. SetSinglePhaseProp("fraction",      phase, "mole",   x)
3. SetSinglePhaseProp("phaseFraction", phase, "mole",   vf)
4. SetSinglePhaseProp("temperature",   phase, nullptr,  T)
5. SetSinglePhaseProp("pressure",      phase, nullptr,  P)
6. SetOverallProp("temperature",       nullptr, T)
7. SetOverallProp("pressure",          nullptr, P)
```

### 5.7 Multi-Component GetCompoundConstant Pattern

Single-component PP can use if-else per property. Multi-component requires loops + lookup table:

```cpp
void GetCompoundConstant(CapeArrayString props, CapeArrayString compIds,
    CapeBoolean& missing, CapeArrayValue vals)
{
    missing = false;
    for (size_t j = 0; j < compIds.size(); ++j) {
        std::wstring cid = static_cast<std::wstring>(compIds[j]);
        int ci = findCompound(cid);  // find in compoundDB
        if (ci < 0) { missing = true; continue; }
        const CompoundData& comp = compoundDB[ci];
        for (size_t i = 0; i < props.size(); ++i) {
            std::wstring pn = static_cast<std::wstring>(props[i]);
            size_t idx = i * compIds.size() + j;  // CAPE-OPEN row-major layout
            if (pn == L"molecularWeight")
                vals[idx].Set(CapeDouble(comp.mw));
            else if (pn == L"boilingTemperature")
                vals[idx].Set(CapeDouble(comp.boilT));
            // ... other properties
            else missing = true;
        }
    }
}
```

> ⚠️ The index for multi-component property values is `i * compIds.size() + j` (row-major), **not** `j * props.size() + i`.

---

## 6. Fix History & Analysis

Key bugs and their universal lessons from initial version to stable. Applicable to both WaterPP and IdealGasPP.

### Universal Lessons Quick Reference

| Version/Source | Key Fix | Universal Lesson |
|:--------------:|---------|------------------|
| WaterPP v1.0.1 | CheckCompounds empty impl→full validation | **Don't leave init methods empty** |
| WaterPP v1.0.2 | Removed all try-catch silent swallowing | **Never let exceptions go silent** |
| WaterPP v1.0.3 | getPDependentPropList explicit setsize(0) | **Empty arrays must also be initialized (resize(0))** |
| WaterPP v1.0.4 | CheckEquilibriumSpec changed to `return true` | **Understand the Adapter call chain** |
| WaterPP v1.0.4 | H/S reads changed to mass basis | **Input/output units must be consistent** |
| WaterPP v1.0.4 | Spec parsing added towlower | **Case-insensitive handling** |
| WaterPP v1.0.5 | Comparison constants synced to lowercase | **towlower is all-or-nothing** |
| WaterPP v1.0.6 | `* MOLWT * 1e3` → `* MOLWT` | **Derive formulas, don't guess** |
| IdealGasPP v1.0 | T/P basis `""` → `nullptr` | **"Invalid base specified" = wrong basis** |
| IdealGasPP v1.0 | CalcEquilibrium only supports TP/PH | **All 9 flash types should be supported, otherwise PME repeatedly falls back** |
| IdealGasPP v1.0 | fraction sum≈1 not validated | **Multi-component must validate fraction count and sum** |
| IdealGasPP v1.0 | Missing CapeUtilitiesAdapter | **All 8 Adapters are mandatory — not one less** |
| IdealGasPP v1.0 | GetOverallTPFraction used for VF flash | **TP flash and VF flash should take different read paths** |

---

## 7. Reference Resources

### Project Source Code

| Resource | Path | Type |
|----------|------|------|
| WaterPP complete implementation | `ChemProp\WaterPP\WaterPP.cpp` + `WaterPP.h` | Single-component 2-phase example |
| Water property engine | `ChemProp\WaterPP\Water.cpp` + `Water.h` | IAPWS-97 pure math |
| IdealGasPP complete implementation | `ChemProp\IdealGasPP\IdealGasPP.cpp` + `IdealGasPP.h` | Multi-component 1-phase example |
| CMake build scripts | `ChemProp\WaterPP\CMakeLists.txt` / `ChemProp\IdealGasPP\CMakeLists.txt` | Build templates |

### External Resources

| Resource | Path/Link |
|----------|-----------|
| COBIA SDK headers | `C:\Program Files (x86)\Common Files\CAPE-OPEN Laboratories Network\Include\` |
| cobiaRegister registration tool | `C:\Program Files\Common Files\CAPE-OPEN Laboratories Network\COBIA\` |
| CapeWizard code generator | Same directory as above |
| COFE (PME testing) | https://www.amsterchem.com/cofe.html |
| CAPE-OPEN Standard | https://www.colan.org/ |
| IAPWS-97 | http://www.iapws.org/ |
| MinGW-w64 (MSYS2) | https://www.msys2.org/ |