//
// This header is auto-generated using the headergen tool in /tools.
//
#pragma once

#include "sn/core/preprocessor.h" // For __VA_OPT__.

class QString;
class QStringView;

/**
 * @internal
 *
 * Same as `SN_DECLARE_QSTRING_FUNCTIONS`, but `to_qstring` and `try_to_qstring` take `TYPE` by value. This can result in
 * better codegen on most architectures as the 1st arg can now be passed in registers.
 */
#define _SN_DECLARE_QSTRING_FUNCTIONS_BY_VALUE(TYPE, ... /* TAGS */) \
    [[nodiscard]] bool try_to_qstring(TYPE src, QString *dst __VA_OPT__(,) __VA_ARGS__) noexcept; \
    void to_qstring(TYPE src, QString *dst __VA_OPT__(,) __VA_ARGS__); \
    [[nodiscard]] bool try_from_qstring(QStringView src, TYPE *dst __VA_OPT__(,) __VA_ARGS__) noexcept; \
    void from_qstring(QStringView src, TYPE *dst __VA_OPT__(,) __VA_ARGS__);

/**
 * @internal
 *
 * Unlike the `DECLARE_*` macros, this macro defines `QString` functions for `TYPE` that just ignore the provided `TAG`,
 * effectively shifting the tag sequence by a single position to the left.
 *
 * Note that this macro will only work when invoked from the `sn::detail::builtins` namespace because it's calling
 * `QString` functions directly (e.g. calling `to_qstring` instead of `sn::to_qstring`).
 */
#define _SN_DEFINE_INLINE_QSTRING_TAG_EATING_FUNCTIONS(TYPE, TAG) \
    template<class... Tags> \
    [[nodiscard]] inline bool try_to_qstring(const TYPE &src, QString *dst, TAG tag, Tags... tags) noexcept { \
        return try_to_qstring(src, dst, tags...); \
    } \
    template<class... Tags> \
    inline void to_qstring(const TYPE &src, QString *dst, TAG tag, Tags... tags) { \
        return to_qstring(src, dst, tags...); \
    } \
    template<class... Tags> \
    [[nodiscard]] inline bool try_from_qstring(QStringView src, TYPE *dst, TAG tag, Tags... tags) noexcept { \
        return try_from_qstring(src, dst, tags...); \
    } \
    template<class... Tags> \
    inline void from_qstring(QStringView src, TYPE *dst, TAG tag, Tags... tags) { \
        return from_qstring(src, dst, tags...); \
    }
