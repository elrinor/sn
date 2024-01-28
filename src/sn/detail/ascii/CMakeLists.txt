cmake_minimum_required(VERSION 3.24 FATAL_ERROR)

set(SN_ASCII_SOURCES
    ascii_functions.h)

add_library(sn_ascii INTERFACE ${SN_ASCII_SOURCES})
add_library(sn::ascii ALIAS sn_ascii)
lint_target(sn_ascii)

if(SN_BUILD_TESTS)
    set(TEST_SN_ASCII_SOURCES
        ascii_functions_ut.cpp)

    add_executable(test_sn_ascii ${TEST_SN_ASCII_SOURCES})
    target_link_libraries(test_sn_ascii PUBLIC GTest::gtest GTest::gtest_main)
    lint_target(test_sn_ascii)

    add_custom_target(run_test_sn_ascii
        test_sn_ascii
        DEPENDS test_sn_ascii
        USES_TERMINAL)
    add_dependencies(run_all_tests run_test_sn_ascii)
endif()
