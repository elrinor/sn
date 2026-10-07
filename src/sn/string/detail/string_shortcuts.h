//
// This header is auto-generated using the headergen tool in /tools.
//
#pragma once

#include <string>
#include <string_view>

#include "sn/core/error_fwd.h"
#include "sn/core/preprocessor.h" // For __VA_OPT__.

/**
 * @internal
 *
 * Same as `SN_DECLARE_STRING_FUNCTIONS`, but `to_string` takes `TYPE` by value. This can result in better codegen on
 * most architectures as the 1st arg can now be passed in registers.
 */
#define _SN_DECLARE_STRING_FUNCTIONS_BY_VALUE(TYPE, ... /* TAGS */) \
    [[nodiscard]] bool to_string(TYPE src, std::string *dst, sn::error *err __VA_OPT__(,) __VA_ARGS__); \
    [[nodiscard]] bool from_string(std::string_view src, TYPE *dst, sn::error *err __VA_OPT__(,) __VA_ARGS__);

/**
 * @internal
 *
 * Unlike the `DECLARE_*` macros, this macro defines `std::string` functions for `TYPE` that just ignore the provided `TAG`,
 * effectively shifting the tag sequence by a single position to the left.
 *
 * Note that this macro will only work when invoked from the `sn::detail::builtins` namespace because it's calling
 * `std::string` functions directly (e.g. calling `to_string` instead of `sn::to_string`).
 */
#define _SN_DEFINE_INLINE_STRING_TAG_EATING_FUNCTIONS(TYPE, TAG) \
    template<class... Tags> \
    [[nodiscard]] inline bool to_string(const TYPE &src, std::string *dst, sn::error *err, TAG, Tags... tags) { \
        return to_string(src, dst, err, tags...); \
    } \
    template<class... Tags> \
    [[nodiscard]] inline bool from_string(std::string_view src, TYPE *dst, sn::error *err, TAG, Tags... tags) { \
        return from_string(src, dst, err, tags...); \
    }
