cmake_minimum_required(VERSION 3.24 FATAL_ERROR)

set(SN_DETAIL_WORKAROUND_SOURCES
    make_unique_for_overwrite.h
    to_underlying.h)

add_library(sn_detail_workaround INTERFACE ${SN_DETAIL_WORKAROUND_SOURCES})
lint_target(sn_detail_workaround)
