cmake_minimum_required(VERSION 3.24 FATAL_ERROR)

set(SN_DETAIL_ENUM_TABLE_SOURCES
    enum_table.h
    enum_table_errors.h
    enum_table_errors.cpp
    lowercase_buffer.h
    overwrite_buffer.h
    universal_enum_table.h)

add_library(sn_detail_enum_table STATIC ${SN_DETAIL_ENUM_TABLE_SOURCES})
target_link_libraries(sn_detail_enum_table PUBLIC sn_core sn_string)
lint_target(sn_detail_enum_table)
