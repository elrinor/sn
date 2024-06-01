cmake_minimum_required(VERSION 3.24 FATAL_ERROR)

set(SN_DETAIL_VALIDATION_SOURCES
    validation.h)

add_library(sn_detail_validation INTERFACE ${SN_DETAIL_VALIDATION_SOURCES})
lint_target(sn_detail_validation)
