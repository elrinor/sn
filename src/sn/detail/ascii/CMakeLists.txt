cmake_minimum_required(VERSION 3.24 FATAL_ERROR)

set(SN_DETAIL_ASCII_SOURCES
    ascii_functions.h)

add_library(sn_detail_ascii INTERFACE ${SN_DETAIL_ASCII_SOURCES})
lint_target(sn_detail_ascii)

if(SN_BUILD_TESTS)
    set(TEST_SN_DETAIL_ASCII_SOURCES
        ascii_functions_ut.cpp)

    add_executable(test_sn_detail_ascii ${TEST_SN_DETAIL_ASCII_SOURCES})
    target_link_libraries(test_sn_detail_ascii PUBLIC sn_detail_ascii GTest::gtest GTest::gtest_main)
    lint_target(test_sn_detail_ascii)

    add_custom_target(run_test_sn_detail_ascii
        test_sn_detail_ascii
        DEPENDS test_sn_detail_ascii
        USES_TERMINAL)
    add_dependencies(run_all_tests run_test_sn_detail_ascii)
endif()
