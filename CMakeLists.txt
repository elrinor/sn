cmake_minimum_required(VERSION 3.24 FATAL_ERROR)

project("sn")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


#
# External settings
#
set(SN_BUILD_TESTS "${PROJECT_IS_TOP_LEVEL}" CACHE BOOL "Build SN tests")
set(SN_CHECK_STYLE "${PROJECT_IS_TOP_LEVEL}" CACHE BOOL "Enable style checks")
set(SN_FORMAT_LIB "fmt_bundled" CACHE STRING "Format library to use, 'std', 'fmt_bundled', or 'fmt_external'")
set_property(CACHE SN_FORMAT_LIB PROPERTY STRINGS std fmt_bundled fmt_external)
set(SN_FLOAT_LIB "fast_float_bundled" CACHE STRING "Float string functions to use, 'strtof', 'from_chars', 'fast_float_bundled', or 'fast_float_external'")
set_property(CACHE SN_FLOAT_LIB PROPERTY STRINGS strtof from_chars fast_float_bundled fast_float_external)
set(SN_TYPE_NAME_IMPL "funcsig" CACHE STRING "Implementation of sn::type_name to use, 'typeid' or 'funcsig'")
set_property(CACHE SN_TYPE_NAME_IMPL PROPERTY STRINGS typeid funcsig)
set(SN_ENUM_HASH_LIB "std" CACHE STRING "Hash library to use for enum serialization hash maps, 'std', 'frozen_bundled', or 'frozen_external'")
set_property(CACHE SN_ENUM_HASH_LIB PROPERTY STRINGS std frozen_bundled frozen_external)

set(SN_BUNDLED_FMT_TAG "10.1.1" CACHE STRING "Git tag for the bundled fmt library")
set(SN_BUNDLED_FAST_FLOAT_TAG "v5.2.0" CACHE STRING "Git tag for the bundled fast_float library")
set(SN_BUNDLED_FROZEN_TAG "dd1f58c5f6c97fbf0832cc4e84676663839b913e" CACHE STRING "Git tag for the bundled frozen library")
set(SN_BUNDLED_GOOGLETEST_TAG "v1.14.0" CACHE STRING "Git tag for the bundled google test library")
set(SN_BUNDLED_CPPLINT_TAG "b3fcac06919b83d3e9c8a3b6dc450eba02869092" CACHE STRING "Git tag for the bundled cpplint")

include(FetchContent)


#
# Set up tests.
#
if(SN_BUILD_TESTS)
    FetchContent_Declare(googletest
            GIT_REPOSITORY https://github.com/google/googletest.git
            GIT_TAG "${SN_BUNDLED_GOOGLETEST_TAG}")
    FetchContent_MakeAvailable(googletest)

    add_custom_target(run_all_tests)
endif()


#
# Set up linting.
#
if(SN_CHECK_STYLE)
    find_package(Python COMPONENTS Interpreter)

    FetchContent_Declare(cpplint
            GIT_REPOSITORY https://github.com/elrinor/cpplint.git
            GIT_TAG "${SN_BUNDLED_CPPLINT_TAG}")
    FetchContent_MakeAvailable(cpplint)

    set(SN_CPPLINT_SCRIPT "${cpplint_SOURCE_DIR}/cpplint.py" CACHE STRING "Path to cpplint script")

    add_custom_target(lint_all)
endif()

function(lint_target TARGET)
    if(SN_CHECK_STYLE)
        get_target_property(SOURCES_ALL ${TARGET} SOURCES)

        set(SOURCES_FILTERED)
        foreach(SOURCE_FILE ${SOURCES_ALL})
            if(SOURCE_FILE MATCHES \\.c$|\\.cxx$|\\.cpp$|\\.cc$|\\.h$|\\.hh$)
                list(APPEND SOURCES_FILTERED ${SOURCE_FILE})
            endif()
        endforeach()

        add_custom_target("lint_${TARGET}"
                COMMAND "${Python_EXECUTABLE}" "${SN_CPPLINT_SCRIPT}" "--quiet" ${SOURCES_FILTERED}
                DEPENDS ${SOURCES_FILTERED}
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
        add_dependencies(lint_all "lint_${TARGET}")
    endif()
endfunction()


#
# Set up warnings-as-errors for all compilers. Only when building standalone.
#
if(PROJECT_IS_TOP_LEVEL)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
        add_compile_options(-Werror=return-type) # Control reaches the end of non-void function. This is an error on MSVC.
        add_compile_options(-Werror=unused-result) # Ignoring return value of function declared with 'nodiscard' attribute.
        add_compile_options(-Werror=unused-value) # Expression result unused.
        add_compile_options(-Werror=suggest-override) # 'func' overrides a member function but is not marked 'override'.
    endif()
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        add_compile_options(-Werror=return-local-addr) # Returning reference to temporary.
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
        add_compile_options(-Werror=return-stack-address) # Returning reference to temporary.
        add_compile_options(-Werror=unused-comparison) # Comparison result unused.
        add_compile_options(-Werror=inconsistent-missing-override) # 'override' only used on some of the overridden functions.
        add_compile_options(-Werror=invalid-noreturn) # function declared 'noreturn' should not return.
        add_compile_options(-Werror=uninitialized) # variable 'x' is used uninitialized.
        add_compile_options(-Werror=delete-abstract-non-virtual-dtor) # delete called on 'x' that is abstract but has non-virtual destructor.
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        add_compile_options(/we4834) # Discarding return value of function with 'nodiscard' attribute.
        add_compile_options(/we4172) # Returning address of local variable or temporary.
        add_compile_options(/we26433) # Function should be marked with 'override'.
    endif()
endif()

add_subdirectory(src)
