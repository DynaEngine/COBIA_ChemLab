# ChemLab Road Map

## Project Vision

ChemLab — A modern chemical process simulation platform based on COBIA (CAPE-OPEN Binary Interop Architecture), cross-platform (Windows / Linux / macOS), implemented in C++/Qt.

---

## Architecture Overview

```
┌──────────────────────────────────────────────────────────────┐
│                    ChemLab GUI (Qt 6)                         │
│  ┌──────────┬──────────────┬──────────────┬───────────────┐  │
│  │ Ribbon UI │ Flowsheet   │ Property     │ Data Charts   │  │
│  │           │ Canvas      │ Grid         │ & Tables      │  │
│  └──────────┴──────────────┴──────────────┴───────────────┘  │
├──────────────────────────────────────────────────────────────┤
│              ChemEngine Core (C++)                            │
│  ┌──────────┬──────────────┬──────────────┬───────────────┐  │
│  │ Flowsheet │ Solver       │ COBIA        │ Unit Ops      │  │
│  │ Manager   │(Topological) │ Wrappers     │ Registry      │  │
│  └──────────┴──────────────┴──────────────┴───────────────┘  │
├──────────────────────────────────────────────────────────────┤
│     COBIA Modules                   COBIA Property Packages   │
│  ┌──────────────────┐     ┌──────────────────────────────┐   │
│  │ ChemUnit/        │     │ ChemProp/                    │   │
│  │   MHExch ✓       │     │   IdealGasPP ✓, WaterPP ✓   │   │
│  └──────────────────┘     └──────────────────────────────┘   │
└──────────────────────────────────────────────────────────────┘
```

> ✓ = Implemented

---

## Development Principles

1. **Engine + GUI parallel development**: Each feature is implemented in ChemEngine first, then immediately integrated into the ChemLab GUI for debugging and validation through the interface.
2. **Testable deliverables**: At the end of each Phase, the GUI must be able to load the Engine for a complete functional demonstration.
3. **Skeleton first, flesh later**: Establish end-to-end connectivity first, then progressively enrich feature details.

---

## Phase 1 — Skeleton (Current)

> **Goal**: GUI can load Engine, create flowsheets, run solver, and view results.

### 1.1 Engine Core ✓/WIP

| Module | Files | Status |
|---|---|---|
| Flowsheet Manager | `ChemEngine/Flowsheet/Flowsheet.h/.cpp` | ✓ Implemented |
| Topological Solver | `ChemEngine/Solver/FlowsheetSolver.h/.cpp` | ✓ Implemented |
| COBIA Material Stream Wrapper | `ChemEngine/COBIA/CapeMaterialStream.h/.cpp` | ✓ Implemented |
| COBIA Unit Operation Wrapper | `ChemEngine/COBIA/CapeUnitWrapper.h/.cpp` | ✓ Implemented |
| Property Package Manager | `ChemEngine/COBIA/PropertyPackageManager.h/.cpp` | ✓ Implemented |
| Compound Constant Properties | `ChemEngine/Base/CompoundConstantProperties.h` | ✓ Implemented |
| Unit Converter | `ChemEngine/Utils/UnitConverter.h/.cpp` | ✓ Implemented |

### 1.2 COBIA Modules ✓

| Module | Project | Status |
|---|---|---|
| Ideal Gas Property Package | `ChemProp/IdealGasPP` | ✓ Implemented |
| Water IAPWS-97 Property Package | `ChemProp/WaterPP` | ✓ Implemented |
| Heat Exchanger Unit Operation | `ChemUnit/MHExch` | ✓ Implemented |

### 1.3 GUI Foundation ✓

| Feature | Status |
|---|---|
| Main window framework (QMainWindow + Ribbon Bar) | ✓ Implemented |
| Dark theme Ribbon bar | ✓ Implemented |
| Dockable navigation sidebar (QDockWidget) | ✓ Implemented |
| Tabbed MDI document area (QMdiArea) | ✓ Implemented |
| File menu (New/Open/Save/Exit) | ✓ Implemented |

### 1.4 GUI ↔ Engine Integration (Current Focus)

- [x] **Engine loading & initialization**: Load ChemEngine on startup, SimulationManager bridge
- [x] **Property package selection panel**: Select/switch property packages in GUI, Engine returns compound list
- [x] **Compound selector**: Select compounds from property packages, set feed composition (with built-in sample database fallback)
- [x] **Flowsheet tree/list view**: Display Flowsheet object tree in navigation panel (Streams / Unit Ops)
- [x] **Solver control panel**: Set tolerance, max iterations, trigger solve, show progress
- [x] **Solver results panel**: Material stream result tables (T, P, Flow, Composition, Properties), unit operation status
- [x] **Log/diagnostics output window**: Real-time Engine solver logs streamed to GUI
- [x] **Unit operation panel**: Auto-discover all Unit Operations from registry/COBIA, filter by type
- [x] **All panels support docking**: QDockWidget + tabifyDockWidget layout, freely drag, float, close
- [x] **Module categorization view**: Auto-group by Vendor/Creator, collapsible tree view (QTreeView)

> **Module categorization design**: Both UnitOperationInfo and PropertyPackageInfo include a `vendor` field.
> - Level 1 grouping: Type (COBIA / CAPE-OPEN 1.1 / CAPE-OPEN 1.0)
> - Level 2 grouping: Vendor (e.g. "AmsterCHEM", "ChemSep", "ChemEngine")
> - Level 3: Specific module name, leaf nodes draggable to canvas
> - Unrecognized vendors grouped under "Unknown / Standalone"
> - Flat QListWidget mode preserved in list view, toggleable via ToolBar

- [x] **Simple flowsheet demo**: Mixer + Heater + Flash three-unit flowsheet, end-to-end runnable (QuickDemo)

### 1.5 UI Polish & Branding

> **Goal**: Elevate ChemLab's visual quality and professional feel.

- [x] **SVG Logo & Icon System**
  - Design ChemLab brand Logo (flask/molecular structure theme)
  - Application icon (.ico) multi-size embedding
  - Complete SVG toolbar icon set (New/Open/Save/Run/Stop etc.) — basic set implemented
  - Unit operation equipment icons (Mixer, Heater, Flash, Pump, Valve, etc.)
  - Property package / compound / stream type icons
  - SVG icon coloring scheme (dark theme adapted)
- [x] **Global Color Scheme Optimization**
  - Define ChemLab brand palette (primary/secondary/accent/warning/success)
  - QSS (Qt Style Sheets) global stylesheet — dark theme implemented
  - Full dark theme coverage (Dock panels, tables, trees, buttons, scrollbars)
  - Light theme as an alternative option
  - High-contrast mode support (accessibility)
- [ ] **Typography System**
  - Monospace programming font (Consolas / Cascadia Code): logs, code views
  - UI font (Segoe UI / Microsoft YaHei): panels, menus
  - Heading font size hierarchy specification
- [ ] **Panel Visual Enhancements**
  - Dock panel title bar styling (gradient background, icons)
  - Inter-panel splitter styling
  - Tree widget row height, indentation, selection highlight
  - Table alternating row colors, hover effects
  - Empty panel placeholder hints ("No Property Package loaded" guidance text + icon)
- [ ] **Status Bar Enhancements**
  - Solver status indicator lights (● Idle / ● Running / ● Done / ● Failed)
  - Current property package name display
  - Iteration count / residual real-time display
- [ ] **Splash Screen**
  - ChemLab Logo centered
  - Loading progress bar (Engine init, module registration, GUI ready)
  - Version number display
- [ ] **Ribbon Bar Polish**
  - Custom Ribbon color scheme (harmonized with dark theme)
  - Large icon mode (Icon + Text) / small icon mode toggle
  - Quick Access Toolbar (QAT) customization
- [ ] **Animations & Transitions**
  - Dock panel expand/collapse animation
  - Tab switch transition
  - Solver progress pulse animation
  - Notification/toast fade in/out

### 1.6 Simple Flowsheet Demo

> **Goal**: Mixer + Heater + Flash three-unit flowsheet end-to-end, validating full-chain integration.

- [x] **Flowsheet creation wizard**: Select unit operations from Unit Operation Panel, drag onto flowsheet — QuickDemo button for one-click creation
- [x] **Stream auto-connection**: connectObjects builds Feed1+Feed2→Mixer→Heater→Flash→Vapor/Liquid topology
- [x] **Solve dispatch**: Solver topological sort → solve() per object, convergence criterion: all objects Calculate
- [x] **BuiltIn unit operations**: BuiltInMixer (mixing) / BuiltInHeater (heating) / BuiltInFlash (flash) built-in implementations
- [x] **Result comparison**: FlowsheetPanel tree shows ✓/✖ status, ResultsPanel shows property parameters

### 1.7 Property Research Panel

> **Goal**: Standalone property analysis tool panel for pure component/mixture property calculation and curve plotting.
> **Core philosophy**: Users can directly select any registered property package in the panel without setting it as the global property package; supports simultaneous loading of multiple property packages with result comparison.

- [x] **Panel Architecture**
  - `PropertyResearchPanel` QWidget + QDockWidget docking
  - Tab-stacked with "Solver" panel in the right area
- [x] **Compound Selection**
  - Select 1~N compounds from loaded property packages (Checkbox tree list)
  - Support mass/mole fraction input, auto-normalize totals
- [x] **Property Calculation**
  - Fixed T/P calculation: density, enthalpy, entropy, heat of vaporization, surface tension, viscosity
  - Real-time result table display (QTableWidget: Property / Phase / Value)
- [x] **Curve Plotting (Qwt)**
  - **Pure component saturation line**: P-T plot (lnP vs 1/T coordinates), based on Antoine-type correlation approximation
  - **Property-temperature curve**: Cp vs T at fixed P
  - Dark theme adapted, grid lines
- [x] **Data Export**
  - Export calculation results to CSV
- [x] **Property Package Selector**
  - Dropdown or tree list enumerating all registered property packages (COBIA + COM enumeration)
  - Property packages auto-grouped by vendor/type (IdealGasPP, WaterPP, PR-PP, SRK-PP, NRTL-PP, etc.)
  - Display property package metadata: name, vendor, CAPE-OPEN version, available compound count
  - Support independent loading of property package instances (loaded PPs do not occupy global PME settings, used only by the research panel)
  - **Multi-package simultaneous loading in panel**: Independently select and instantiate multiple PPs coexisting in the panel
  - Auto-update available compound list when switching property packages
  - Empty state hint when no PP loaded: "No Property Package selected — choose one to begin"
- [x] **Multi-Package Comparison View**
  - **Comparison table**: Same property, same T/P calculation results displayed side-by-side for multiple selected PPs
    - Header: Property | Phase | [PP1 Value] | [PP2 Value] | ... | Δ(Max-Min) | Rel.Dev%
    - Significant deviation row highlighting (e.g. > 5% yellow, > 20% red)
  - **Curve overlay**: Multiple PPs overlaid on same axes
    - Different color/line style per PP (solid/dashed/dotted), legend auto-generated
    - Supports: saturation line overlay, property-T curve overlay, property-P curve overlay
  - **Deviation plot**: Based on Reference PP, plot relative deviation curves for each PP (Deviation vs T/P)
  - **Comparison report export**: Export multi-package comparison table + charts as CSV / PNG
- [x] **Smart Property Package Discovery**
  - Auto-scan from COBIA Registry: `categoryId_PropertyPackageManager` and `categoryId_StandAlonePropertyPackage`
  - Scan traditional CAPE-OPEN 1.0/1.1 PPs from COM Registry
  - Property package cache: cache results after first scan, support manual refresh
- [x] **Calculation Mode Extensions**
  - **Fixed T/P mode**: Currently implemented
  - **Fixed P/H mode**: Given pressure and enthalpy, flash calculate phase compositions and properties
  - **Fixed T/ρ mode**: Given temperature and density, calculate pressure and other properties
  - **Pure component temperature sweep**: Specify T range [Tmin, Tmax] and step, auto-generate property-temperature table
  - **Mixture phase equilibrium curves**: Given composition, calculate bubble/dew point curves (T-xy, P-xy)
- [ ] **Property Package Health Diagnostics**
  - Detect calculation result anomalies (NaN/Inf/out-of-bounds) across PPs under identical conditions
  - Alert for potential PP version or parameter configuration issues

---

## Phase 2 — Unit Operation Expansion

> **Goal**: Engine supports a complete basic unit operation library, GUI can build moderately complex flowsheets.

### 2.1 Engine — New Unit Operations (ChemUnit)

- [ ] **Mixer / Splitter** — Mixer / Splitter
- [ ] **Pump / Compressor / Expander** — Pump / Compressor / Expander
- [ ] **Flash Drum** — Flash drum (isothermal / adiabatic / fixed vapor fraction)
- [ ] **Valve** — Valve (adiabatic throttling)
- [ ] **Heat Exchanger** — Simple heat exchanger (spec T or Q)

### 2.2 Engine — Solver Enhancements

- [ ] Tear stream auto-detection and convergence (Wegstein / Broyden)
- [ ] Multi-level convergence tolerance control
- [ ] Solve failure diagnostics (object/variable with maximum residual)
- [ ] Flowsheet cycle detection and reporting

### 2.3 Engine — Property Package Expansion (ChemProp)

- [ ] **Peng-Robinson PP** — PR equation of state property package
- [ ] **SRK PP** — Soave-Redlich-Kwong property package
- [ ] **NRTL PP** — Activity coefficient property package (liquid phase)
- [ ] Pure component database expansion (≥ 100 common compounds)

### 2.4 GUI — Flowsheet Editing

- [ ] **Flowsheet object property editor (Property Grid)**: Select stream/equipment, show editable properties on the right
- [x] **Unit operation panel**: Categorized tree expansion by Vendor/type, drag-and-drop unit operations to flowsheet canvas
- [x] **Stream connection editor**: Select source/destination ports for connections (via canvas port-to-port)
- [x] **Flowsheet tree interaction**: Double-click navigation tree node to open corresponding editor
- [x] **Property package category view**: PPs auto-grouped by vendor/type, collapsible tree list

### 2.5 GUI — Results Display

- [x] **Stream result table**: Multi-stream side-by-side comparison view (ResultsPanel)
- [x] **Property table view**: Select stream to show all properties (density, enthalpy, entropy, viscosity, etc.)
- [x] **Property research panel**: Independent property analysis → PP selector → multi-package comparison → curve overlay → deviation plot → data export (continuing Phase 1.7)

---

## Phase 3 — Flowsheet Canvas & Visualization

> **Goal**: PFD graphical editing + data chart display.

### 3.1 GUI — Flowsheet Canvas

- [x] **PFD Canvas** (QGraphicsView): Drag-and-drop equipment icons, connect streams, zoom/pan
- [x] **Equipment icon library**: One icon per unit operation type (SVG icons)
- [x] **Stream routing**: Orthogonal lines via StreamItem / StreamBlockItem, port-to-port connections
- [x] **Canvas ↔ Engine bidirectional sync**: Add equipment on canvas → Engine creates object; Engine solve results → canvas updates annotations
- [ ] **Grid snapping / alignment**
- [ ] **Undo / Redo**
- [ ] **Layer management** (equipment layer, stream layer, annotation layer)

### 3.2 GUI — Data Visualization

- [x] **Chart components** (Qwt): Line charts, scatter plots — built in Phase 1.7 Property Research Panel
- [ ] **Phase diagrams**: T-xy / P-xy / ternary phase diagrams (Phase 1.7 Property Research Panel pilots this)
- [x] **Property curves**: Saturation lines, isotherms, Cp/H/S-temperature curves (via Property Research Panel)
- [ ] **Column profile plots**: Temperature/composition distribution along column stages
- [ ] **Report export**: PDF / HTML formats

---

## Phase 4 — Separation & Reactors

> **Goal**: Engine supports distillation columns and reactors, covering complete chemical process scenarios.

### 4.1 Engine — Separation Units (ChemUnit)

- [ ] **Distillation Column** — Shortcut method (FUG: Fenske-Underwood-Gilliland)
- [ ] **Distillation Column** — Rigorous method (Inside-Out / Newton)
- [ ] **Absorber / Stripper** — Absorber / Stripper
- [ ] **Liquid-Liquid Extractor** — Extraction column
- [ ] **3-Phase Separator** — Three-phase separator

### 4.2 Engine — Reactors (ChemUnit)

- [ ] **Conversion Reactor** — Conversion reactor
- [ ] **Equilibrium Reactor** — Gibbs free energy minimization
- [ ] **CSTR** — Continuous stirred-tank reactor (kinetics)
- [ ] **PFR** — Plug flow reactor

### 4.3 Engine — Property Enhancements

- [ ] Reaction component thermochemical database (enthalpy of formation, Gibbs free energy)
- [ ] Oil characterization
- [ ] UNIFAC group contribution method (estimate activity coefficients without experimental data)

### 4.4 GUI — Column & Reactor Editors

- [ ] **Distillation column configuration wizard**: Number of stages, feed location, reflux ratio, pressure profile
- [ ] **Column internal results view**: Temperature/composition/flow rate profile plots
- [ ] **Reactor configuration panel**: Reaction equation editing, kinetic parameter input
- [ ] **Convergence monitoring**: Column/reactor iteration process visualization

---

## Phase 5 — Dynamic Simulation & Control

> **Goal**: Engine supports dynamic mode, GUI provides real-time trends and control panels.

### 5.1 Engine — Dynamic Engine

- [ ] Dynamic mode manager (steady-state ↔ dynamic switching)
- [ ] ODE/DAE solver (explicit Euler / implicit BDF)
- [ ] Dynamic model library: tanks, heat exchangers, CSTR, distillation columns
- [ ] Pressure-Flow solver
- [ ] Adaptive step size control
- [ ] Snapshot / checkpoint save & restore

### 5.2 Engine — Process Control

- [ ] **PID Controller**
- [ ] On-Off controller
- [ ] Cascade control (Cascade PID)
- [ ] Feedforward control + feedback
- [ ] Control valve model

### 5.3 GUI — Dynamic Panels

- [ ] **Real-time trend plot**: Real-time curves of key variables over time
- [ ] **PID panel**: SP/PV/OP numeric display + parameter adjustment
- [ ] **Dynamic flowsheet replay**: Time-based flowsheet state playback

---

## Phase 6 — Optimization & Tools

> **Goal**: Sensitivity analysis, parameter optimization, economic evaluation.

### 6.1 Engine — Optimizer

- [ ] Sensitivity analysis engine
- [ ] Constrained optimizer (SQP / PSO)
- [ ] Parameter estimation / data regression
- [ ] Case studies (multi-case batch runs)

### 6.2 Engine — Economic Analysis

- [ ] Equipment cost estimation (CAPEX) — based on Guthrie / Turton correlations
- [ ] Operating cost calculation (OPEX) — utilities, raw materials
- [ ] Economic indicators (NPV, IRR, Payback)

### 6.3 GUI — Tool Panels

- [ ] **Sensitivity / Optimization configuration panel**: Independent variables, objective function, constraints
- [ ] **Result comparison table**: Multi-case side-by-side comparison
- [ ] **Economic analysis report**: Investment/operating cost summary

---

## Phase 7 — Extensibility & Release

> **Goal**: Plugin system, third-party interoperability, cross-platform release.

### 7.1 Extensibility

- [ ] **Plugin interface**: Dynamically load custom unit operations / property packages
- [ ] **Python scripting engine** (pybind11): User-written custom scripts
- [ ] DWSIM XML / dwxmz format import
- [ ] CAPE-OPEN 1.1 third-party component compatibility

### 7.2 Engineering

- [ ] Unit tests (Google Test + Qt Test)
- [ ] Property calculation benchmark validation
- [ ] Dark / light theme switching
- [ ] Multi-language support (i18n)
- [ ] Auto-update mechanism
- [ ] CI/CD (GitHub Actions)

### 7.3 Documentation

- [ ] User manual + example tutorials
- [ ] Developer documentation + API documentation (Doxygen)
- [ ] Built-in help system (QHelpEngine)

### 7.4 Release

| Platform | Format |
|---|---|
| Windows (x64) | MSI / Portable |
| Linux (x64) | AppImage / Flatpak / deb / rpm |
| macOS | .dmg |

---

## Milestone Summary

| Phase | Focus | Key Deliverable |
|---|---|---|
| **1** *(current)* | Skeleton | GUI loads Engine → create flowsheet → solve → view results |
| **1.5** | UI Polish | SVG Logo / Icon system / Color scheme / Typography / Module category view *(in progress)* |
| **1.6** | Flowsheet Demo | Mixer + Heater + Flash three-unit flowsheet end-to-end |
| **1.7** | Property Research | Property calculation panel + curve plotting (T-xy, P-xy, saturation line, property-T) |
| **2** | Unit Op Expansion | Full basic unit operation library + property editor + result tables *(unit op panel/PP views/results ✓)* |
| **3** | PFD Canvas | Graphical flowsheet editing + data charts *(canvas core ✓, grid/undo/layers pending)* |
| **4** | Separation & Reactors | Rigorous distillation + reactors + column profile plots |
| **5** | Dynamics & Control | Dynamic mode + PID + real-time trend plots |
| **6** | Optimization & Tools | Sensitivity + economic evaluation + multi-case comparison |
| **7** | Extensibility & Release | Plugin system + cross-platform packaging + documentation |

---

## Development Environment Setup

> All commands below are executed from the project root directory.

### CMake Presets (Recommended)

The project provides `CMakePresets.json` with preset Ninja + MinGW-w64 build configurations:

```powershell
# Configure (defaults to qt6-minsizerel)
cmake --preset qt6-minsizerel

# Build ChemLab GUI
cmake --build --preset qt6-minsizerel --target ChemLab -j8

# Also available: debug / release configurations
cmake --preset qt6-debug
cmake --build --preset qt6-debug --target ChemLab -j8

cmake --preset qt6-release
cmake --build --preset qt6-release --target ChemLab -j8
```

| Preset | Compiler | Build System | Build Type |
|---|---|---|---|
| `qt6-minsizerel` *(default)* | MinGW 13.1.0 (Qt 6.9.1) | Ninja | MinSizeRel |
| `qt6-debug` | MinGW 13.1.0 (Qt 6.9.1) | Ninja | Debug |
| `qt6-release` | MinGW 13.1.0 (Qt 6.9.1) | Ninja | Release |

### Manual CMake Build

```powershell
# Using MinGW Makefiles
mkdir build && cd build
cmake .. -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug
cmake --build . --target ChemLab -j8
```

### Output Locations

| Configuration | Path |
|---|---|
| `qt6-minsizerel` | `build/qt6-minsizerel/ChemLab/ChemLab.exe` |
| `qt6-debug` | `build/qt6-debug/ChemLab/ChemLab.exe` |
| `qt6-release` | `build/qt6-release/ChemLab/ChemLab.exe` |
| Manual build | `build/ChemLab/ChemLab.exe` |

---

## Key Dependencies

| Module | Technology Stack |
|---|---|
| GUI | Qt 6.5+ (Widgets, Svg, Charts), QWindowKit |
| Icons | SVG (Qt SVG module), custom vector icon library |
| Build | CMake 3.20+, Ninja / MSVC 2022 / GCC 12+ / Clang 16+ |
| Presets | `CMakePresets.json` (qt6-minsizerel / qt6-debug / qt6-release) |
| COBIA | COBIA SDK (C++), CAPE-OPEN 1.2 interfaces |
| Thermo | IAPWS-97 (Water), PR/SRK/NRTL (in-house), CoolProp (optional backend) |
| Math/LA | Eigen 3.4+, NLopt |
| Scripting | pybind11 (Python), LuaJIT |
| Testing | Google Test, Qt Test |
| Docs | Doxygen, Sphinx |
| CI/CD | GitHub Actions |
| Packaging | CPack, Wix, AppImage, Flatpak |