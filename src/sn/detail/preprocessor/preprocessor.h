#pragma once

#include "sn/core/preprocessor.h"

// This is where the common preprocessing routines go that we don't expose as part of SN interface.

/**
 * @internal
 *
 * Converts a tuple of type names into a tuple of default-constructed values.
 *
 * For example, `_SN_PP_TUPLE_TYPES_TO_DEFALT_CTORS((A, B))` will expand to `(A(), B())`.
 *
 * @param TYPES_TUPLE                   Tuple of type names.
 */
#define _SN_PP_TUPLE_TYPES_TO_DEFALT_CTORS(TYPES_TUPLE)                                                                 \
    SN_PP_TUPLE_TRANSFORM(_SN_PP_TUPLE_TYPES_TO_DEFALT_CTORS_I, TYPES_TUPLE)
#define _SN_PP_TUPLE_TYPES_TO_DEFALT_CTORS_I(X) X()

