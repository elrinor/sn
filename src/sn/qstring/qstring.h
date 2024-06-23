#pragma once

#include "sn/core/tag.h"
#include "sn/qstring/detail/qstring_builtins.h"

#include "qstring_concepts.h"

namespace sn::detail {

template<class T, sn::concepts::tag... Tags>
consteval void validate_try_to_qstring() {
    if constexpr (sizeof...(Tags) == 0) {
        static_assert(sn::concepts::try_to_qstringable<T>,
                      "Type T is not supported, did you forget to declare `bool try_to_qstring(const T &src, QString *dst)` in T's namespace?");
    } else {
        static_assert(sn::concepts::try_to_qstringable<T, Tags...>,
                      "Type T with provided Tags is not supported, did you forget to declare `bool try_to_qstring(const T &src, QString *dst, Tags...)` in T's namespace?");
    }
}

template<class T, sn::concepts::tag... Tags>
consteval void validate_to_qstring() {
    if constexpr (sizeof...(Tags) == 0) {
        static_assert(sn::concepts::to_qstringable<T>,
                      "Type T is not supported, did you forget to declare `void to_qstring(const T &src, QString *dst)` in T's namespace?");
    } else {
        static_assert(sn::concepts::to_qstringable<T, Tags...>,
                      "Type T with provided Tags is not supported, did you forget to declare `void to_qstring(const T &src, QString *dst, Tags...)` in T's namespace?");
    }
}

template<class T, sn::concepts::tag... Tags>
consteval void validate_try_from_qstring() {
    if constexpr (sizeof...(Tags) == 0) {
        static_assert(sn::concepts::try_from_qstringable<T>,
                      "Type T is not supported, did you forget to declare `bool try_from_qstring(QStringView src, T *dst)` in T's namespace?");
    } else {
        static_assert(sn::concepts::try_from_qstringable<T, Tags...>,
                      "Type T with provided Tags is not supported, did you forget to declare `bool try_from_qstring(QStringView src, T *dst, Tags...)` in T's namespace?");
    }
}

template<class T, sn::concepts::tag... Tags>
consteval void validate_from_qstring() {
    if constexpr (sizeof...(Tags) == 0) {
        static_assert(sn::concepts::from_qstringable<T>,
                      "Type T is not supported, did you forget to declare `void from_qstring(QStringView src, T *dst)` in T's namespace?");
    } else {
        static_assert(sn::concepts::from_qstringable<T, Tags...>,
                      "Type T with provided Tags is not supported, did you forget to declare `void from_qstring(QStringView src, T *dst, Tags...)` in T's namespace?");
    }
}

template<class T, sn::concepts::tag... Tags>
[[nodiscard]] bool do_try_to_qstring(const T &src, QString *dst, Tags... tags) noexcept {
    validate_try_to_qstring<T, Tags...>();
    using sn::detail::builtins::try_to_qstring;
    return try_to_qstring(src, dst, tags...);
}

template<class T, sn::concepts::tag... Tags>
void do_to_qstring(const T &src, QString *dst, Tags... tags) {
    validate_to_qstring<T, Tags...>();
    using sn::detail::builtins::to_qstring;
    to_qstring(src, dst, tags...);
}

template<class T, sn::concepts::tag... Tags>
[[nodiscard]] bool do_try_from_qstring(QStringView src, T *dst, Tags... tags) noexcept {
    validate_try_from_qstring<T, Tags...>();
    using sn::detail::builtins::try_from_qstring;
    return try_from_qstring(src, dst, tags...);
}

template<class T, sn::concepts::tag... Tags>
void do_from_qstring(QStringView src, T *dst, Tags... tags) {
    validate_from_qstring<T, Tags...>();
    using sn::detail::builtins::from_qstring;
    from_qstring(src, dst, tags...);
}

} // namespace sn::detail

namespace sn {

template<class T, sn::concepts::tag... Tags>
[[nodiscard]] bool try_to_qstring(const T &src, QString *dst, Tags... tags) noexcept {
    return sn::detail::do_try_to_qstring(src, dst, tags...);
}

template<class T, sn::concepts::tag... Tags>
void to_qstring(const T &src, QString *dst, Tags... tags) {
    sn::detail::do_to_qstring(src, dst, tags...);
}

template<class T, sn::concepts::tag... Tags>
[[nodiscard]] QString to_qstring(const T &src, Tags... tags) {
    QString result;
    sn::detail::do_to_qstring(src, &result, tags...);
    return result;
}

template<class T, sn::concepts::tag... Tags>
[[nodiscard]] bool try_from_qstring(QStringView src, T *dst, Tags... tags) noexcept {
    return sn::detail::do_try_from_qstring(src, dst, tags...);
}

template<class T, sn::concepts::tag... Tags>
void from_qstring(QStringView src, T *dst, Tags... tags) {
    sn::detail::do_from_qstring(src, dst, tags...);
}

template<class T, sn::concepts::tag... Tags>
[[nodiscard]] T from_qstring(QStringView src, Tags... tags) {
    T result;
    sn::detail::do_from_qstring(src, &result, tags...);
    return result;
}

} // namespace sn
