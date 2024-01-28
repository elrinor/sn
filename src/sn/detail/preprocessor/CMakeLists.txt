cmake_minimum_required(VERSION 3.24 FATAL_ERROR)

set(SN_PREPROCESSOR_SOURCES
    preprocessor.h)

add_library(sn_preprocessor INTERFACE ${SN_PREPROCESSOR_SOURCES})
add_library(sn::preprocessor ALIAS sn_preprocessor)
lint_target(sn_preprocessor)
