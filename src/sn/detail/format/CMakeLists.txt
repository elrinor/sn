cmake_minimum_required(VERSION 3.24 FATAL_ERROR)

set(SN_DETAIL_FORMAT_SOURCES
    format.h)

add_library(sn_detail_format INTERFACE ${SN_DETAIL_FORMAT_SOURCES})
lint_target(sn_detail_format)

if(SN_FORMAT_LIB STREQUAL "std")
    target_compile_definitions(sn_detail_format INTERFACE SN_USE_STD_FORMAT=1)
elseif(SN_FORMAT_LIB STREQUAL "fmt_bundled" OR SN_FORMAT_LIB STREQUAL "fmt_external")
    target_compile_definitions(sn_detail_format INTERFACE SN_USE_FMT_FORMAT=1)

    if(SN_FORMAT_LIB STREQUAL "fmt_bundled")
        FetchContent_Declare(fmt
            GIT_REPOSITORY "https://github.com/fmtlib/fmt.git"
            GIT_TAG "${SN_BUNDLED_FMT_TAG}")
        FetchContent_MakeAvailable(fmt)
    endif()

    target_link_libraries(sn_detail_format INTERFACE fmt::fmt)
else()
    message(FATAL_ERROR "Unsupported value '${SN_FORMAT_LIB}' for SN_FORMAT_LIB")
endif()
