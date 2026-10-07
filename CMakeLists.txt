cmake_minimum_required(VERSION 3.19)
project(ChemLab VERSION 0.0.1 LANGUAGES CXX)

# ============================================================================
# Auto-detect COBIA SDK
# ============================================================================
find_path(COBIA_INCLUDE_DIR
    NAMES COBIA.h
    PATHS
        "C:/Program Files (x86)/Common Files/CAPE-OPEN Laboratories Network/Include"
        "C:/Program Files/Common Files/CAPE-OPEN Laboratories Network/Include"
    DOC "COBIA SDK include directory"
)

find_program(COBIA_REGISTER
    NAMES cobiaRegister.exe
    PATHS
        "C:/Program Files/Common Files/CAPE-OPEN Laboratories Network/COBIA"
        "C:/Program Files (x86)/Common Files/CAPE-OPEN Laboratories Network/COBIA"
    DOC "COBIA registration tool"
)

if(NOT COBIA_INCLUDE_DIR)
    message(FATAL_ERROR "COBIA SDK not found. Set COBIA_INCLUDE_DIR manually.")
endif()
if(NOT COBIA_REGISTER)
    message(WARNING "cobiaRegister.exe not found. Register/unregister targets disabled.")
endif()

add_subdirectory(ChemProp)
add_subdirectory(ChemUnit)
add_subdirectory(ChemEngine)

set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/install/ChemLab" CACHE PATH
        "Install prefix" FORCE)

option(BUILD_GUI "Build ChemLab Qt6 GUI application" ON)

if(BUILD_GUI)
    set(CMAKE_PREFIX_PATH "D:/program/QT6/6.9.1/mingw_64" CACHE PATH "Qt6 install prefix")
    find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)
    add_subdirectory(third_party/QWindowKit)
    add_subdirectory(ChemLab)
endif()

set(WIN32_EXTRA_LIBS
    advapi32
    shell32
    shlwapi
    user32
    ole32
    oleaut32
    uuid
)

set(MINGW_EXTRA_LINK_OPTIONS
    -Wl,--enable-stdcall-fixup
    -static-libgcc
    -static-libstdc++
    -Wl,-Bstatic,--whole-archive
    -lwinpthread
    -Wl,--no-whole-archive,-Bdynamic
)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)