cmake_minimum_required(VERSION 3.19)
project(ChemRouting VERSION 0.1.0 LANGUAGES C)

# ============================================================================
# 库目标 — 纯 C，无任何外部依赖
# ============================================================================

add_library(ChemRouting STATIC
    src/chemrouting.c
    src/routing_orthogonal.c
    src/routing_midpoint.c
    src/crossing.c
)

target_include_directories(ChemRouting PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(ChemRouting PUBLIC c_std_11)

if(MSVC)
    target_compile_options(ChemRouting PRIVATE /W4)
else()
    target_compile_options(ChemRouting PRIVATE -Wall -Wextra -pedantic)
endif()

# ============================================================================
# 测试目标
# ============================================================================

option(CHEMROUTING_BUILD_TESTS "Build ChemRouting tests" ON)

if(CHEMROUTING_BUILD_TESTS)
    enable_testing()

    add_executable(test_chemrouting
        test/test_chemrouting.c
    )

    target_link_libraries(test_chemrouting PRIVATE ChemRouting)

    add_test(NAME test_chemrouting COMMAND test_chemrouting)
endif()