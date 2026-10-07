# COBIA Unit Operation Development Guide

> Using the project's `MHExch` (multi-stream heat exchanger unit operation) as a reference example. After reading this guide, you should be able to independently write, debug, and register a COBIA Unit Operation module that can be loaded by COFE.

---

## 1. Background: What is a COBIA Unit Operation

### 1.1 Role Positioning in CAPE-OPEN

There are two major pluggable components in a process simulation system:

```
┌──────────────────────┐          ┌──────────────────────┐
│   Property Package   │          │    Unit Operation     │
│        (PP)          │          │        (UO)           │
│                      │          │                      │
│  Role: answers        │          │  Role: answers        │
│  "what it is"         │          │  "what it does"       │
│  - density, enthalpy, │          │  - heat exchange,     │
│    entropy...         │          │    distillation,      │
│  - provides to        │          │    reaction...        │
│    Material objects   │          │  - operates on        │
│                       │          │    Material objects   │
└──────────────────────┘          └──────────────────────┘
         │                                  │
         └────────────┬─────────────────────┘
                      ▼
              ┌──────────────┐
              │   PME (COFE) │
              │ Process       │
              │ Modeling      │
              │ Environment   │
              └──────────────┘
```

- **Property Package**: A pure computation component. The PME asks "what is the density at this T/P?" and the PP returns a numeric value.
- **Unit Operation**: A process component with **Ports** and **Parameters**. The PME connects material streams and calls `Validate` → `Calculate`.

### 1.2 Unit Operation vs Property Package

| Dimension | Property Package (PP) | Unit Operation (UO) |
|-----------|----------------------|---------------------|
| **Input** | Material object (T/P/composition) | Material ports + parameters |
| **Output** | Numeric values (density, enthalpy...) | Modifies outlet Material objects |
| **Has ports?** | No | Yes (Inlet/Outlet) |
| **Has parameters?** | Optional | Yes (e.g., heat exchanger hot/cold side selection) |
| **Core method** | `CalcSinglePhaseProp` | `Validate` + `Calculate` |
| **Typical implementation** | 1 class, 8 Adapters | 1 main class + N helper classes (ports, parameters, validation, solver) |

### 1.3 The 5 Required Adapters

A COBIA unit operation must inherit from the following 5 Adapters:

| # | Adapter | Corresponding CAPE-OPEN Interface | Purpose |
|---|---------|----------------------------------|---------|
| 1 | `CapeIdentificationAdapter` | ICapeIdentification | Name and description |
| 2 | `CapeUnitAdapter` | ICapeUnit | Port collection, validation, calculation |
| 3 | `CapeUtilitiesAdapter` | ICapeUtilities | Parameter collection, Initialize/Terminate |
| 4 | `CapePersistAdapter` | ICapePersist | Save/Load state |
| 5 | `CapeOpenObject` | IUnknown base | COBIA object base class |

> Compared to a Property Package's 8 Adapters, a Unit Operation only needs 5, because unit operations do not directly compute properties — they delegate to the Property Package through Material objects.

---

## 2. Core Concepts

### 2.1 Port

A port is the connection point between a unit operation and the external world. MHExch has 10 material ports (5 inlets, 5 outlets):

```cpp
// MaterialPort.h — minimal material port implementation
class MaterialPort :
    public CapeOpenObject<MaterialPort>,
    public CapeIdentificationAdapter<MaterialPort>,
    public CapeUnitPortAdapter<MaterialPort>
{
    CapeStringImpl portName;
    CapePortDirection direction;   // CAPE_INLET or CAPE_OUTLET
    CapeBoolean primary;           // whether it must be connected
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

Three port types:
- `CAPE_MATERIAL` — Material stream (most common)
- `CAPE_ENERGY` — Energy stream (heat/work)
- `CAPE_INFORMATION` — Information stream

Port direction:
- `CAPE_INLET` — Feed
- `CAPE_OUTLET` — Product

Port necessity:
- `primary=true` — Must be connected, otherwise validation fails
- `primary=false` — Optional connection

### 2.2 Parameter

Parameters allow users to configure unit operation behavior in the PME. MHExch has 5 parameters controlling the hot/cold side of each feed:

```cpp
// Parameter type inheritance chain (using string parameter as example)
class ParameterOption :
    public CapeIdentificationAdapter,   // Name
    public CapeParameterAdapter,        // Type/Mode/Validation
    public CapeParameterSpecificationAdapter,  // Default/Bounds
    public CapeStringParameterAdapter,  // getValue/putValue
    public CapeStringParameterSpecificationAdapter  // Option list
{
    CapeStringImpl value, defaultValue;
    CapeArrayStringImpl optionNames;    // {"Ignore", "Hot", "Cold"}
};
```

Three parameter types:
| Type | Adapter | Example |
|------|---------|---------|
| Real | `CapeRealParameterAdapter` | Temperature setpoint, pressure drop |
| Integer | `CapeIntegerParameterAdapter` | Number of theoretical stages |
| String/Option | `CapeStringParameterAdapter` | Hot/Cold side selection |

### 2.3 Collection

Ports and parameters are exposed to the PME through collections. COBIA provides generic templates to simplify collection implementation:

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

Key operations:
- `addItem(item)` — Register all ports and parameters in the constructor
- `Item(index)` — PME iterates by index
- `Item(name)` — PME looks up by name

### 2.4 Lifecycle: Validate → Calculate

A unit operation's execution is divided into two phases:

```
User Action           PME Call              UO Implementation
──────────           ──────────            ──────────────────
Connect stream       Connect()
Modify parameter     putValue()            → set validationStatus=NOT_VALIDATED
Click Run            Validate(message)     → check port connections, parameter validity
                                           → return true/false
                     Calculate()           → read T/P/composition from inlet
                                           → compute → write to outlet
```

**Key rules**:
1. `Validate` only executes when status is `NOT_VALIDATED`; sets `CAPE_VALID` on pass
2. Any port change or parameter change resets status to `NOT_VALIDATED`
3. Must check `validationStatus == CAPE_VALID` before `Calculate`
4. Never modify inlet Material objects (no-side-effect principle)

---

## 3. Creating a COBIA Unit Operation from Scratch

### 3.1 Environment Setup

| Component | Path/Source |
|-----------|-------------|
| **COBIA SDK** | `C:\Program Files (x86)\Common Files\CAPE-OPEN Laboratories Network\Include\` |
| **cobiaRegister.exe** | `C:\Program Files\Common Files\CAPE-OPEN Laboratories Network\COBIA\` |
| **MSYS2 MinGW-w64** | `D:\msys64\mingw64\bin\` |
| **CMake** | 3.16+ |
| **COFE** | https://www.amsterchem.com/cofe.html |

Verification:

```powershell
Test-Path "C:\Program Files (x86)\Common Files\CAPE-OPEN Laboratories Network\Include\COBIA.h"
& "D:\msys64\mingw64\bin\g++.exe" --version
```

### 3.2 Project File Structure

Below is the recommended structure for a **single-file minimal implementation**. Like Property Packages, a Unit Operation can also place all logic in 2 files:

```
MyUnit/
├── CMakeLists.txt        # Build configuration
├── MyUnit.h              # Class declaration + 5 Adapter inheritance + ports/parameters/validation/calculation
├── MyUnit.cpp            # Register function + COBIA entry point
└── Engine.h              # Calculation engine (pure math, no COBIA dependency, optional)
```

> **Design principle**: The calculation engine is completely decoupled from CAPE-OPEN interfaces and can be compiled separately as a command-line tool for unit testing.

### 3.3 Step 1: CMakeLists.txt

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

# Install to install/ChemUnit/
install(TARGETS MyUnit
    RUNTIME DESTINATION ChemUnit
    LIBRARY DESTINATION ChemUnit)
```

### 3.4 Step 2: Minimal Header File (MyUnit.h)

The template below implements the **simplest unit operation**: 1 inlet + 1 outlet, no computation, outlet = a copy of inlet.

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
    // --- Member variables ---
    CapeStringImpl name, description;
    CapeBoolean dirty;
    CapeValidationStatus validationStatus;

    // Ports
    CapeStringImpl inName, outName;
    CapeThermoMaterial inlet, outlet;

    // Collections
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
    // ICapeUnit — Ports
    // ================================================================
    CapeCollection<CapeUnitPort> ports() {
        CapeArrayCollection<CapeUnitPort> coll;
        for (auto& p : portList) coll.addItem(p);
        return coll;
    }

    // ================================================================
    // ICapeUnit — Validation
    // ================================================================
    CapeValidationStatus getValStatus() { return validationStatus; }

    CapeBoolean Validate(CapeString message) {
        if (validationStatus == CAPE_VALID) return true;

        // Check if inlet is connected
        if (!inlet) {
            message = COBIATEXT("Inlet is not connected");
            validationStatus = CAPE_INVALID;
            return false;
        }
        // Check inlet/outlet compound list consistency
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
    // ICapeUnit — Calculation
    // ================================================================
    void Calculate() {
        if (validationStatus != CAPE_VALID)
            throw cape_open_error(COBIATEXT("Unit is not in a valid state"));

        // Copy inlet to outlet (simplest implementation: pass-through)
        outlet.CopyFromMaterial(inlet);

        // Add your calculation logic here...
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
    // ICapePersist — Save/Load
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
    // Registration info
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

### 3.5 Step 3: Entry File (MyUnit.cpp)

The `.cpp` file for a unit operation is very concise — just include the header and register:

```cpp
#define COBIA_PMC_ENTRY_POINTS       // Generate COM entry points like DllRegisterServer
#define PMC_REGISTERFORALLUSERS      // Register to HKLM (visible to all users)
#include <COBIA_PMC.h>
#include "MyUnit.h"

COBIA_PMC_REGISTER(MyUnit);
```

A few **must-know** macros:
- `COBIA_PMC_ENTRY_POINTS` — Without this, exported functions like `capeRegisterObjects` / `DllRegisterServer` are not generated, causing `entry point could not be located` errors during registration
- `PMC_REGISTERFORALLUSERS` — Registers to `HKLM` instead of `HKCU`. Missing it causes linker error `undefined reference to isPMCRegistrationForAllUsers`
- `COBIA_PMC_REGISTER(ClassName)` — Macro auto-links `ClassName::Register()` and `ClassName::getObjectUUID()`

### 3.6 Step 4: Build, Install, Register

```powershell
# Configure (using the project's existing CMakePresets)
cmake --preset mingw-minsizerel

# Build only MyUnit
cmake --build build/mingw-minsizerel --target MyUnit

# Install to install/ChemUnit/
cmake --install build/mingw-minsizerel --config MinSizeRel

# Register
& "C:\Program Files\Common Files\CAPE-OPEN Laboratories Network\COBIA\cobiaRegister.exe" `
    -a "install/ChemUnit/MyUnit.dll"

# Unregister (must be done before updating the DLL)
& "C:\Program Files\Common Files\CAPE-OPEN Laboratories Network\COBIA\cobiaRegister.exe" `
    -u "install/ChemUnit/MyUnit.dll"
```

---

## 4. Advanced: Using MHExch as Reference

### 4.1 MHExch Single-Class Architecture Explained

MHExch has been refactored to use the same **single-class pattern** as Property Packages, condensing all logic into 2 files:

```
MHExch/
├── MHExch.h              ← Class declaration + inline getters + helper declarations
└── MHExch.cpp            ← All implementation + COBIA registration entry
```

**Class relationship diagram**:

```
┌──────────────────────────────────────────────┐
│         MHeatExchangerUnit                   │
│  (Single core class, directly inherits       │
│   5 Adapters)                                │
│                                              │
│  CapeOpenObject                               │
│  CapeIdentificationAdapter   ← Name/Desc      │
│  CapeUnitAdapter             ← Ports/Val/Calc │
│  CapeUtilitiesAdapter        ← Params/Init    │
│  CapePersistAdapter          ← Save/Load      │
│                                              │
│  Owns:                                        │
│    MaterialPort (×10)       ← Material ports  │
│    PortCollection           ← Port collection  │
│                                              │
│  Private methods:                             │
│    validateMaterialPorts()  ← Port connection │
│    validateMSHEXSides()     ← Hot/Cold check  │
│    flashOutlet()            ← Outlet flash    │
└──────────────────────────────────────────────┘
```

**Call relationships**:

```
Validate(message)
  └→ validateMaterialPorts()   // Primary feed must be connected + compound consistency
  └→ validateMSHEXSides()      // Hot/Cold sides cannot be identical

Calculate()
  └→ flashOutlet(i) × nPorts  // Per-outlet T/P flash
      └→ CopyFromMaterial(inlet)
      └→ CalcEquilibrium(TP)
```

### 4.2 Single-Class Pattern vs Multi-File Split

The single-class pattern implements parameter/validation/solver logic as **private methods** of the core class, rather than separate classes:

```cpp
class MHeatExchangerUnit : public /* 5 Adapters */ {
    // Validation: private methods, not a Validator class
    CapeBoolean validateMaterialPorts(CapeString msg);
    CapeBoolean validateMSHEXSides(CapeString msg);

    // Solver: private methods, not a Solver class
    void flashOutlet(size_t idx);

public:
    CapeBoolean Validate(CapeString msg) {  // Directly calls private methods
        return validateMaterialPorts(msg) && validateMSHEXSides(msg);
    }
    void Calculate() {
        for (size_t i = 0; i < nPorts; i++) flashOutlet(i);
    }
};
```

| Dimension | Multi-File Split | Single-Class Pattern |
|-----------|-----------------|---------------------|
| File count | 7~11 files | **2 files** (.h + .cpp) |
| Readability | Requires jumping across files | **Full picture at a glance** |
| Reusability | Validator/Solver can be reused | Only copy-paste reuse |
| Compile speed | Small file changes compile fast | Any change recompiles all |
| Suitable scale | Distillation columns, reactors | Heat exchangers, pumps, mixers |
| PP style match | Not applicable | ✅ Consistent with IdealGasPP |
| **Recommended** | Complex units | **Most cases** |

> The project's MHExch has been refactored from 11 files to the 2-file single-class pattern. Experience shows: for modules at the heat exchanger level of complexity, the single-class pattern significantly reduces over-engineering and improves readability.

### 4.3 Adding Real Computation: Pass-Through → Heat Exchanger

The `Calculate()` in the minimal template is just a pass-through copy. To become a real heat exchanger, the core logic is:

```cpp
void Calculate() {
    // 1. Read T, P, composition, flow from inlet
    CapeReal T_in, P_in;
    CapeArrayReal X, flow;
    inlet.GetOverallTPFraction(T_in, P_in, X);
    inlet.GetOverallProp(COBIATEXT("totalFlow"),
        COBIATEXT("mole"), flow);

    // 2. Call calculation engine (your mathematical model)
    CapeReal T_out = myHeatExchangerModel(T_in, P_in, flow[0], X);

    // 3. Write to outlet
    outlet.SetOverallTPFraction(T_out, P_in, X);
    outlet.SetOverallProp(COBIATEXT("totalFlow"),
        COBIATEXT("mole"), flow);

    // 4. Flash outlet (if Property Package supports phase equilibrium)
    CAPEOPEN_1_2::CapeThermoEquilibriumRoutine eq(outlet);
    eq.CalcEquilibrium(flashCond_TP, COBIATEXT(""));
}
```

### 4.4 Complete Constructor with Ports

The minimal template omitted port construction. The full version needs to create and register ports in the constructor:

```cpp
MyUnit() : name(unitName), description(unitDesc),
           dirty(false), validationStatus(CAPE_NOT_VALIDATED),
           inName(COBIATEXT("Inlet")), outName(COBIATEXT("Outlet"))
{
    // Create ports
    inlet  = new MyMaterialPort(name, validationStatus,
               inName, CAPE_INLET, true);
    outlet = new MyMaterialPort(name, validationStatus,
               outName, CAPE_OUTLET, true);

    // Register to port list (inlet must precede outlet; PME pairs by index)
    portList.push_back(inlet);
    portList.push_back(outlet);
}
```

---

## 5. Common Issues and Debugging

### 5.1 UUID Conflicts

```powershell
# Symptom: "already registered" error during registration, or COFE loads old DLL
# Solution: Generate a new UUID
powershell -c "[guid]::NewGuid()"
```

### 5.2 Your Module Doesn't Appear in COFE

Checklist:
1. Was the DLL registered successfully? (`cobiaRegister -a` outputs `OK`)
2. Is `categoryId_UnitOperation` correctly added?
3. Has COFE been restarted? (COFE scans registered components at startup)
4. Does 32/64-bit match? (COFE 64-bit requires 64-bit DLL)

### 5.3 Validate Is Called Every Time

This is normal. `Connect/Disconnect/putValue` all reset `validationStatus` to `NOT_VALIDATED`, and the PME subsequently calls `Validate`. Ensure `Validate` checks at the start:

```cpp
if (validationStatus == CAPE_VALID) return true;  // Idempotent
```

### 5.4 Compiler Warning: `control reaches end of non-void function`

Triggered when a function has multiple `return` paths but one branch is not covered. Ensure all branches have return values, or add a fallback `return` outside the loop.

### 5.5 COBIA vs Traditional COM Migration

| Traditional COM | COBIA |
|-----------------|-------|
| `BSTR` / `CBStr` | `CapeStringImpl` / `CapeString` |
| `VARIANT` | `CapeValue` / `CapeArrayValue` |
| `HRESULT` return | C++ exceptions `throw cape_open_error()` |
| `DllRegisterServer` | `cobiaRegister.exe -a` |
| ATL `CComObject` | `CapeOpenObject<T>` |
| `BEGIN_TRY`/`END_TRY` | Not needed |

### 5.6 CMake One-Click Registration

Create a `register_*` custom target in CMakeLists.txt to build, install, and register in one step:

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

Usage:
```powershell
cmake --build build --target register_MyUnit
```

> `copy_if_different` only copies when the DLL has changed, avoiding unnecessary file timestamp updates.

### 5.7 Different UUIDs for Debug/Release

When debugging, you often need both Debug and Release versions installed in COFE simultaneously. Give the two versions different UUIDs (differing in the last byte) to coexist:

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

COFE will treat them as 4 independent components, allowing you to compare Debug and Release behavior side by side.

### 5.8 Key Resources

| Resource | Link |
|----------|------|
| COBIA SDK Download | https://colan.repositoryhosting.com/trac/colan_cobia/downloads |
| CO-LaN Official Site | https://www.colan.org/ |
| COFE Download | https://www.amsterchem.com/cofe.html |
| Project's Property Package Dev Guide | `../ChemProp/docs/COBIA物性包开发指南.md` |
| Project ChemLab Roadmap | `../../Docs/ChemLab_Road_Map.md` |