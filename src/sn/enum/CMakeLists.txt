cmake_minimum_required(VERSION 3.24 FATAL_ERROR)

set(SN_ENUM_SOURCES
    enum_reflection.h
    enum_string.h
    frozen_enum_table.h
    std_enum_table.cpp
    std_enum_table.h)

add_library(sn_enum STATIC ${SN_ENUM_SOURCES})
add_library(sn::enum ALIAS sn_enum)
target_compile_definitions(sn_enum PUBLIC SN_MAX_SMALL_BUFFER_SIZE=${SN_MAX_SMALL_BUFFER_SIZE})
target_link_libraries(sn_enum PUBLIC sn_core sn_string sn_ascii sn_enum_table)
lint_target(sn_enum)

if(SN_ENUM_HASH_LIB STREQUAL "std")
    target_compile_definitions(sn_core INTERFACE SN_USE_STD_ENUM_HASH=1)
elseif(SN_ENUM_HASH_LIB STREQUAL "frozen_bundled" OR SN_ENUM_HASH_LIB STREQUAL "frozen_external")
    target_compile_definitions(sn_core INTERFACE SN_USE_FROZEN_ENUM_HASH=1)

    if(SN_ENUM_HASH_LIB STREQUAL "frozen_bundled")
        FetchContent_Declare(frozen
            GIT_REPOSITORY "https://github.com/serge-sans-paille/frozen.git"
            GIT_TAG "${SN_BUNDLED_FROZEN_TAG}")
        FetchContent_MakeAvailable(frozen)
    endif()

    target_link_libraries(sn_enum INTERFACE frozen::frozen)
else()
    message(FATAL_ERROR "Unsupported value '${SN_ENUM_HASH_LIB}' for SN_ENUM_HASH_LIB")
endif()

if(SN_BUILD_TESTS)
    set(TEST_SN_ENUM_SOURCES
        enum_string_multi_ut.h
        enum_string_multi_first_ut.cpp
        enum_string_multi_second_ut.cpp
        enum_string_ut.cpp)

    add_executable(test_sn_enum ${TEST_SN_ENUM_SOURCES})
    target_link_libraries(test_sn_enum PUBLIC sn_enum GTest::gtest GTest::gtest_main)
    lint_target(test_sn_enum)

    add_custom_target(run_test_sn_enum
        test_sn_enum
        DEPENDS test_sn_enum
        USES_TERMINAL)
    add_dependencies(run_all_tests run_test_sn_enum)
endif()

if(SN_BUILD_BENCHMARKS)
    set(BENCHMARK_SN_ENUM_SOURCES enum_string_benchmark.cpp)

    add_executable(benchmark_sn_enum ${BENCHMARK_SN_ENUM_SOURCES})
    target_link_libraries(benchmark_sn_enum PUBLIC sn_enum benchmark::benchmark benchmark::benchmark_main)
    lint_target(benchmark_sn_enum)

    add_custom_target(run_benchmark_sn_enum
        benchmark_sn_enum
        DEPENDS benchmark_sn_enum
        USES_TERMINAL)
    add_dependencies(run_all_benchmarks run_benchmark_sn_enum)
endif()
