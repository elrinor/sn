//
// This header is auto-generated using the headergen tool in /tools.
//
#pragma once
@INCLUDES@
#include "sn/core/preprocessor.h" // For __VA_OPT__.
@DECLS@
/**
 * @internal
 *
 * Same as `SN_DECLARE_@UPPER@_FUNCTIONS`, but `to_@LOWER@` and `try_to_@LOWER@` take `TYPE` by value. This can result in
 * better codegen on most architectures as the 1st arg can now be passed in registers.
 */
#define _SN_DECLARE_@UPPER@_FUNCTIONS_BY_VALUE(TYPE, ... /* TAGS */) \
    [[nodiscard]] bool try_to_@LOWER@(TYPE src, @DST@ __VA_OPT__(,) __VA_ARGS__) noexcept; \
    void to_@LOWER@(TYPE src, @DST@ __VA_OPT__(,) __VA_ARGS__); \
    [[nodiscard]] bool try_from_@LOWER@(@SRC@, TYPE *dst __VA_OPT__(,) __VA_ARGS__) noexcept; \
    void from_@LOWER@(@SRC@, TYPE *dst __VA_OPT__(,) __VA_ARGS__);

/**
 * @internal
 *
 * Unlike the `DECLARE_*` macros, this macro defines `@TYPE@` functions for `TYPE` that just ignore the provided `TAG`,
 * effectively shifting the tag sequence by a single position to the left.
 */
#define _SN_DEFINE_INLINE_@UPPER@_TAG_EATING_FUNCTIONS(TYPE, TAG) \
    template<class... Tags> \
    [[nodiscard]] inline bool try_to_@LOWER@(const TYPE &src, @DST@, TAG tag, Tags... tags) noexcept { \
        return try_to_@LOWER@(src, dst, tags...); \
    } \
    template<class... Tags> \
    inline void to_@LOWER@(const TYPE &src, @DST@, TAG tag, Tags... tags) { \
        return to_@LOWER@(src, dst, tags...); \
    } \
    template<class... Tags> \
    [[nodiscard]] inline bool try_from_@LOWER@(@SRC@, TYPE *dst, TAG tag, Tags... tags) noexcept { \
        return try_from_@LOWER@(src, dst, tags...); \
    } \
    template<class... Tags> \
    inline void from_@LOWER@(@SRC@, TYPE *dst, TAG tag, Tags... tags) { \
        return from_@LOWER@(src, dst, tags...); \
    }
