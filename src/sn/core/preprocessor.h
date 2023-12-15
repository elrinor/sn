#pragma once

#if defined(_MSC_VER) && (!defined(_MSVC_TRADITIONAL) || _MSVC_TRADITIONAL)
#   error "sn doesn't support traditional MSVC preprocessor, build with /Zc:preprocessor"
#endif

/**
 * Concatenates two tokens together, performing macro substitution first.
 */
#define SN_PP_CAT(x, y) _SN_PP_CAT_I(x, y)

#define _SN_PP_CAT_I(x, y) x ## y


/**
 * Removes parentheses from the provided argument.
 */
#define SN_PP_REMOVE_PARENS(x) _SN_PP_REMOVE_PARENS_I x

#define _SN_PP_REMOVE_PARENS_I(...) __VA_ARGS__
