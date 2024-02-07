cmake_minimum_required(VERSION 3.24 FATAL_ERROR)

if(SN_QT_LIB)
    set(SN_QENUM_SOURCES
        ascii_qstring_functions.h
        enum_qstring.h
        std_qstring_enum_table.h
        std_qstring_enum_table.cpp)

    add_library(sn_qenum STATIC ${SN_QENUM_SOURCES})
    add_library(sn::qenum ALIAS sn_qenum)
    target_link_libraries(sn_qenum PUBLIC sn_string sn_detail_ascii sn_detail_enum_table Qt::Core)
    lint_target(sn_qenum)

    if(SN_BUILD_TESTS)
        set(TEST_SN_QENUM_SOURCES enum_qstring_ut.cpp)

        add_executable(test_sn_qenum ${TEST_SN_QENUM_SOURCES})
        target_link_libraries(test_sn_qenum PUBLIC sn_qenum GTest::gtest GTest::gtest_main)
        lint_target(test_sn_qenum)

        add_custom_target(run_test_sn_qenum
            test_sn_qenum
            DEPENDS test_sn_qenum
            USES_TERMINAL)
        add_dependencies(run_all_tests run_test_sn_qenum)
    endif()
endif()
