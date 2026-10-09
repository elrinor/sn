//
// This header is auto-generated using the headergen tool in /tools.
//
#pragma once
@INCLUDES@
#include "sn/core/error_fwd.h"
#include "sn/core/preprocessor.h" // For __VA_OPT__.
@DECLS@
/**
 * @internal
 *
 * Same as `SN_DECLARE_@UPPER@_FUNCTIONS`, but `to_@LOWER@` takes `TYPE` by value. This can result in better codegen on
 * most architectures as the 1st arg can now be passed in registers.
 */
#define _SN_DECLARE_@UPPER@_FUNCTIONS_BY_VALUE(TYPE, ... /* TAGS */) \
    [[nodiscard]] bool to_@LOWER@(TYPE src, @DST@, sn::error *err __VA_OPT__(,) __VA_ARGS__); \
    [[nodiscard]] bool from_@LOWER@(@SRC@, TYPE *dst, sn::error *err __VA_OPT__(,) __VA_ARGS__);

/**
 * @internal
 *
 * Unlike the `DECLARE_*` macros, this macro defines `@TYPE@` functions for `TYPE` that just ignore the provided `TAG`,
 * effectively shifting the tag sequence by a single position to the left.
 *
 * Note that this macro will only work when invoked from the `sn::detail::builtins` namespace because it's calling
 * `@TYPE@` functions directly (e.g. calling `to_@LOWER@` instead of `sn::to_@LOWER@`).
 */
#define _SN_DEFINE_INLINE_@UPPER@_TAG_EATING_FUNCTIONS(TYPE, TAG) \
    template<class... Tags> \
    [[nodiscard]] inline bool to_@LOWER@(const TYPE &src, @DST@, sn::error *err, TAG, Tags... tags) { \
        return to_@LOWER@(src, dst, err, tags...); \
    } \
    template<class... Tags> \
    [[nodiscard]] inline bool from_@LOWER@(@SRC@, TYPE *dst, sn::error *err, TAG, Tags... tags) { \
        return from_@LOWER@(src, dst, err, tags...); \
    }
