cmake_minimum_required(VERSION 3.24 FATAL_ERROR)

if (SN_BUILD_TOOLS)
    add_custom_target(sn_generate_headers)
endif ()

function(add_generate_headers TARGET_NAME)
    if (SN_BUILD_TOOLS)
        cmake_parse_arguments(PARSE_ARGV 1
            ARG
            ""
            "DIR;NAME;TYPE;VIEW;INCLUDES;DECLS"
            ""
        )

        if (NOT ARG_DIR)
            message(FATAL_ERROR "DIR is required")
        endif ()

        if (NOT ARG_NAME)
            message(FATAL_ERROR "NAME is required")
        endif ()

        if (NOT ARG_TYPE)
            message(FATAL_ERROR "TYPE is required")
        endif ()

        # Unfortunately we can't pass values with \n with them to a cmake script in args - ninja generator chokes on
        # this. There is an option of passing them line-by-line, but we're just passing the text in files instead.
        if (ARG_INCLUDES)
            set(INCLUDES_FILE "${CMAKE_CURRENT_BINARY_DIR}/${ARG_NAME}_includes.h.in")
            file(WRITE "${INCLUDES_FILE}" "${ARG_INCLUDES}")
        endif ()

        if (ARG_DECLS)
            set(DECLS_FILE "${CMAKE_CURRENT_BINARY_DIR}/${ARG_NAME}_decls.h.in")
            file(WRITE "${DECLS_FILE}" "${ARG_DECLS}")
        endif ()

        add_custom_target(
            ${TARGET_NAME}
            COMMAND "${CMAKE_COMMAND}"
                -DSOURCE_DIR=${CMAKE_CURRENT_FUNCTION_LIST_DIR}
                -DTARGET_DIR=${ARG_DIR}
                -DNAME=${ARG_NAME}
                -DTYPE=${ARG_TYPE}
                -DVIEW=${ARG_VIEW}
                -DINCLUDES_FILE=${INCLUDES_FILE}
                -DDECLS_FILE=${DECLS_FILE}
                -P "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/headergen.cmake"
            SOURCES
                "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/template.h"
                "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/template_concepts.h"
                "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/template_fwd.h"
        )

        add_dependencies(sn_generate_headers ${TARGET_NAME})
    endif ()
endfunction()
