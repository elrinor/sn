#pragma once

#include "sn/core/preprocessor.h"

// This is where the common preprocessing routines go that we don't expose as part of SN interface.

#define _SN_PP_ADD_DEFAULT_CTOR(X) X()

/**
 * @internal
 *
 * Converts a tuple of type names into a comma-separated sequence of default-constructed values.
 *
 * For example, `_SN_TUPLE_ENUM_DEFAULT_CTORS((A, B))` will expand to `A(), B()`. Empty tuple expands to nothing.
 *
 * @param TUPLE                         Tuple of type names.
 */
#define _SN_TUPLE_ENUM_DEFAULT_CTORS(TUPLE) SN_PP_TUPLE_ENUM(SN_PP_TUPLE_TRANSFORM(_SN_PP_ADD_DEFAULT_CTOR, TUPLE))

