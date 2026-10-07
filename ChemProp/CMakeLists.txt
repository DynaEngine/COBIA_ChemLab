cmake_minimum_required(VERSION 3.19)
project(IdealGasPP VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ============================================================================
# Target: IdealGasPP - COBIA Ideal Gas Property Package
# ============================================================================
add_library(IdealGasPP SHARED
    IdealGasPP/IdealGasPP.cpp
    IdealGasPP/IdealGasPP.h
)

target_include_directories(IdealGasPP PRIVATE
    ${COBIA_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_definitions(IdealGasPP PRIVATE
    COBIA_NOAUTOLINK
)

if(WIN32)
    target_link_libraries(IdealGasPP PRIVATE
        ${WIN32_EXTRA_LIBS}
    )
endif()

if(MINGW)
    target_link_options(IdealGasPP PRIVATE
        ${MINGW_EXTRA_LINK_OPTIONS}
    )
endif()

set_target_properties(IdealGasPP PROPERTIES
    PREFIX ""
    SUFFIX ".dll"
)

# ============================================================================
# Target: WaterPP - COBIA Water IAPWS-97 Property Package
# ============================================================================
add_library(WaterPP SHARED
    WaterPP/WaterPP.cpp
    WaterPP/WaterPP.h
    WaterPP/Water.cpp
    WaterPP/Water.h
)

target_include_directories(WaterPP PRIVATE
    ${COBIA_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_definitions(WaterPP PRIVATE
    COBIA_NOAUTOLINK
)

if(WIN32)
    target_link_libraries(WaterPP PRIVATE
        ${WIN32_EXTRA_LIBS}
    )
endif()

if(MINGW)
    target_link_options(WaterPP PRIVATE
        ${MINGW_EXTRA_LINK_OPTIONS}
    )
endif()

set_target_properties(WaterPP PROPERTIES
    PREFIX ""
    SUFFIX ".dll"
)

# ============================================================================
# Installation
# ============================================================================
set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/install" CACHE PATH "Install prefix" FORCE)
set(IDEALGASPP_INSTALL_PATH "${CMAKE_INSTALL_PREFIX}/ChemProp/IdealGasPP.dll")
set(WATERPP_INSTALL_PATH "${CMAKE_INSTALL_PREFIX}/ChemProp/WaterPP.dll")

install(TARGETS IdealGasPP
    RUNTIME DESTINATION ChemProp
    LIBRARY DESTINATION ChemProp
)

install(TARGETS WaterPP
    RUNTIME DESTINATION ChemProp
    LIBRARY DESTINATION ChemProp
)

# ============================================================================
# Registration / Unregistration (via COBIA cobiaRegister.exe)
# ============================================================================
if(COBIA_REGISTER)
    add_custom_target(unregister
        COMMAND "${COBIA_REGISTER}" -u "${IDEALGASPP_INSTALL_PATH}"
        COMMAND "${COBIA_REGISTER}" -u "${WATERPP_INSTALL_PATH}"
        COMMENT "Unregistering IdealGasPP and WaterPP..."
    )

    add_custom_target(unregister_IdealGasPP
        COMMAND "${COBIA_REGISTER}" -u "${IDEALGASPP_INSTALL_PATH}"
        COMMENT "Unregistering IdealGasPP..."
    )

    add_custom_target(unregister_WaterPP
        COMMAND "${COBIA_REGISTER}" -u "${WATERPP_INSTALL_PATH}"
        COMMENT "Unregistering WaterPP..."
    )

    add_custom_target(register
        COMMAND "${CMAKE_COMMAND}" --install "${CMAKE_BINARY_DIR}" --config "$<CONFIG>"
        COMMAND "${COBIA_REGISTER}" -a "${IDEALGASPP_INSTALL_PATH}"
        COMMAND "${COBIA_REGISTER}" -a "${WATERPP_INSTALL_PATH}"
        DEPENDS IdealGasPP WaterPP
        COMMENT "Installing and registering IdealGasPP and WaterPP..."
    )

    add_custom_target(register_IdealGasPP
        COMMAND "${CMAKE_COMMAND}" --install "${CMAKE_BINARY_DIR}" --config "$<CONFIG>"
        COMMAND "${COBIA_REGISTER}" -a "${IDEALGASPP_INSTALL_PATH}"
        DEPENDS IdealGasPP
        COMMENT "Installing and registering IdealGasPP..."
    )

    add_custom_target(register_WaterPP
        COMMAND "${CMAKE_COMMAND}" --install "${CMAKE_BINARY_DIR}" --config "$<CONFIG>"
        COMMAND "${COBIA_REGISTER}" -a "${WATERPP_INSTALL_PATH}"
        DEPENDS WaterPP
        COMMENT "Installing and registering WaterPP..."
    )

    add_custom_target(rebuild
        COMMAND cmd /c "\"${COBIA_REGISTER}\" -u \"${IDEALGASPP_INSTALL_PATH}\" 2>nul & \"${COBIA_REGISTER}\" -u \"${WATERPP_INSTALL_PATH}\" 2>nul & \"${CMAKE_COMMAND}\" --build \"${CMAKE_BINARY_DIR}\" --target register"
        COMMENT "Rebuilding: unregister -> compile -> install -> register"
    )

    add_custom_target(rebuild_IdealGasPP
        COMMAND cmd /c "\"${COBIA_REGISTER}\" -u \"${IDEALGASPP_INSTALL_PATH}\" 2>nul & \"${CMAKE_COMMAND}\" --build \"${CMAKE_BINARY_DIR}\" --target register_IdealGasPP"
        COMMENT "Rebuilding IdealGasPP: unregister -> compile -> install -> register"
    )

    add_custom_target(rebuild_WaterPP
        COMMAND cmd /c "\"${COBIA_REGISTER}\" -u \"${WATERPP_INSTALL_PATH}\" 2>nul & \"${CMAKE_COMMAND}\" --build \"${CMAKE_BINARY_DIR}\" --target register_WaterPP"
        COMMENT "Rebuilding WaterPP: unregister -> compile -> install -> register"
    )
endif()

# ============================================================================
# IDE folder organization
# ============================================================================
set_target_properties(IdealGasPP PROPERTIES FOLDER "ComponentLibraries")
set_target_properties(WaterPP PROPERTIES FOLDER "ComponentLibraries")