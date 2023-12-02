cmake_minimum_required(VERSION 3.24 FATAL_ERROR)

project("sn")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# External settings
set(SN_BUILD_TESTS "${PROJECT_IS_TOP_LEVEL}" CACHE BOOL "Build SN tests")
set(SN_FORMAT_LIB "fmt_bundled" CACHE STRING "Format library to use, 'std', 'fmt_bundled', or 'fmt_external'")
set_property(CACHE SN_FORMAT_LIB PROPERTY STRINGS std fmt_bundled fmt_external)
set(SN_FLOAT_LIB "fast_float_bundled" CACHE STRING "Float string functions to use, 'strtof', 'fast_float_bundled', or 'fast_float_external'")
set_property(CACHE SN_FLOAT_LIB PROPERTY STRINGS strtof fast_float_bundled fast_float_external)
set(SN_TYPE_NAME_IMPL "funcsig" CACHE STRING "Implementation of sn::type_name to use, 'typeid' or 'funcsig'")
set_property(CACHE SN_TYPE_NAME_IMPL PROPERTY STRINGS typeid funcsig)
set(SN_BUNDLED_FMT_TAG "10.1.1" CACHE STRING "Git tag for the bundled fmt library")
set(SN_BUNDLED_FAST_FLOAT_TAG "v5.2.0" CACHE STRING "Git tag for the bundled fast_float library")

include(FetchContent)

if(SN_BUILD_TESTS)
    FetchContent_Declare(googletest
            GIT_REPOSITORY https://github.com/google/googletest.git
            GIT_TAG v1.14.0)
    FetchContent_MakeAvailable(googletest)

    add_custom_target(run_all_tests)
endif()

add_subdirectory(src)
