cmake_minimum_required(VERSION 3.24 FATAL_ERROR)

set(SN_REFLECTION_SOURCES
    class_reflection.h
    class_reflection_data.h
    detail/reflection_type_traits.h
    detail/member_reflection.h)

add_library(sn_reflection INTERFACE ${SN_REFLECTION_SOURCES})
add_library(sn::reflection ALIAS sn_reflection)
target_link_libraries(sn_reflection INTERFACE sn_core)
lint_target(sn_reflection)

if(SN_BUILD_TESTS)
    set(TEST_SN_REFLECTION_SOURCES
        test/reflection_ut.cpp)

    add_executable(test_sn_reflection ${TEST_SN_REFLECTION_SOURCES})
    target_link_libraries(test_sn_reflection PUBLIC sn_reflection GTest::gtest GTest::gtest_main)
    lint_target(test_sn_reflection)

    add_custom_target(run_test_sn_reflection
        test_sn_reflection
        DEPENDS test_sn_reflection
        USES_TERMINAL)
    add_dependencies(run_all_tests run_test_sn_reflection)
endif()
