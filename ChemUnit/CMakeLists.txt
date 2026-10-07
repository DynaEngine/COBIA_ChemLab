cmake_minimum_required(VERSION 3.19)
project(MHExch VERSION 0.5.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ============================================================================
# Target: MHExch - COBIA Multi-Stream Heat Exchanger Unit Operation
# ============================================================================
add_library(MHExch SHARED
    MHExch/MHExch.cpp
)

target_include_directories(MHExch PRIVATE
    ${COBIA_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/MHExch
)

target_compile_definitions(MHExch PRIVATE
    COBIA_NOAUTOLINK
)

if(WIN32)
    target_link_libraries(MHExch PRIVATE
        ${WIN32_EXTRA_LIBS}
    )
endif()

if(MINGW)
    target_link_options(MHExch PRIVATE
        ${MINGW_EXTRA_LINK_OPTIONS}
    )
endif()

set_target_properties(MHExch PROPERTIES
    PREFIX ""
    SUFFIX ".dll"
)

# ============================================================================
# Installation
# ============================================================================
set(MHEXCH_INSTALL_PATH "${CMAKE_INSTALL_PREFIX}/ChemUnit/MHExch.dll")

install(TARGETS MHExch
    RUNTIME DESTINATION ChemUnit
    LIBRARY DESTINATION ChemUnit
)

# ============================================================================
# Registration / Unregistration (via COBIA cobiaRegister.exe)
# ============================================================================
if(COBIA_REGISTER)
    add_custom_target(unregister_MHExch
        COMMAND "${COBIA_REGISTER}" -u "${MHEXCH_INSTALL_PATH}"
        COMMENT "Unregistering MHExch..."
    )

    add_custom_target(register_MHExch
        COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:MHExch>" "${MHEXCH_INSTALL_PATH}"
        COMMAND "${COBIA_REGISTER}" -a "${MHEXCH_INSTALL_PATH}"
        DEPENDS MHExch
        COMMENT "Installing and registering MHExch..."
    )

    add_custom_target(rebuild_MHExch
        COMMAND cmd /c "\"${COBIA_REGISTER}\" -u \"${MHEXCH_INSTALL_PATH}\" 2>nul & \"${CMAKE_COMMAND}\" --build \"${CMAKE_BINARY_DIR}\" --target register_MHExch"
        COMMENT "Rebuilding MHExch: unregister -> compile -> install -> register"
    )
endif()

# ============================================================================
# IDE folder organization
# ============================================================================
set_target_properties(MHExch PROPERTIES FOLDER "UnitOperations")