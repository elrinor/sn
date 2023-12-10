#pragma once

#if defined(_MSC_VER) && (!defined(_MSVC_TRADITIONAL) || _MSVC_TRADITIONAL)
#   error "sn doesn't support traditional MSVC preprocessor, build with /Zc:preprocessor"
#endif

/**
 * Concatenates two tokens together, performing macro substitution first.
 */
#define SN_PP_CAT(x, y) _SN_PP_CAT_I(x, y)

#define _SN_PP_CAT_I(x, y) x ## y
