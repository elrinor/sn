cmake_minimum_required(VERSION 3.24 FATAL_ERROR)

set(SN_CORE_SOURCES
    enum_reflection.h
    exception.h
    format.h
    globals.h
    preprocessor.h
    type_name.h
    type_name_builtins.h)

add_library(sn_core INTERFACE ${SN_CORE_SOURCES})
add_library(sn::core ALIAS sn_core)
target_include_directories(sn_core INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/../..)
target_compile_definitions(sn_core INTERFACE SN_MAX_SMALL_BUFFER_SIZE=${SN_MAX_SMALL_BUFFER_SIZE})
lint_target(sn_core)

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(sn_core INTERFACE "/Zc:preprocessor")
endif()

if(SN_FORMAT_LIB STREQUAL "std")
    target_compile_definitions(sn_core INTERFACE SN_USE_STD_FORMAT=1)
elseif(SN_FORMAT_LIB STREQUAL "fmt_bundled" OR SN_FORMAT_LIB STREQUAL "fmt_external")
    target_compile_definitions(sn_core INTERFACE SN_USE_FMT_FORMAT=1)

    if(SN_FORMAT_LIB STREQUAL "fmt_bundled")
        FetchContent_Declare(fmt
                GIT_REPOSITORY "https://github.com/fmtlib/fmt.git"
                GIT_TAG "${SN_BUNDLED_FMT_TAG}")
        FetchContent_MakeAvailable(fmt)
    endif()

    target_link_libraries(sn_core INTERFACE fmt::fmt)
else()
    message(FATAL_ERROR "Unsupported value '${SN_FORMAT_LIB}' for SN_FORMAT_LIB")
endif()

if(SN_TYPE_NAME_IMPL STREQUAL "typeid")
    target_compile_definitions(sn_core INTERFACE SN_USE_TYPEID_TYPE_NAME=1)
elseif(SN_TYPE_NAME_IMPL STREQUAL "funcsig")
    target_compile_definitions(sn_core INTERFACE SN_USE_FUNCSIG_TYPE_NAME=1)
else()
    message(FATAL_ERROR "Unsupported value '${SN_TYPE_NAME_IMPL}' for SN_TYPE_NAME_IMPL")
endif()

if(SN_BUILD_TESTS)
    set(TEST_SN_CORE_SOURCES
        preprocessor_ut.cpp
        type_name_ut.cpp)

    add_executable(test_sn_core ${TEST_SN_CORE_SOURCES})
    target_link_libraries(test_sn_core PUBLIC sn_core GTest::gtest GTest::gtest_main)
    lint_target(test_sn_core)

    add_custom_target(run_test_sn_core
            test_sn_core
            DEPENDS test_sn_core
            USES_TERMINAL)
    add_dependencies(run_all_tests run_test_sn_core)
endif()
