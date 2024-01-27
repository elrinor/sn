cmake_minimum_required(VERSION 3.24 FATAL_ERROR)

if(SN_BUILD_TOOLS AND TARGET fmt::fmt) # ppgen is built only when fmt is available.
    set(SN_PPGEN_SOURCES
        ppgen.cpp)

    add_executable(ppgen ${SN_PPGEN_SOURCES})
    target_link_libraries(ppgen PUBLIC sn_string fmt::fmt)
    lint_target(ppgen)
endif()
