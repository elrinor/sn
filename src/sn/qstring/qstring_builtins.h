#pragma once

#include <concepts>

#include "sn/core/tags.h"

#include "qstring_fwd.h"

namespace sn::builtins {

//
// Support for QString / QStringView / char16_t * / wchar_t * (on Windows).
//

[[nodiscard]] inline bool try_to_qstring(QStringView src, QString *dst) noexcept {
    *dst = src.toString();
    return true;
}

inline void to_qstring(QStringView src, QString *dst) {
    *dst = src.toString();
}

[[nodiscard]] inline bool try_from_qstring(QStringView src, QString *dst) noexcept {
    *dst = src.toString();
    return true;
}

inline void from_qstring(QStringView src, QString *dst) {
    *dst = src.toString();
}


//
// Separate constrained overloads for `QString` b/c we want to keep the strings shared if possible. `from_string`
// API doesn't let us do that as it's taking a `QStringView`, and thus we're only overloading `to_string`.
//

template<std::same_as<QString> T>
[[nodiscard]] bool try_to_qstring(const T &src, QString *dst) noexcept {
    *dst = src;
    return true;
}

template<std::same_as<QString> T>
void to_qstring(const T &src, QString *dst) {
    *dst = src;
}


//
// Support for bool, ignoring builtin conversions. If we don't ignore them, then bool overload becomes a catch-all
// for all pointer types.
//

SN_DECLARE_QSTRING_FUNCTIONS(bool, tn::detail::explicit_type_tag)

template<std::same_as<bool> T>
[[nodiscard]] bool try_to_qstring(const T &src, QString *dst) noexcept {
    return try_to_qstring(src, dst, tn::detail::explicit_type);
}

template<std::same_as<bool> T>
void to_qstring(const T &src, QString *dst) {
    to_qstring(src, dst, tn::detail::explicit_type);
}

template<std::same_as<bool> T>
[[nodiscard]] bool try_from_qstring(QStringView src, T *dst) noexcept {
    return try_from_qstring(src, dst, tn::detail::explicit_type);
}

template<std::same_as<bool> T>
void from_qstring(QStringView src, T *dst) {
    from_qstring(src, dst, tn::detail::explicit_type);
}


//
// Support for arithmetic types.
//
// No support for char / unsigned char / signed char here, as it's not clear what the default behavior should be.
//

SN_DECLARE_QSTRING_FUNCTIONS(short)
SN_DECLARE_QSTRING_FUNCTIONS(unsigned short)
SN_DECLARE_QSTRING_FUNCTIONS(int)
SN_DECLARE_QSTRING_FUNCTIONS(unsigned int)
SN_DECLARE_QSTRING_FUNCTIONS(long)
SN_DECLARE_QSTRING_FUNCTIONS(unsigned long)
SN_DECLARE_QSTRING_FUNCTIONS(long long)
SN_DECLARE_QSTRING_FUNCTIONS(unsigned long long)
SN_DECLARE_QSTRING_FUNCTIONS(float)
SN_DECLARE_QSTRING_FUNCTIONS(double)

} // namespace sn::builtins
