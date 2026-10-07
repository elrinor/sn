cmake_minimum_required(VERSION 3.24 FATAL_ERROR)

# Note that the module is called njson and not json. Since nlohmann_json 3.11.3, nlohmann::json has a base class in
# nlohmann::detail, so ADL with a nlohmann::json argument also finds nlohmann::detail::to_json / from_json. These
# hard-error during overload resolution for arguments in sn order, e.g. to_json(const T &, nlohmann::json *).

if(SN_JSON_LIB STREQUAL "nlohmann_json_bundled")
    # This is the archive that nlohmann_json recommends for FetchContent, it's a lot smaller than the repository.
    FetchContent_Declare(nlohmann_json
        URL "https://github.com/nlohmann/json/releases/download/${SN_BUNDLED_NLOHMANN_JSON_TAG}/json.tar.xz")
    FetchContent_MakeAvailable(nlohmann_json)
elseif(SN_JSON_LIB STREQUAL "nlohmann_json_external")
    find_package(nlohmann_json 3.11 REQUIRED)
elseif(NOT SN_JSON_LIB STREQUAL "none")
    message(FATAL_ERROR "Unsupported value '${SN_JSON_LIB}' for SN_JSON_LIB")
endif()

if(NOT SN_JSON_LIB STREQUAL "none")
    set(SN_NJSON_SOURCES
        class_njson.h
        enum_njson.h
        njson.h
        njson_concepts.h
        njson_fwd.h
        detail/njson_builtins.cpp
        detail/njson_builtins.h
        detail/njson_dispatch.h
        detail/njson_exceptions.cpp
        detail/njson_exceptions.h
        detail/njson_shortcuts.h
        detail/njson_std_builtins.h)

    add_library(sn_njson STATIC ${SN_NJSON_SOURCES})
    add_library(sn::njson ALIAS sn_njson)
    target_link_libraries(sn_njson PUBLIC sn_core sn_string sn_reflection sn_detail_codegen nlohmann_json::nlohmann_json)
    lint_target(sn_njson)

    add_generate_headers(sn_njson_generate_headers
        DIR ${CMAKE_CURRENT_SOURCE_DIR}
        NAME "njson"
        TYPE "nlohmann::json"
        INCLUDES "#include <nlohmann/json_fwd.hpp>")

    if(SN_BUILD_TESTS)
        set(TEST_SN_NJSON_SOURCES
            test/class_njson_ut.cpp
            test/enum_njson_ut.cpp
            test/njson_std_ut.cpp
            test/njson_ut.cpp)

        add_executable(test_sn_njson ${TEST_SN_NJSON_SOURCES})
        target_link_libraries(test_sn_njson PUBLIC sn_njson GTest::gtest GTest::gtest_main)
        lint_target(test_sn_njson)

        add_custom_target(run_test_sn_njson
            test_sn_njson
            DEPENDS test_sn_njson
            USES_TERMINAL)
        add_dependencies(run_all_tests run_test_sn_njson)
    endif()
endif()
