cmake_minimum_required(VERSION 3.24 FATAL_ERROR)

project("sn")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# External settings
set(SN_BUILD_TESTS "${PROJECT_IS_TOP_LEVEL}" CACHE BOOL "Build SN tests")
set(SN_CHECK_STYLE "${PROJECT_IS_TOP_LEVEL}" CACHE BOOL "Enable style checks")
set(SN_FORMAT_LIB "fmt_bundled" CACHE STRING "Format library to use, 'std', 'fmt_bundled', or 'fmt_external'")
set_property(CACHE SN_FORMAT_LIB PROPERTY STRINGS std fmt_bundled fmt_external)
set(SN_FLOAT_LIB "fast_float_bundled" CACHE STRING "Float string functions to use, 'strtof', 'fast_float_bundled', or 'fast_float_external'")
set_property(CACHE SN_FLOAT_LIB PROPERTY STRINGS strtof fast_float_bundled fast_float_external)
set(SN_TYPE_NAME_IMPL "funcsig" CACHE STRING "Implementation of sn::type_name to use, 'typeid' or 'funcsig'")
set_property(CACHE SN_TYPE_NAME_IMPL PROPERTY STRINGS typeid funcsig)
set(SN_BUNDLED_FMT_TAG "10.1.1" CACHE STRING "Git tag for the bundled fmt library")
set(SN_BUNDLED_FAST_FLOAT_TAG "v5.2.0" CACHE STRING "Git tag for the bundled fast_float library")
set(SN_BUNDLED_GOOGLETEST_TAG "v1.14.0" CACHE STRING "Git tag for the bundled google test library")
set(SN_BUNDLED_CPPLINT_TAG "7b88b68187e3516540fab3caa900988d2179ed24" CACHE STRING "Git tag for the bundled cpplint")

include(FetchContent)

if(SN_BUILD_TESTS)
    FetchContent_Declare(googletest
            GIT_REPOSITORY https://github.com/google/googletest.git
            GIT_TAG "${SN_BUNDLED_GOOGLETEST_TAG}")
    FetchContent_MakeAvailable(googletest)

    add_custom_target(run_all_tests)
endif()

if(SN_CHECK_STYLE)
    find_package(Python COMPONENTS Interpreter)

    FetchContent_Declare(cpplint
            GIT_REPOSITORY https://github.com/cpplint/cpplint.git
            GIT_TAG "${SN_BUNDLED_CPPLINT_TAG}")
    FetchContent_MakeAvailable(cpplint)

    set(SN_CPPLINT_SCRIPT "${cpplint_SOURCE_DIR}/cpplint.py")

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

add_subdirectory(src)
