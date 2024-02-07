cmake_minimum_required(VERSION 3.24 FATAL_ERROR)

set(SN_STRING_SOURCES
    enum_string.h
    frozen_enum_table.h
    std_enum_table.cpp
    std_enum_table.h
    string.h
    string_builtins.cpp
    string_builtins.h
    string_concepts.h
    string_exceptions.cpp
    string_exceptions.h
    string_fwd.h)

add_library(sn_string STATIC ${SN_STRING_SOURCES})
add_library(sn::string ALIAS sn_string)
target_link_libraries(sn_string PUBLIC sn_core sn_detail_ascii sn_detail_enum_table sn_detail_preprocessor)
lint_target(sn_string)

if(SN_FLOAT_LIB STREQUAL "strtof")
    target_compile_definitions(sn_string PUBLIC SN_USE_STRTOF=1)
elseif(SN_FLOAT_LIB STREQUAL "from_chars")
    target_compile_definitions(sn_string PUBLIC SN_USE_FROM_CHARS=1)
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

if(SN_ENUM_HASH_LIB STREQUAL "std")
    target_compile_definitions(sn_string INTERFACE SN_USE_STD_ENUM_HASH=1)
elseif(SN_ENUM_HASH_LIB STREQUAL "frozen_bundled" OR SN_ENUM_HASH_LIB STREQUAL "frozen_external")
    target_compile_definitions(sn_string INTERFACE SN_USE_FROZEN_ENUM_HASH=1)

    if(SN_ENUM_HASH_LIB STREQUAL "frozen_bundled")
        FetchContent_Declare(frozen
            GIT_REPOSITORY "https://github.com/serge-sans-paille/frozen.git"
            GIT_TAG "${SN_BUNDLED_FROZEN_TAG}")
        FetchContent_MakeAvailable(frozen)
    endif()

    target_link_libraries(sn_string INTERFACE frozen::frozen)
else()
    message(FATAL_ERROR "Unsupported value '${SN_ENUM_HASH_LIB}' for SN_ENUM_HASH_LIB")
endif()

if(SN_BUILD_TESTS)
    set(TEST_SN_STRING_SOURCES
        enum_string_multi_ut.h
        enum_string_multi_first_ut.cpp
        enum_string_multi_second_ut.cpp
        enum_string_ut.cpp
        string_ut.cpp
        string_ut.h)

    add_executable(test_sn_string ${TEST_SN_STRING_SOURCES})
    target_link_libraries(test_sn_string PUBLIC sn_string sn_detail_test GTest::gtest GTest::gtest_main)
    lint_target(test_sn_string)

    add_custom_target(run_test_sn_string
            test_sn_string
            DEPENDS test_sn_string
            USES_TERMINAL)
    add_dependencies(run_all_tests run_test_sn_string)
endif()

if(SN_BUILD_BENCHMARKS)
    set(BENCHMARK_SN_STRING_SOURCES enum_string_benchmark.cpp)

    add_executable(benchmark_sn_string ${BENCHMARK_SN_STRING_SOURCES})
    target_link_libraries(benchmark_sn_string PUBLIC sn_string benchmark::benchmark benchmark::benchmark_main)
    lint_target(benchmark_sn_string)

    add_custom_target(run_benchmark_sn_string
        benchmark_sn_string
        DEPENDS benchmark_sn_string
        USES_TERMINAL)
    add_dependencies(run_all_benchmarks run_benchmark_sn_string)
endif()
