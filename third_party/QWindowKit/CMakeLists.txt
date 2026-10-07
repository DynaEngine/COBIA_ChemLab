# QWindowKit - Static library built from sources
# Sources: https://gitee.com/zoc_zoc/qwindowkit

set(CMAKE_AUTOMOC ON)

set(QWK_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(QWK_CORE_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/core)
set(QWK_WIDGETS_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/widgets)
set(QWK_FRAME_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/widgetframe)

# --- Core sources (only Windows-relevant) ---
set(QWK_CORE_SOURCES
    ${QWK_CORE_SRC_DIR}/qwkglobal.cpp
    ${QWK_CORE_SRC_DIR}/qwindowkit_windows.cpp
    ${QWK_CORE_SRC_DIR}/windowagentbase.cpp
    ${QWK_CORE_SRC_DIR}/windowitemdelegate.cpp
    ${QWK_CORE_SRC_DIR}/contexts/abstractwindowcontext.cpp
    ${QWK_CORE_SRC_DIR}/contexts/qtwindowcontext.cpp
    ${QWK_CORE_SRC_DIR}/contexts/win32windowcontext.cpp
    ${QWK_CORE_SRC_DIR}/kernel/nativeeventfilter.cpp
    ${QWK_CORE_SRC_DIR}/kernel/sharedeventfilter.cpp
    ${QWK_CORE_SRC_DIR}/kernel/winidchangeeventfilter.cpp
)

# --- Widgets sources (only Windows-relevant) ---
set(QWK_WIDGETS_SOURCES
    ${QWK_WIDGETS_SRC_DIR}/widgetwindowagent.cpp
    ${QWK_WIDGETS_SRC_DIR}/widgetwindowagent_win.cpp
    ${QWK_WIDGETS_SRC_DIR}/widgetitemdelegate.cpp
)

# --- WidgetFrame sources (WindowBar + WindowButton) ---
set(QWK_WIDGETFRAME_SOURCES
    ${QWK_FRAME_SRC_DIR}/windowbar.cpp
    ${QWK_FRAME_SRC_DIR}/windowbutton.cpp
)

# Build as static library
add_library(QWindowKit STATIC
    ${QWK_CORE_SOURCES}
    ${QWK_WIDGETS_SOURCES}
    ${QWK_WIDGETFRAME_SOURCES}
)

target_include_directories(QWindowKit PUBLIC
    ${QWK_INCLUDE_DIR}
    ${QWK_CORE_SRC_DIR}
    ${QWK_CORE_SRC_DIR}/contexts
    ${QWK_CORE_SRC_DIR}/kernel
    ${QWK_CORE_SRC_DIR}/shared
    ${QWK_WIDGETS_SRC_DIR}
    ${QWK_FRAME_SRC_DIR}
)

target_link_libraries(QWindowKit PUBLIC
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

# Qt private headers (needed for internal QWK includes)
target_include_directories(QWindowKit PRIVATE
    "${Qt6_DIR}/../../../include/QtCore/${Qt6_VERSION}"
    "${Qt6_DIR}/../../../include/QtCore/${Qt6_VERSION}/QtCore"
    "${Qt6_DIR}/../../../include/QtGui/${Qt6_VERSION}"
    "${Qt6_DIR}/../../../include/QtGui/${Qt6_VERSION}/QtGui"
)

target_compile_definitions(QWindowKit PUBLIC
    QWK_CORE_STATIC
    QWK_WIDGETS_STATIC
)

target_compile_features(QWindowKit PRIVATE cxx_std_17)

set_target_properties(QWindowKit PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED TRUE
)

# Suppress warnings for third-party code
if(MSVC)
    target_compile_options(QWindowKit PRIVATE /W0)
else()
    target_compile_options(QWindowKit PRIVATE -w)
endif()