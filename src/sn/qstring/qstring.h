#pragma once

#include <QtCore/QString>
#include <QtCore/QStringView>

#include "sn/core/tags.h"
#include "sn/qstring/detail/qstring_builtins.h"

#include "qstring_concepts.h"

namespace sn::detail {

template<class T, sn::any_tag... Tags>
[[nodiscard]] bool do_try_to_qstring(const T &src, QString *dst, Tags... tags) noexcept {
    static_assert(qstringable<T, Tags...>, "Type T is not supported, did you forget to invoke SN_DECLARE_QSTRING_FUNCTIONS?");
    using sn::builtins::try_to_qstring;
    return try_to_qstring(src, dst, tags...);
}

template<class T, sn::any_tag... Tags>
void do_to_qstring(const T &src, QString *dst, Tags... tags) {
    static_assert(qstringable<T, Tags...>, "Type T is not supported, did you forget to invoke SN_DECLARE_QSTRING_FUNCTIONS?");
    using sn::builtins::to_qstring;
    to_qstring(src, dst, tags...);
}

template<class T, sn::any_tag... Tags>
[[nodiscard]] bool do_try_from_qstring(QStringView src, T *dst, Tags... tags) noexcept {
    static_assert(qstringable<T, Tags...>, "Type T is not supported, did you forget to invoke SN_DECLARE_QSTRING_FUNCTIONS?");
    using sn::builtins::try_from_qstring;
    return try_from_qstring(src, dst, tags...);
}

template<class T, sn::any_tag... Tags>
void do_from_qstring(QStringView src, T *dst, Tags... tags) {
    static_assert(qstringable<T, Tags...>, "Type T is not supported, did you forget to invoke SN_DECLARE_QSTRING_FUNCTIONS?");
    using sn::builtins::from_qstring;
    from_qstring(src, dst, tags...);
}

} // namespace sn::detail

namespace sn::niebloids {

struct to_qstring {
    template<class T, sn::any_tag... Tags>
    [[nodiscard]] QString operator()(const T &src, Tags... tags) const {
        QString result;
        sn::detail::do_to_qstring(src, &result, tags...);
        return result;
    }
};

template<class T>
struct from_qstring {
    template<sn::any_tag... Tags>
    [[nodiscard]] T operator()(QStringView src, Tags... tags) const {
        T result;
        sn::detail::do_from_qstring(src, &result, tags...);
        return result;
    }
};

} // namespace sn::niebloids

namespace sn {

inline constexpr sn::niebloids::to_qstring to_qstring_v;
template<class T>
inline constexpr sn::niebloids::from_qstring<T> from_qstring_v;

template<class T, sn::any_tag... Tags>
[[nodiscard]] bool try_to_qstring(const T &src, QString *dst, Tags... tags) noexcept {
    return sn::detail::do_try_to_qstring(src, dst, tags...);
}

template<class T, sn::any_tag... Tags>
void to_qstring(const T &src, QString *dst, Tags... tags) {
    sn::detail::do_to_qstring(src, dst, tags...);
}

template<class T, sn::any_tag... Tags>
[[nodiscard]] QString to_qstring(const T &src, Tags... tags) {
    QString result;
    sn::detail::do_to_qstring(src, &result, tags...);
    return result;
}

template<class T, sn::any_tag... Tags>
[[nodiscard]] bool try_from_qstring(QStringView src, T *dst, Tags... tags) noexcept {
    return sn::detail::do_try_from_qstring(src, dst, tags...);
}

template<class T, sn::any_tag... Tags>
void from_qstring(QStringView src, T *dst, Tags... tags) {
    sn::detail::do_from_qstring(src, dst, tags...);
}

template<class T, sn::any_tag... Tags>
[[nodiscard]] T from_qstring(QStringView src, Tags... tags) {
    T result;
    sn::detail::do_from_qstring(src, &result, tags...);
    return result;
}

} // namespace sn
