cmake_minimum_required(VERSION 3.24 FATAL_ERROR)

set(SN_WORKAROUND_SOURCES
    make_unique_for_overwrite.h
    to_underlying.h)

add_library(sn_workaround INTERFACE ${SN_WORKAROUND_SOURCES})
add_library(sn::workaround ALIAS sn_workaround)
target_include_directories(sn_workaround INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/../..)
lint_target(sn_workaround)
