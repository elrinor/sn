cmake_minimum_required(VERSION 3.24 FATAL_ERROR)

if(SN_BUILD_TOOLS AND TARGET fmt::fmt) # intgen is built only when fmt is available.
    set(SN_INTGEN_SOURCES
        intgen.cpp)

    add_executable(sn_intgen ${SN_INTGEN_SOURCES})
    target_link_libraries(sn_intgen PUBLIC fmt::fmt)
    lint_target(sn_intgen)
endif()
