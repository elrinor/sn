cmake_minimum_required(VERSION 3.24 FATAL_ERROR)

set(SN_ENUM_TABLE_SOURCES
    enum_table.h
    enum_table_exceptions.h
    enum_table_exceptions.cpp
    small_buffer.h
    universal_enum_table.h)

add_library(sn_enum_table STATIC ${SN_ENUM_TABLE_SOURCES})
add_library(sn::enum_table ALIAS sn_enum_table)
target_link_libraries(sn_enum_table PUBLIC sn_core sn_string sn_workaround)
lint_target(sn_enum_table)
