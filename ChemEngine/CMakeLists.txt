add_library(ChemEngine STATIC
    src/Compound.cpp
    src/SimulationObject.cpp
    src/UnitConverter.cpp
    src/Flowsheet.cpp
    src/FlowsheetSolver.cpp
    src/CapeMaterialStream.cpp
    src/CapeMaterialObject.cpp
    src/CapeUnitWrapper.cpp
    src/PropertyPackageManager.cpp
    src/UnitOperationManager.cpp
    src/BuiltInUnits.cpp
    src/PropertyPackage.cpp
    src/UnitOperationBase.cpp
)

target_include_directories(ChemEngine PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${COBIA_INCLUDE_DIR}
)