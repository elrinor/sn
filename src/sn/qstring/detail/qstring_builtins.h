#pragma once

#include <concepts>

#include "sn/qstring/qstring_fwd.h"

namespace sn::builtins {

//
// Support for QString.
//

void is_qstring_supported_type(std::type_identity<QString>);

[[nodiscard]] inline bool try_to_qstring(const QString &src, QString *dst) noexcept {
    *dst = src;
    return true;
}

inline void to_qstring(const QString &src, QString *dst) {
    *dst = src;
}

[[nodiscard]] inline bool try_from_qstring(QStringView src, QString *dst) noexcept {
    *dst = src.toString();
    return true;
}

inline void from_qstring(QStringView src, QString *dst) {
    *dst = src.toString();
}


//
// Support for QStringView, to_qstring only.
//

void is_qstring_supported_type(std::type_identity<QStringView>);

[[nodiscard]] inline bool try_to_qstring(QStringView src, QString *dst) noexcept {
    *dst = src.toString();
    return true;
}

inline void to_qstring(QStringView src, QString *dst) {
    *dst = src.toString();
}

[[nodiscard]] inline bool try_from_qstring(QStringView src, QStringView *dst) noexcept = delete;
inline void from_qstring(QStringView src, QStringView *dst) = delete;


//
// Support for char16_t[N], to_string only.
//

template<std::size_t N>
void is_qstring_supported_type(std::type_identity<char16_t[N]>);

template<std::size_t N>
[[nodiscard]] inline bool try_to_qstring(const char16_t (&src)[N], QString *dst) noexcept {
    *dst = QString::fromUtf16(src);
    return true;
}

template<std::size_t N>
inline void to_qstring(const char16_t (&src)[N], QString *dst) {
    *dst = QString::fromUtf16(src);
}

template<std::size_t N>
[[nodiscard]] inline bool try_from_qstring(QStringView src, const char16_t (*dst)[N]) noexcept = delete;
template<std::size_t N>
inline void from_qstring(QStringView src, const char16_t (*dst)[N]) = delete;


//
// Support for const char16_t *, to_string only.
//

void is_qstring_supported_type(std::type_identity<const char16_t *>);

[[nodiscard]] inline bool try_to_qstring(const char16_t *src, QString *dst) noexcept {
    *dst = QString::fromUtf16(src);
    return true;
}

inline void to_qstring(const char16_t *src, QString *dst) {
    *dst = QString::fromUtf16(src);
}

[[nodiscard]] inline bool try_from_qstring(QStringView src, const char16_t **dst) noexcept = delete;
inline void from_qstring(QStringView src, const char16_t **dst) = delete;


//
// Note: we could have implemented wchar_t * and wchar_t[N] support here for Windows builds, but it makes very little
// sense. It's 2024 and wchar_t should rest in peace.
//


//
// Support for arithmetic types.
//
// No support for char / unsigned char / signed char here, as it's not clear what the default behavior should be.
//

_SN_DECLARE_QSTRING_FUNCTIONS_BY_VALUE(bool)
_SN_DECLARE_QSTRING_FUNCTIONS_BY_VALUE(short)
_SN_DECLARE_QSTRING_FUNCTIONS_BY_VALUE(unsigned short)
_SN_DECLARE_QSTRING_FUNCTIONS_BY_VALUE(int)
_SN_DECLARE_QSTRING_FUNCTIONS_BY_VALUE(unsigned int)
_SN_DECLARE_QSTRING_FUNCTIONS_BY_VALUE(long)
_SN_DECLARE_QSTRING_FUNCTIONS_BY_VALUE(unsigned long)
_SN_DECLARE_QSTRING_FUNCTIONS_BY_VALUE(long long)
_SN_DECLARE_QSTRING_FUNCTIONS_BY_VALUE(unsigned long long)
_SN_DECLARE_QSTRING_FUNCTIONS_BY_VALUE(float)
_SN_DECLARE_QSTRING_FUNCTIONS_BY_VALUE(double)

} // namespace sn::builtins