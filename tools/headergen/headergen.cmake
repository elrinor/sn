cmake_minimum_required(VERSION 3.24 FATAL_ERROR)

if (NOT SOURCE_DIR)
    message(FATAL_ERROR "SOURCE_DIR not set")
endif ()
if (NOT TARGET_DIR)
    message(FATAL_ERROR "TARGET_DIR not set")
endif ()
if (NOT NAME)
    message(FATAL_ERROR "NAME not set")
endif ()
if (NOT TYPE)
    message(FATAL_ERROR "TYPE not set")
endif ()

# Set up LOWER & UPPER.
string(TOLOWER "${NAME}" LOWER)
string(TOUPPER "${NAME}" UPPER)

# Set up SRC & DST.
if (VIEW)
    set(SRC "${VIEW} src")
else ()
    set(SRC "const ${TYPE} &src")
endif ()
set(DST "${TYPE} *dst")

# Set up INCLUDES, DECLS
if (INCLUDES_FILE)
    file(READ "${INCLUDES_FILE}" INCLUDES)
    string(APPEND INCLUDES "\n")
    string(PREPEND INCLUDES "\n")
endif ()
if (DECLS_FILE)
    file(READ "${DECLS_FILE}" DECLS)
    string(APPEND DECLS "\n")
    string(PREPEND DECLS "\n")
endif ()

configure_file("${SOURCE_DIR}/template.h" "${TARGET_DIR}/${LOWER}.h" @ONLY)
configure_file("${SOURCE_DIR}/template_fwd.h" "${TARGET_DIR}/${LOWER}_fwd.h" @ONLY)
configure_file("${SOURCE_DIR}/template_concepts.h" "${TARGET_DIR}/${LOWER}_concepts.h" @ONLY)
configure_file("${SOURCE_DIR}/detail/template_shortcuts.h" "${TARGET_DIR}/detail/${LOWER}_shortcuts.h" @ONLY)
