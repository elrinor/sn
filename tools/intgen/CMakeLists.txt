cmake_minimum_required(VERSION 3.24 FATAL_ERROR)

if(SN_BUILD_TOOLS)
    set(SN_INTGEN_SOURCES
        intgen.cpp)

    add_executable(sn_intgen ${SN_INTGEN_SOURCES})
    lint_target(sn_intgen)
endif()
