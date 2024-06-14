cmake_minimum_required(VERSION 3.24 FATAL_ERROR)

set(SN_DETAIL_CODEGEN_SOURCES
    forwarding.h
    tuple_types.h
    validation.h)

add_library(sn_detail_codegen INTERFACE ${SN_DETAIL_CODEGEN_SOURCES})
lint_target(sn_detail_codegen)
