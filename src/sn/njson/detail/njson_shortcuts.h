//
// This header is auto-generated using the headergen tool in /tools.
//
#pragma once

#include <nlohmann/json_fwd.hpp>

#include "sn/core/preprocessor.h" // For __VA_OPT__.

/**
 * @internal
 *
 * Same as `SN_DECLARE_NJSON_FUNCTIONS`, but `to_njson` and `try_to_njson` take `TYPE` by value. This can result in
 * better codegen on most architectures as the 1st arg can now be passed in registers.
 */
#define _SN_DECLARE_NJSON_FUNCTIONS_BY_VALUE(TYPE, ... /* TAGS */) \
    [[nodiscard]] bool try_to_njson(TYPE src, nlohmann::json *dst __VA_OPT__(,) __VA_ARGS__) noexcept; \
    void to_njson(TYPE src, nlohmann::json *dst __VA_OPT__(,) __VA_ARGS__); \
    [[nodiscard]] bool try_from_njson(const nlohmann::json &src, TYPE *dst __VA_OPT__(,) __VA_ARGS__) noexcept; \
    void from_njson(const nlohmann::json &src, TYPE *dst __VA_OPT__(,) __VA_ARGS__);

/**
 * @internal
 *
 * Unlike the `DECLARE_*` macros, this macro defines `nlohmann::json` functions for `TYPE` that just ignore the provided `TAG`,
 * effectively shifting the tag sequence by a single position to the left.
 *
 * Note that this macro will only work when invoked from the `sn::detail::builtins` namespace because it's calling
 * `nlohmann::json` functions directly (e.g. calling `to_njson` instead of `sn::to_njson`).
 */
#define _SN_DEFINE_INLINE_NJSON_TAG_EATING_FUNCTIONS(TYPE, TAG) \
    template<class... Tags> \
    [[nodiscard]] inline bool try_to_njson(const TYPE &src, nlohmann::json *dst, TAG tag, Tags... tags) noexcept { \
        return try_to_njson(src, dst, tags...); \
    } \
    template<class... Tags> \
    inline void to_njson(const TYPE &src, nlohmann::json *dst, TAG tag, Tags... tags) { \
        return to_njson(src, dst, tags...); \
    } \
    template<class... Tags> \
    [[nodiscard]] inline bool try_from_njson(const nlohmann::json &src, TYPE *dst, TAG tag, Tags... tags) noexcept { \
        return try_from_njson(src, dst, tags...); \
    } \
    template<class... Tags> \
    inline void from_njson(const nlohmann::json &src, TYPE *dst, TAG tag, Tags... tags) { \
        return from_njson(src, dst, tags...); \
    }
