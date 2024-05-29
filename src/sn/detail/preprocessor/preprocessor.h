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


/**
 * @internal
 *
 * Converts a tuple of type names into a tuple of function parameter declarations, using `NAME_PREFIX` as the prefix for
 * parameter names.
 *
 * For example, `_SN_PP_TUPLE_TYPES_TO_DECL_PARAMS(arg, (int, float))` will expand to `(int arg0, float arg1)`.
 *
 * @param NAME_PREFIX                   Prefix for the parameter names.
 * @param TYPES_TUPLE                   Tuple of type names.
 */
#define _SN_PP_TUPLE_TYPES_TO_DECL_PARAMS(NAME_PREFIX, TYPES_TUPLE)                                                     \
    SN_PP_TUPLE_TRANSFORM_DI(_SN_PP_TUPLE_TYPES_TO_DECL_PARAMS_I, NAME_PREFIX, TYPES_TUPLE)
#define _SN_PP_TUPLE_TYPES_TO_DECL_PARAMS_I(NAME_PREFIX, I, TYPE)                                                       \
    TYPE SN_PP_CAT(NAME_PREFIX, I)


/**
 * @internal
 *
 * To be used together with `_SN_PP_TUPLE_TYPES_TO_DECL_PARAMS`.
 *
 * Converts a tuple of type names into a tuple of function arguments suitable to be used in a function call. This means
 * that the type names in `TYPES_TUPLE` are effectively ignored, only the tuple size is used.
 *
 * `_SN_PP_TUPLE_TYPES_TO_CALL_PARAMS(arg, (int, float))` will expand to `(arg0, arg1)`.
 *
 * @param NAME_PREFIX                   Prefix for the parameter names.
 * @param TYPES_TUPLE                   Tuple of type names.
 */
#define _SN_PP_TUPLE_TYPES_TO_CALL_PARAMS(NAME_PREFIX, TYPES_TUPLE)                                                     \
    SN_PP_TUPLE_TRANSFORM_DI(_SN_PP_TUPLE_TYPES_TO_CALL_PARAMS_I, NAME_PREFIX, TYPES_TUPLE)
#define _SN_PP_TUPLE_TYPES_TO_CALL_PARAMS_I(NAME_PREFIX, I, TYPE)                                                       \
    SN_PP_CAT(NAME_PREFIX, I)
