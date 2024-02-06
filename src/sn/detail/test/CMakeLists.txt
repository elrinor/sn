cmake_minimum_required(VERSION 3.24 FATAL_ERROR)

set(SN_DETAIL_TEST_SOURCES
    boolean_test_suite.h
    test_suite.h
    integer_test_suite.h)

add_library(sn_detail_test INTERFACE ${SN_DETAIL_TEST_SOURCES})
target_link_libraries(sn_detail_test INTERFACE sn_string GTest::gtest)
lint_target(sn_detail_test)
