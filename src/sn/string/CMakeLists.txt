cmake_minimum_required(VERSION 3.24 FATAL_ERROR)

set(SN_STRING_SOURCES
    string_builtins.cpp
    string_exceptions.cpp)

set(SN_STRING_HEADERS
    string.h
    string_builtins.h
    string_exceptions.h
    string_fwd.h)

add_library(sn_string STATIC ${SN_STRING_SOURCES} ${SN_STRING_HEADERS})
add_library(sn::string ALIAS sn_string)
target_link_libraries(sn_string PUBLIC sn_core)

if(SN_FLOAT_LIB STREQUAL "strtof")
    target_compile_definitions(sn_string PUBLIC SN_USE_STRTOF=1)
elseif(SN_FLOAT_LIB STREQUAL "fast_float_bundled" OR SN_FORMAT_LIB STREQUAL "fast_float_external")
    target_compile_definitions(sn_string PUBLIC SN_USE_FAST_FLOAT=1)

    if(SN_FLOAT_LIB STREQUAL "fast_float_bundled")
        FetchContent_Declare(fast_float
                GIT_REPOSITORY "https://github.com/fastfloat/fast_float.git"
                GIT_TAG "${SN_BUNDLED_FAST_FLOAT_TAG}")
        FetchContent_MakeAvailable(fast_float)
    endif()

    target_link_libraries(sn_string PUBLIC FastFloat::fast_float)
else()
    message(FATAL_ERROR "Unsupported value '${SN_FLOAT_LIB}' for SN_FLOAT_LIB")
endif()

if(SN_BUILD_TESTS)
    set(TEST_SN_STRING_SOURCES string_ut.cpp)

    add_executable(test_sn_string ${TEST_SN_STRING_SOURCES})
    target_link_libraries(test_sn_string PUBLIC sn_string GTest::gtest GTest::gtest_main)

    add_custom_target(run_test_sn_string
            test_sn_string
            DEPENDS test_sn_string
            USES_TERMINAL)
    add_dependencies(run_all_tests run_test_sn_string)
endif()
