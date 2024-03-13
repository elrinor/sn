cmake_minimum_required(VERSION 3.24 FATAL_ERROR)

set(SN_DETAIL_PREPROCESSOR_SOURCES
    preprocessor.h)

add_library(sn_detail_preprocessor INTERFACE ${SN_DETAIL_PREPROCESSOR_SOURCES})
lint_target(sn_detail_preprocessor)
