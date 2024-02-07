cmake_minimum_required(VERSION 3.24 FATAL_ERROR)

if(SN_QT_LIB)
    set(SN_QSTRING_SOURCES
        ascii_qstring_functions.h
        enum_qstring.h
        qstring.h
        qstring_builtins.cpp
        qstring_builtins.h
        qstring_concepts.h
        qstring_exceptions.cpp
        qstring_exceptions.h
        qstring_fwd.h
        std_qstring_enum_table.cpp
        std_qstring_enum_table.h)

    add_library(sn_qstring STATIC ${SN_QSTRING_SOURCES})
    add_library(sn::qstring ALIAS sn_qstring)
    target_link_libraries(sn_qstring PUBLIC sn_core sn_string sn_detail_ascii sn_detail_enum_table Qt::Core)
    lint_target(sn_qstring)

    if(SN_BUILD_TESTS)
        set(TEST_SN_QSTRING_SOURCES
            enum_qstring_ut.cpp
            qstring_ut.h
            qstring_ut.cpp)

        add_executable(test_sn_qstring ${TEST_SN_QSTRING_SOURCES})
        target_link_libraries(test_sn_qstring PUBLIC sn_qstring sn_detail_test GTest::gtest GTest::gtest_main)
        lint_target(test_sn_qstring)

        add_custom_target(run_test_sn_qstring
            test_sn_qstring
            DEPENDS test_sn_qstring
            USES_TERMINAL)
        add_dependencies(run_all_tests run_test_sn_qstring)
    endif()
endif()
