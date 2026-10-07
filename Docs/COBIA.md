# CAPE-OPEN Binary Interop Architecture (COBIA)

> **Version**: 1.2.2.1 | **Copyright**: CO-LaN (CAPE-OPEN Laboratories Network) 2024-2026
> **Website**: https://www.colan.org/

---

## 1. What is CAPE-OPEN

CAPE-OPEN is the **de facto standard** for interoperability between process modeling software components. Built on common software technologies (COM and CORBA), it is an open, multi-platform, unified, and free standard. In practice, it allows third-party software components (such as specialized property packages or unit operation models) to be used in a "plug-and-play" manner within most commercial process simulation environments.

---

## 2. What is COBIA

COBIA (CAPE-OPEN Binary Interop Architecture) is a **middleware platform and API** specifically designed for CAPE-OPEN (but not limited to CAPE-OPEN).

### 2.1 As Middleware

COBIA provides:
- **Object Registration & Discovery**
- **Object Instantiation**
- **Runtime API Functions**
- **Cross-platform Support** (Windows / Linux / macOS)

### 2.2 Advantages over COM

Traditional CAPE-OPEN is based on COM technology, which is mature but limited to Windows. COBIA offers the following advantages over COM:

| Feature | COM | COBIA |
|---------|-----|-------|
| Type Safety | Weakly typed (VARIANT) | **Strongly typed**, reducing error surface |
| Memory Management | Callee-allocated | **Caller-allocated**, simpler and more efficient |
| Cross-platform | Windows only | Windows / Linux / macOS |
| Developer Experience | Raw COM API | Wrapper / Adapter layers to simplify development |

### 2.3 COBIA-COM Interoperability (COMBIA)

On Windows:
- A COBIA PME can **automatically access** any COM PMC registered on the system
- When registering a COBIA PMC, it is **automatically registered as COM** as well, allowing COBIA PMCs to be used by COM PMEs
- The library that manages COBIA-COM interoperability is called **COMBIA**, installed automatically with COBIA

```
┌──────────┐     COMBIA      ┌──────────┐
│  COBIA   │ ◄────────────► │   COM    │
│   PME    │   Interop Layer │   PMC    │
└──────────┘                 └──────────┘
```

---

## 3. SDK Installation & File Structure

### 3.1 Windows Installation Paths

| Architecture | Path | Contents |
|--------------|------|----------|
| x64 (64-bit) | `C:\Program Files\Common Files\CAPE-OPEN Laboratories Network\` | Runtime DLLs, registration tools, SDK tools |
| x86 (32-bit) | `C:\Program Files (x86)\Common Files\CAPE-OPEN Laboratories Network\` | Headers, reference docs, SDK tools |

### 3.2 64-bit Runtime Directory (`COBIA/`)

| File | Purpose |
|------|---------|
| `COBIA.dll` | COBIA core runtime library |
| `COBIAMarshal.dll` | Inter-process communication marshaling library |
| `COMBIA.dll` | COBIA-COM interop bridge library |
| `COMBIAClassFactory.dll` | COM class factory bridge |
| `COBIAIDLParser.dll` | IDL parser |
| `COBIAPMCHost.exe` | PMC host process |
| `COBIAResource.dll` | Resource library |
| `ProxyInterfaces_CAPEOPEN_1_2.dll` | CAPE-OPEN 1.2 proxy interfaces |
| `cobiaRegister.exe` | **Command-line** registration tool |
| `wCobiaRegister.exe` | **GUI** registration tool |
| `license.txt` | License file |

### 3.3 64-bit SDK Directory (`COBIA-SDK/`)

| File | Purpose |
|------|---------|
| `COBIA_CodeGen.exe` | **Code generator** — generates C++/COM code from CIDL |
| `cobiaCmdPrompt.bat` | Developer command prompt script |
| `CodeGenerator_CPP.dll` | C++ code generation backend |
| `CodeGenerator_COM.dll` | COM code generation backend |

### 3.4 32-bit Header Directory (`Include/`)

47 header files total, grouped by function:

**Base Types & Utilities**
| Header | Description |
|--------|-------------|
| `COBIA.h` | **Main entry header**, includes all core definitions |
| `COBIA_BasicDataTypes.h` | Basic data types (CapeReal, CapeInteger, CapeUUID, etc.) |
| `COBIA_Enumerations.h` | Enumeration type definitions |
| `COBIA_Error.h` | Error code definitions |
| `COBIA_UndefinedValues.h` | Undefined value constants |
| `COBIA_SmartPointer.h` | Smart pointers (CobiaSmartPointer, CapeSmartPointer) |
| `StdAdditions.h` | Standard extensions |

**Interface Definition Layer**
| Header | Description |
|--------|-------------|
| `COBIA_Interfaces.h` | COBIA core interfaces (ICobiaBase, ICapeInterface, container type interfaces) |
| `COBIA_DataImplInterfaces.h` | Data implementation interfaces |
| `COBIA_RegistryInterfaces.h` | Registry access interfaces (Registry Key/Writer, PMCRegistrar, PMCEnumerator) |
| `COBIA_ClassFactories.h` | Class factory interfaces |
| `COBIA_IDLInterfaces.h` | IDL interfaces |
| `COBIA_MarshalInterfaces.h` | Marshaling interfaces |
| `CapeInterfaces_1_2.h` | **CAPE-OPEN 1.2 standard interfaces** (core!) |

**Wrapper Layer (Consumer Interfaces)**
| Header | Description |
|--------|-------------|
| `COBIA_DataWrappers.h` | Data type wrappers |
| `COBIA_IDLInterfaceWrappers.h` | IDL interface wrappers |
| `COBIA_MarshalInterfaceWrappers.h` | Marshaling interface wrappers |
| `COBIA_RegistryWrappers.h` | Registry interface wrappers |
| `CapeInterfaceWrappers_1_2.h` | CAPE-OPEN 1.2 interface wrappers |
| `COBIA_IDLParser_Wrappers.h` | IDL parser wrappers |

**Adapter Layer (Implementing Interfaces)**
| Header | Description |
|--------|-------------|
| `CapeInterfaceAdapters_1_2.h` | CAPE-OPEN 1.2 interface adapters |
| `COBIA_ConstDataAdapters.h` | Const data adapters |
| `COBIA_MarshalInterfaceAdapters.h` | Marshaling interface adapters |

**Client & Implementation**
| Header | Description |
|--------|-------------|
| `COBIA_ClientBaseClasses.h` | Client base classes |
| `COBIA_COBIADataClasses.h` | COBIA data class implementations |
| `COBIA_StandardDataContainers.h` | Standard data container implementations |

**Platform Abstraction Layer**
| Header | Description |
|--------|-------------|
| `COBIA_Windows.h` / `COBIA_Windows_Stub.h` | Windows platform abstraction |
| `COBIA_POSIX.h` / `COBIA_POSIX_Stub.h` | POSIX platform abstraction |
| `COBIA_Marshal.h` / `COBIA_Marshal_Windows.h` / `COBIA_Marshal_POSIX.h` | Marshaling platform abstraction |

**Tools**
| Header | Description |
|--------|-------------|
| `COBIA_PMC.h` | PMC registration & entry point macros |
| `COBIA_Functions.h` | COBIA API function declarations |
| `COBIA_IDLParser.h` | IDL parser |
| `COBIA_TestCOBIAVersion.h` | Version testing |
| `cape_open_error.h` / `CapeErrorImpl.h` | Error handling |

### 3.5 Reference Documentation (`COBIA Reference/`)

Doxygen-generated HTML reference documentation covering detailed descriptions of all interfaces, enumerations, and data types.

---

## 4. Core Concepts

### 4.1 Interfaces

Two software components interact through **interfaces**. An interface is a fixed set of functions identified by an **Interface Identifier (IID)** — a Globally Unique Identifier (GUID).

COBIA interfaces follow the platform's **C ABI** for binary compatibility. Each interface consists of two parts:

```
┌─────────────────┐        ┌──────────────────────┐
│  void *me       │───────►│  Object instance      │
│  VTable *vTbl   │───────►│  ┌─────────────────┐ │
└─────────────────┘        │  │ addReference    │ │
                           │  │ release         │ │
   ICapeInterface          │  │ queryInterface  │ │
                           │  │ getLastError    │ │
                           │  └─────────────────┘ │
                           └──────────────────────┘
```

Key design principles:
- `me` is a void pointer to the object instance
- `vTbl` points to the virtual function table; **all instances of the same type share a single VTable**
- Every function's first parameter in the VTable is the `me` pointer
- Interfaces are pure C structs, enabling cross-compiler, cross-language binary compatibility

### 4.2 Reference Counting & Lifecycle

```c
struct ICapeInterface {
    struct VTable {
        void (COBIAMETHOD *addReference)(void *me);   // Increment reference count
        void (COBIAMETHOD *release)(void *me);         // Decrement; destroy when zero
        CapeResult (COBIAMETHOD *queryInterface)(void *me, const CapeUUID &uuid, ICapeInterface **ptr);
        CapeResult (COBIAMETHOD *getLastError)(void *me, ICapeError *&ptr);
    };
    void *me;
    VTable *vTbl;
};
```

### 4.3 Interface → Wrapper → Adapter Three-Layer Architecture

COBIA provides three layers of abstraction to simplify interface usage and implementation:

```
┌────────────────────────────────────────────────────┐
│                   Consumer (PME)                    │
│                                                     │
│  Wrapper (C++ friendly encapsulation)                │
│  ├─ Automatic reference count management (smart ptr) │
│  ├─ Check return values and convert to C++ exceptions│
│  ├─ Convert output parameters to return values       │
│  └─ Hide the me pointer                              │
│                                                     │
├────────────────────────────────────────────────────┤
│            Raw C Interface (Binary Compatible)       │
│  struct ICapeXXX { void *me; VTable *vTbl; };       │
├────────────────────────────────────────────────────┤
│                   Implementor (PMC)                  │
│                                                     │
│  Adapter (base class template)                       │
│  ├─ Convert static C functions to member functions   │
│  ├─ Hide the me pointer                              │
│  ├─ Convert interface parameters to Wrapper types    │
│  ├─ Auto-capture exceptions → CAPE-OPEN error handling│
│  └─ Auto-register interface in queryInterface        │
│                                                     │
└────────────────────────────────────────────────────┘
```

**Example**: Using `ICapeThermoPropertyPackageManager`

*Raw C Interface* (unwieldy):
```c
ICapeThermoPropertyPackageManager *manager;
CapeResult res = manager->vTbl->getPropertyPackageList(
    manager->me, packageNames);
```

*Wrapper* (C++ friendly):
```cpp
CAPEOPEN_1_2::CapeThermoPropertyPackageManager manager(unitInterface);
CapeArrayString packages = manager.getPropertyPackageList();
// Reference counting auto-managed, errors auto-converted to exceptions
```

*Adapter* (implementing the interface):
```cpp
class MyPropertyPackageManager :
    public CapeThermoPropertyPackageManagerAdapter<MyPropertyPackageManager>
{
    CapeArrayString getPropertyPackageList() override {
        // Implement business logic
        CapeArrayString result;
        result.push_back("MyPP");
        return result;
    }
};
```

---

## 5. Basic Data Types

### 5.1 Scalar Types

| COBIA Type | C++ Underlying Type | Description |
|-----------|---------------------|-------------|
| `CapeResult` | `uint32_t` | Return value, non-zero indicates error |
| `CapeReal` | `double` | Double-precision real, undefined = CapeRealUndefined (NaN) |
| `CapeInteger` | `int32_t` | 32-bit signed integer, undefined = CapeIntegerUNDEFINED |
| `CapeEnumeration` | `int32_t` | Enumeration base type |
| `CapeBoolean` | `uint32_t` | Boolean value |
| `CapeCharacter` | `COBIACHAR` | Character (platform-dependent encoding) |
| `CapeByte` | `uint8_t` | Byte (for persisting binary data) |

### 5.2 UUID

```cpp
struct CapeUUID {
    unsigned char data[16];  // 16-byte GUID
    // Supports ==, !=, <, > comparison
    // Windows: supports interop with GUID
    static CapeUUID nullValue();
    bool isnull() const;
    const unsigned char *getData() const;
};

class ExtCapeUUID : public CapeUUID {
    // Extended UUID with formatted string support and parsing
};
```

### 5.3 Container Type Interfaces

| Interface | Description | Wrapper Class |
|-----------|-------------|---------------|
| `ICapeString` | String | `CapeString` |
| `ICapeArrayReal` | Real array | `CapeArrayReal` |
| `ICapeArrayInteger` | Integer array | `CapeArrayInteger` |
| `ICapeArrayBoolean` | Boolean array | `CapeArrayBoolean` |
| `ICapeArrayString` | String array | `CapeArrayString` |
| `ICapeArrayEnumeration` | Enumeration array | `CapeArrayEnumeration` |
| `ICapeArrayByte` | Byte array | `CapeArrayByte` |
| `ICapeValue` | Variant value | `CapeValue` |
| `ICapeArrayValue` | Variant value array | `CapeArrayValue` |

### 5.4 Container Implementation Classes

COBIA provides multiple ready-made data container implementations:

| Implementation Class | Based On | Use Case |
|----------------------|----------|----------|
| `CapeStringImpl` | `std::string`/`std::wstring` | Default string implementation |
| `CapeArrayRealImpl` | `std::vector<double>` | Default real array implementation |
| `CapeArrayIntegerImpl` | `std::vector<int32_t>` | Default integer array implementation |
| `CapeStringAdapter` | `std::string` Adapt | Adapt existing string |
| `CapeArrayRealScalarImpl` | Single-element optimized | Scalar properties like temperature/pressure |

**Read-only Const Adapters** (zero-copy, pointing to existing data):
| Class | Description |
|-------|-------------|
| `ConstCapeString` | Points to C string |
| `ConstCapeArrayReal` | Points to existing real array |
| `ConstEmptyCapeArrayString` | Empty string array |

---

## 6. Error Handling

### 6.1 Error Code System

```cpp
// Success
#define COBIAERR_NoError   0

// CAPE-OPEN error (details via ICapeError)
#define COBIAERR_CAPEOPENError   1

// General errors (100-199)
#define COBIAERR_UnknownError            100
#define COBIAERR_CriticalError           101
#define COBIAERR_OutOfMemory             102
#define COBIAERR_NullPointer             103
#define COBIAERR_InvalidArgument         104
#define COBIAERR_NoSuchInterface         105
#define COBIAERR_Denied                  106
#define COBIAERR_NoSuchItem              107
#define COBIAERR_NotImplemented          108
#define COBIAERR_NoService               109
#define COBIAERR_InvalidOperation        115

// Registry errors (500-599)
#define COBIAERR_Registry_NotFound       500
#define COBIAERR_Registry_AccessDenied   501
#define COBIAERR_Registry_Corrupt        502

// Marshaling errors (600-699)
#define COBIAERR_Marshal_Error           601
#define COBIAERR_Marshal_ConnectionClosed 602
```

### 6.2 ICapeError Interface

```c
struct ICapeError {
    struct VTable {
        ICobiaBase::VTable base;
        CapeResult (*getErrorText)(void *me, ICapeString *errorText);
        CapeResult (*getCause)(void *me, ICapeError *&cause);      // Supports error chaining
        CapeResult (*getSource)(void *me, ICapeString *componentDescription);
        CapeResult (*getScope)(void *me, ICapeString *errorScope);
    };
};
```

---

## 7. CAPE-OPEN 1.2 Standard Interfaces

> Defined in `CapeInterfaces_1_2.h`, maintained in the `CAPEOPEN_1_2` namespace.

### 7.1 Interface Inheritance Hierarchy

```
ICapeInterface (reference counting + queryInterface + error)
  ├── ICapeIdentification (name + description)
  │     ├── ICapeUnit (ports + Validate + Calculate)
  │     ├── ICapeThermoMaterial (T/P/flow/composition/phase/physical properties)
  │     │     ├── ICapeThermoMaterialContext
  │     │     └── ICapeThermoMaterialPetro (oil fractions)
  │     ├── ICapeUnitPort (type/direction/connection)
  │     └── ...
  ├── ICapeCollection (collection iteration)
  ├── ICapeParameter (validation/mode/type)
  │     ├── ICapeRealParameter
  │     ├── ICapeIntegerParameter
  │     ├── ICapeStringParameter
  │     ├── ICapeBooleanParameter
  │     └── ICapeArrayParameter...
  ├── ICapeParameterSpecification
  └── ...
```

### 7.2 ICapeUnit — Unit Operation

```c
struct ICapeUnit {
    struct VTable {
        ICapeInterface::VTable base;
        CapeResult (*ports)(void *me, ICapeCollection *&ports);     // Get port collection
        CapeResult (*getValStatus)(void *me, CapeValidationStatus &ValStatus);
        CapeResult (*Calculate)(void *me);                           // Execute calculation!
        CapeResult (*Validate)(void *me, ICapeString *message, CapeBoolean &isValid);
    };
    void *me;
    VTable *vTbl;
    static CapeUUID getInterfaceUUID() { return {...}; }
};
```

**Port Interface ICapeUnitPort**:
```c
struct ICapeUnitPort {
    struct VTable {
        ICapeInterface::VTable base;
        CapeResult (*getPortType)(void *me, CapePortType &portType);          // MATERIAL/ENERGY/INFORMATION
        CapeResult (*getDirection)(void *me, CapePortDirection &portDirection); // INLET/OUTLET
        CapeResult (*getConnectedObject)(void *me, ICapeInterface *&connectedObject);
        CapeResult (*Connect)(void *me, ICapeInterface *objectToConnect);
        CapeResult (*Disconnect)(void *me);
    };
};
```

### 7.3 ICapeThermoMaterial — Thermodynamic Material Object

The most central interface, used for all property and phase equilibrium calculations:

```c
struct ICapeThermoMaterial {
    struct VTable {
        ICapeInterface::VTable base;
        // === Components & Phases ===
        CapeResult (*GetComponentList)(void *me, ICapeArrayString *componentIds, ...);
        CapeResult (*GetPresentPhases)(void *me, ICapeArrayString *phaseLabels, ICapeArrayEnumeration *phaseStatus);

        // === Overall Properties (bulk mixture) ===
        CapeResult (*GetOverallProp)(void *me, ICapeString *property, ICapeString *basis, ICapeArrayReal *results);
        CapeResult (*SetOverallProp)(void *me, ICapeString *property, ICapeString *basis, ICapeArrayReal *values);
        CapeResult (*GetOverallTPFraction)(void *me, CapeReal &T, CapeReal &P, ICapeArrayReal *composition);
        CapeResult (*SetOverallTPFraction)(void *me, CapeReal T, CapeReal P, ICapeArrayReal *composition);

        // === Single-Phase Properties ===
        CapeResult (*GetSinglePhaseProp)(void *me, ICapeString *property, ICapeString *phaseLabel, ICapeString *basis, ICapeArrayReal *results);
        CapeResult (*SetSinglePhaseProp)(void *me, ICapeString *property, ICapeString *phaseLabel, ICapeString *basis, ICapeArrayReal *values);
        CapeResult (*GetTPFraction)(void *me, ICapeString *phaseLabel, CapeReal &T, CapeReal &P, ICapeArrayReal *composition);

        // === Two-Phase Properties (e.g. surface tension) ===
        CapeResult (*GetTwoPhaseProp)(void *me, ICapeString *property, ICapeArrayString *phaseLabels, ICapeString *basis, ICapeArrayReal *results);
        CapeResult (*SetTwoPhaseProp)(void *me, ICapeString *property, ICapeArrayString *phaseLabels, ICapeString *basis, ICapeArrayReal *values);

        // === Phase State ===
        CapeResult (*SetPresentPhases)(void *me, ICapeArrayString *phaseLabels, ICapeArrayEnumeration *phaseStatus);
    };
};
```

**Common Property Identifiers**: `"temperature"`, `"pressure"`, `"enthalpy"`, `"entropy"`, `"density"`, `"viscosity"`, `"surfaceTension"`, `"heatCapacity"`, `"molecularWeight"`, etc.

**Common Basis**: `"mole"`, `"mass"`, `"moleFraction"`, `"massFraction"`, etc.

### 7.4 ICapeThermoPropertyPackageManager — Property Package Manager

```c
struct ICapeThermoPropertyPackageManager {
    struct VTable {
        ICapeInterface::VTable base;
        CapeResult (*getPropertyPackageList)(void *me, ICapeArrayString *PackageNames);
        CapeResult (*GetPropertyPackage)(void *me, ICapeString *PackageName, ICapeInterface *&package);
    };
};
```

### 7.5 ICapeThermoPropertyPackage — Property Package

Key calculation interfaces implemented by property packages:
- **ICapeThermoMaterialContext** — Set/get the material object currently being operated on
- **ICapeThermoPropertyRoutine** — Calculate properties (`CalcSinglePhaseProp`, `CalcTwoPhaseProp`)
- **ICapeThermoEquilibriumRoutine** — Calculate phase equilibrium (`CalcEquilibrium`)
- **ICapeThermoUniversalConstant** — Universal constant retrieval

### 7.6 ICapeCollection — Collection

```c
struct ICapeCollection {
    struct VTable {
        ICapeInterface::VTable base;
        CapeResult (*ItemByIndex)(void *me, CapeInteger index, ICapeInterface *&item);
        CapeResult (*ItemByName)(void *me, ICapeString *name, ICapeInterface *&item);
        CapeResult (*getCount)(void *me, CapeInteger &itemCount);
    };
};
```

Used for iterating port lists, parameter lists, property package lists, etc.

### 7.7 ICapeParameter — Parameter Interface

Supports multiple parameter types:
- **ICapeRealParameter** — Real (with bounds, default value, units)
- **ICapeIntegerParameter** — Integer
- **ICapeStringParameter** — String (with optional restricted option list)
- **ICapeBooleanParameter** — Boolean
- **ICapeArrayReal/Integer/String/BooleanParameter** — Array parameters

All parameters support: validation status (`getValStatus`), read/write mode (`getMode`), type (`getType`), validation (`Validate`), reset (`Reset`)

### 7.8 ICapeIdentification — Component Identification

All CAPE-OPEN objects implement this interface:
```c
struct ICapeIdentification {
    CapeResult (*getComponentName)(void *me, ICapeString *name);
    CapeResult (*putComponentName)(void *me, ICapeString *name);
    CapeResult (*getComponentDescription)(void *me, ICapeString *desc);
    CapeResult (*putComponentDescription)(void *me, ICapeString *desc);
};
```

---

## 8. Category IDs — Component Classification

Defined in `CapeCategories.h`, used to identify PMC types in the registry:

| Category ID Constant | UUID | Description |
|----------------------|------|-------------|
| `categoryId_UnitOperation` | `678C09A5-...` | Unit operation PMC |
| `categoryId_PropertyPackageManager` | `CF51E083-...` | Property package manager PMC |
| `categoryId_StandAlonePropertyPackage` | `CF51E084-...` | Standalone property package PMC |
| `categoryId_PhysicalPropertyCalculator` | `CF51E085-...` | Physical property calculator PMC |
| `categoryId_EquilibriumCalculator` | `CF51E086-...` | Equilibrium calculator PMC |
| `categoryId_FlowsheetMonitoringComponent` | `7BA1AF89-...` | Flowsheet monitoring PMC |

---

## 9. PMC Registration & Instantiation

### 9.1 Registration Tools

| Tool | Type | Description |
|------|------|-------------|
| `cobiaRegister.exe` (64-bit) | CLI | Register/unregister COBIA PMCs |
| `wCobiaRegister.exe` (64-bit) | GUI | Graphical registration management tool |

### 9.2 Registration Scope

- **All Users**: Register to `HKEY_LOCAL_MACHINE`, requires administrator privileges
- **Current User**: Register to `HKEY_CURRENT_USER`

### 9.3 PMC Entry Points (COBIA_PMC.h)

Each PMC DLL must implement the following entry points:

```cpp
// In a .cpp file, define COBIA_PMC_ENTRY_POINTS before #include <COBIA_PMC.h>
#define COBIA_PMC_ENTRY_POINTS
#include <COBIA_PMC.h>

// Registration macro — use in PMC class .cpp
COBIA_PMC_REGISTER(MyUnitOperation)
```

The PMC class must implement:
```cpp
class MyUnitOperation {
public:
    // Registration function — provides name, version, Category ID, etc. to the registrar
    static void Register(CapePMCRegistrar registrar);

    // Returns the object UUID
    static const CapeUUID getObjectUUID();
};

// Registration scope
bool isPMCRegistrationForAllUsers(); // Or define PMC_REGISTERFORALLUSERS
```

### 9.4 Instantiation Flow

```cpp
// 1. Initialize COBIA
capeInitialize();   // Load COBIA.dll and retrieve function pointers

// 2. Enumerate registered PMCs
CapePMCEnumerator enumerator = capeGetPMCEnumerator();
while (enumerator.hasMore()) {
    CapePMCInfo info = enumerator.next();
    // info.name, info.uuid, info.categoryIDs, info.vendor...
}

// 3. Create PMC instance
CapeInterface unitInterface;
capeCreatePMCInstance(uuid, CapePMCCreationFlags(), &unitInterface);

// 4. Obtain specific interface
CAPEOPEN_1_2::CapeUnit capeUnit(unitInterface);

// 5. Use
capeUnit.Calculate();

// 6. Cleanup — smart pointers auto-manage, or manual release
```

### 9.5 COBIA API Functions

Accessed via the `COBIAFUNCTIONS` struct (defined in `COBIA_Functions.h`):

| Function | Description |
|----------|-------------|
| `capeGenUUID()` | Generate UUID |
| `capeStringFromUUID()` | UUID → string |
| `capeUUIDFromString()` | String → UUID |
| `capeGetCobiaVersion()` | Get COBIA version |
| `capeGetCobiaFolder()` | COBIA install directory |
| `capeGetCobiaDataFolder()` | Machine-level data directory |
| `capeGetCobiaUserDataFolder()` | User-level data directory |
| `capeGetErrorDescription()` | Error code → description text |
| `capeGetRegistryKey()` | Get read-only registry access |
| `capeGetRegistryWriter()` | Get registry write access |
| `capeGetPMCEnumerator()` | Get PMC enumerator |
| `capeGetLibraryEnumerator()` | Get library enumerator |
| `capeCreatePMCInstance()` | Create PMC instance |
| `cobiaDepersistFromTransitionFormat()` | Persistence format conversion |

---

## 10. Registry Access

### 10.1 Read Access

```cpp
// Get registry key
ICapeRegistryKey *key;
capeGetRegistryKey("Software\\CO-LaN\\CAPE-OPEN", &key);

// Read value
CapeInteger value;
key->vTbl->getIntegerValue(key->me, "Version", nullptr, value);
```

### 10.2 Write Access (Transactional Mode)

```cpp
// Get writer
ICapeRegistryWriter *writer;
capeGetRegistryWriter(true, &writer);  // true = all users

// Get PMC registrar
ICapePMCRegistrar *registrar;
writer->getPMCRegistrar(&registrar);

registrar->putName("MyUnitOperation");
registrar->putCapeVersion("1.2");
registrar->addCatID(categoryId_UnitOperation);
registrar->putUUID(myUUID);
registrar->addLocation(PMCService_Inproc, "path/to/mydll.dll");

registrar->commit();   // Commit to writer
writer->commit();       // Write to registry
```

---

## 11. Enumeration Types

Defined in `COBIA_Enumerations.h` and `CapeInterfaces_1_2.h`:

### Service Types (CapePMCServiceType)
| Value | Description |
|-------|-------------|
| `PMCService_Inproc32` | 32-bit in-process |
| `PMCService_Inproc64` | 64-bit in-process |
| `PMCService_COM32` | 32-bit COM (Windows only) |
| `PMCService_COM64` | 64-bit COM (Windows only) |
| `PMCService_Remote` | Remote |
| `PMCService_Local` | Local out-of-process |

### Port Type & Direction
| Enum | Value | Description |
|------|-------|-------------|
| `CapePortType::CAPE_MATERIAL` | 0 | Material port |
| `CapePortType::CAPE_ENERGY` | 1 | Energy port |
| `CapePortType::CAPE_INFORMATION` | 2 | Information port |
| `CapePortDirection::CAPE_INLET` | 0 | Inlet |
| `CapePortDirection::CAPE_OUTLET` | 1 | Outlet |

### Solver Status
| Value | Description |
|-------|-------------|
| `CAPE_SOLVED` | Solved |
| `CAPE_NOT_SOLVED` | Not solved |

### Validation Status
| Value | Description |
|-------|-------------|
| `CAPE_VALID` | Valid |
| `CAPE_INVALID` | Invalid |

---

## 12. Code Generator (COBIA_CodeGen)

The COBIA SDK provides a code generation tool that generates code from CAPE-OPEN CIDL (CAPE-OPEN Interface Definition Language) files:

```powershell
# Common options
COBIA_CodeGen -c          # Generate C++ Adapter code
COBIA_CodeGen -i          # Generate interface headers
COBIA_CodeGen -A          # Generate all Adapters
COBIA_CodeGen -u          # Update mode
COBIA_CodeGen -r          # Recursive processing
```

Generated code includes:
- Raw C interface definitions (`CapeInterfaces_1_2.h`)
- C++ Wrapper classes (`CapeInterfaceWrappers_1_2.h`)
- C++ Adapter base classes (`CapeInterfaceAdapters_1_2.h`)
- Category ID constants (`CapeCategories.h`)

---

## 13. Usage Patterns in ChemLab

### 13.1 Header Includes

```cpp
#include <COBIA.h>                    // COBIA core
#include <CapeInterfaces_1_2.h>       // CAPE-OPEN 1.2 interfaces
```

### 13.2 Unit Operation Enumeration & Loading

The project manages unit operations via `UnitOperationManager`:
- `enumerateViaCOBIA()` — Enumerate via COBIA API
- `enumerateViaCOM()` — Enumerate via COM (Windows)
- `enumerateViaRegistry()` — Read registry directly

### 13.3 Property Package Loading

`PropertyPackageManager` implements:
- `enumeratePackages()` — Enumerate available property packages
- `loadPackage()` — Load property package by progId
- `calculateEquilibrium()` — Invoke phase equilibrium calculation
- `calculateProperty()` — Invoke single-phase property calculation

### 13.4 Material Stream Wrapper

`CapeMaterialStream` wraps `ICapeThermoMaterial`:
- `flashTP()` — T/P flash
- `flashPH()` — P/H flash
- `getSinglePhaseProp()` — Get single-phase properties
- `setTemperature()` / `setPressure()` — Set state

### 13.5 Unit Operation Wrapper

`CapeUnitWrapper` wraps `ICapeUnit`:
- `enumeratePorts()` — Enumerate ports
- `connectPort()` / `disconnectPort()` — Connect/disconnect ports
- `validate()` / `solve()` — Validate and solve

---

## 14. Compilation & Linking

### 14.1 CMake Configuration

```cmake
# Set COBIA SDK paths
set(COBIA_INCLUDE_DIR "C:/Program Files (x86)/Common Files/CAPE-OPEN Laboratories Network/Include")
set(COBIA_LIB_DIR "C:/Program Files/Common Files/CAPE-OPEN Laboratories Network/COBIA")

# Headers (header-only library, no .lib linking needed)
target_include_directories(MyPMC PRIVATE ${COBIA_INCLUDE_DIR})

# COBIA.dll loaded at runtime (via capeInitialize)
```

COBIA is a **header-only** library — all implementations are in header files (template classes), with no need to link against `.lib` files. At runtime, `COBIA.dll` is dynamically loaded via `capeInitialize()`.

---

## 15. Key Design Points

1. **Strong Typing**: All interface methods use clearly defined data types, avoiding the ambiguity of COM VARIANTs
2. **Caller-Allocated Memory**: Container type allocation responsibility lies with the caller, simplifying memory management
3. **Header-Only Pure C++ Template Implementation**: No runtime library dependencies, compiles and works immediately
4. **Shared VTable**: VTables of the same type can be statically shared, reducing memory footprint