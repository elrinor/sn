cmake_minimum_required(VERSION 3.24 FATAL_ERROR)

if(SN_QT_SUPPORT)
    set(SN_QJSON_SOURCES
        qjson.h
        qjson_concepts.h
        qjson_fwd.h
        detail/qjson_builtins.h
        detail/qjson_builtins.cpp
        detail/qjson_exceptions.h
        detail/qjson_exceptions.cpp)

    add_library(sn_qjson STATIC ${SN_QJSON_SOURCES})
    add_library(sn::qjson ALIAS sn_qjson)
    target_link_libraries(sn_qjson PUBLIC sn_core sn_qstring sn_reflection Qt::Core)
    lint_target(sn_qjson)

    if(SN_BUILD_TESTS)
        set(TEST_SN_QJSON_SOURCES
            test/qjson_ut.cpp)

        add_executable(test_sn_qjson ${TEST_SN_QJSON_SOURCES})
        target_link_libraries(test_sn_qjson PUBLIC sn_qjson sn_detail_test GTest::gtest GTest::gtest_main)
        lint_target(test_sn_qjson)

        add_custom_target(run_test_sn_qjson
            test_sn_qjson
            DEPENDS test_sn_qjson
            USES_TERMINAL)
        add_dependencies(run_all_tests run_test_sn_qjson)
    endif()
endif()
