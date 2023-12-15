cmake_minimum_required(VERSION 3.24 FATAL_ERROR)

set(SN_ENUM_SOURCES
    enum_exceptions.cpp
    std_enum_table.cpp)

set(SN_ENUM_HEADERS
    ascii_functions.h
    enum_exceptions.h
    enum_fwd.h
    enum_reflection.h
    enum_string.h
    std_enum_table.h)

add_library(sn_enum STATIC ${SN_ENUM_SOURCES} ${SN_ENUM_HEADERS})
add_library(sn::enum ALIAS sn_enum)
target_link_libraries(sn_enum PUBLIC sn_core sn_string sn_workaround)
lint_target(sn_enum)

if(SN_ENUM_HASH_LIB STREQUAL "std")
    target_compile_definitions(sn_core PUBLIC SN_USE_STD_ENUM_HASH=1)
elseif(SN_ENUM_HASH_LIB STREQUAL "frozen_bundled" OR SN_ENUM_HASH_LIB STREQUAL "frozen_external")
    target_compile_definitions(sn_core PUBLIC SN_USE_FROZEN_ENUM_HASH=1)

    if(SN_ENUM_HASH_LIB STREQUAL "frozen_bundled")
        FetchContent_Declare(frozen
            GIT_REPOSITORY "https://github.com/serge-sans-paille/frozen.git"
            GIT_TAG "${SN_BUNDLED_FROZEN_TAG}")
        FetchContent_MakeAvailable(frozen)
    endif()

    target_link_libraries(sn_enum PUBLIC frozen::frozen)
else()
    message(FATAL_ERROR "Unsupported value '${SN_ENUM_HASH_LIB}' for SN_ENUM_HASH_LIB")
endif()

if(SN_BUILD_TESTS)
    set(TEST_SN_ENUM_SOURCES enum_string_ut.cpp)

    add_executable(test_sn_enum ${TEST_SN_ENUM_SOURCES})
    target_link_libraries(test_sn_enum PUBLIC sn_enum GTest::gtest GTest::gtest_main)
    lint_target(test_sn_enum)

    add_custom_target(run_test_sn_enum
        test_sn_enum
        DEPENDS test_sn_enum
        USES_TERMINAL)
    add_dependencies(run_all_tests run_test_sn_enum)
endif()
