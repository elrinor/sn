cmake_minimum_required(VERSION 3.24 FATAL_ERROR)

if(SN_QT_SUPPORT)
    set(SN_QBYTEARRAY_SOURCES
        qbytearray.h
        qbytearray_concepts.h
        qbytearray_fwd.h
        qbytearray_tags.h
        detail/qbytearray_builtins.cpp
        detail/qbytearray_builtins.h)

    add_library(sn_qbytearray STATIC ${SN_QBYTEARRAY_SOURCES})
    add_library(sn::qbytearray ALIAS sn_qbytearray)
    target_link_libraries(sn_qbytearray PUBLIC sn_core sn_string sn_reflection sn_detail_ascii sn_detail_enum_table sn_detail_codegen Qt::Core)
    lint_target(sn_qbytearray)

    add_generate_headers(sn_qbytearray_generate_headers
        DIR ${CMAKE_CURRENT_SOURCE_DIR}
        NAME "qbytearray"
        TYPE "QByteArray"
        VIEW "QByteArrayView"
        DECLS "class QByteArray;\nclass QByteArrayView;")

    if(SN_BUILD_TESTS)
        set(TEST_SN_QBYTEARRAY_SOURCES
            test/qbytearray_ut.h
            test/qbytearray_ut.cpp)

        add_executable(test_sn_qbytearray ${TEST_SN_QBYTEARRAY_SOURCES})
        target_link_libraries(test_sn_qbytearray PUBLIC sn_qbytearray sn_detail_test GTest::gtest GTest::gtest_main)
        lint_target(test_sn_qbytearray)

        add_custom_target(run_test_sn_qbytearray
            test_sn_qbytearray
            DEPENDS test_sn_qbytearray
            USES_TERMINAL)
        add_dependencies(run_all_tests run_test_sn_qbytearray)
    endif()
endif()
