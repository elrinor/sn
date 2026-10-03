cmake_minimum_required(VERSION 3.24 FATAL_ERROR)

set(SN_RANGES_SOURCES
    clear.h
    insert.h)

add_library(sn_ranges INTERFACE ${SN_RANGES_SOURCES})
add_library(sn::ranges ALIAS sn_ranges)
target_link_libraries(sn_ranges INTERFACE sn_core)
lint_target(sn_ranges)

if(SN_BUILD_TESTS)
    set(TEST_SN_RANGES_SOURCES
        detail/ranges_ut.cpp)

    add_executable(test_sn_ranges ${TEST_SN_RANGES_SOURCES})
    target_link_libraries(test_sn_ranges PUBLIC sn_ranges GTest::gtest GTest::gtest_main)
    lint_target(test_sn_ranges)

    add_custom_target(run_test_sn_ranges
        test_sn_ranges
        DEPENDS test_sn_ranges
        USES_TERMINAL)
    add_dependencies(run_all_tests run_test_sn_ranges)
endif()
