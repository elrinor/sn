cmake_minimum_required(VERSION 3.24 FATAL_ERROR)

set(SN_DETAIL_ENUM_TABLE_SOURCES
    enum_string_maps.h
    enum_table.h
    enum_table_exceptions.h
    enum_table_exceptions.cpp
    enum_table_hash.h
    enum_type_traits.h
    perfect_hash.h
    universal_enum_table.h)

add_library(sn_detail_enum_table STATIC ${SN_DETAIL_ENUM_TABLE_SOURCES})
target_link_libraries(sn_detail_enum_table PUBLIC sn_core sn_string)
lint_target(sn_detail_enum_table)

if(SN_BUILD_TESTS)
    set(TEST_SN_DETAIL_ENUM_TABLE_SOURCES
        enum_table_hash_ut.cpp
        perfect_hash_ut.cpp)

    add_executable(test_sn_detail_enum_table ${TEST_SN_DETAIL_ENUM_TABLE_SOURCES})
    target_link_libraries(test_sn_detail_enum_table PUBLIC sn_detail_enum_table GTest::gtest GTest::gtest_main)
    lint_target(test_sn_detail_enum_table)

    add_custom_target(run_test_sn_detail_enum_table
        test_sn_detail_enum_table
        DEPENDS test_sn_detail_enum_table
        USES_TERMINAL)
    add_dependencies(run_all_tests run_test_sn_detail_enum_table)
endif()
