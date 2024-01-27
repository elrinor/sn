#include "qstring_builtins.h"

#include "qstring_exceptions.h"

namespace sn::builtins {

//
// bool.
//

bool try_to_qstring(const bool &src, QString *dst, tn::detail::explicit_type_tag) noexcept {
    *dst = src ? QStringLiteral("true") : QStringLiteral("false");
    return true;
}

bool try_from_qstring(QStringView src, bool *dst, tn::detail::explicit_type_tag) noexcept {
    if (src == QStringLiteral("true") || src == QStringLiteral("1")) {
        *dst = true;
        return true;
    } else if (src == QStringLiteral("false") || src == QStringLiteral("0")) {
        *dst = false;
        return true;
    } else {
        return false;
    }
}

void to_qstring(const bool &src, QString *dst, tn::detail::explicit_type_tag) {
    (void) try_to_qstring(src, dst); // Always succeeds.
}

void from_qstring(QStringView src, bool *dst, tn::detail::explicit_type_tag) {
    if (!try_from_qstring(src, dst))
        throw_from_string_error<bool>(src);
}


//
// Arithmetic types.
//

namespace detail_qt {
template<class T>
inline T qstringview_to(QStringView src, bool *ok);

#define SN_DEFINE_QSTRINGVIEW_TO(TYPE, METHOD_NAME)                                                                     \
template<>                                                                                                              \
inline TYPE qstringview_to<TYPE>(QStringView src, bool *ok) {                                                           \
    return src.METHOD_NAME(ok);                                                                                         \
}

SN_DEFINE_QSTRINGVIEW_TO(short, toShort)
SN_DEFINE_QSTRINGVIEW_TO(unsigned short, toUShort)
SN_DEFINE_QSTRINGVIEW_TO(int, toInt)
SN_DEFINE_QSTRINGVIEW_TO(unsigned int, toUInt)
SN_DEFINE_QSTRINGVIEW_TO(long, toLong)
SN_DEFINE_QSTRINGVIEW_TO(unsigned long, toULong)
SN_DEFINE_QSTRINGVIEW_TO(long long, toLongLong)
SN_DEFINE_QSTRINGVIEW_TO(unsigned long long, toULongLong)
SN_DEFINE_QSTRINGVIEW_TO(float, toFloat)
SN_DEFINE_QSTRINGVIEW_TO(double, toDouble)

template<class T>
inline bool try_to_qstring(const T &src, QString *dst) noexcept {
    *dst = QString::number(src);
    return true;
}

template<class T>
inline void to_qstring(const T &src, QString *dst) {
    (void) try_to_qstring(src, dst);
}

template<class T>
inline bool try_from_qstring(QStringView src, T *dst) noexcept {
    // Qt number handling doesn't match what we have in sn::from_string, and we want all our functions to work the same.
    // What's wrong with the Qt code:
    // - Qt skips leading whitespaces.
    // - Qt handles '+' prefix.
    // - Qt handles 0x and 0b prefixes.
    if (!src.empty() && !src[0].isDigit() && src[0] != QLatin1Char('-'))
        return false;
    if (src.size() >= 2 && !src[1].isDigit())
        return false;

    bool ok;
    *dst = qstringview_to<T>(src, &ok);
    return ok;
}

template<class T>
inline void from_qstring(QStringView src, T *dst) {
    if (!try_from_qstring(src, dst))
        throw_from_string_error<T>(src);
}

} // namespace detail_qt

#define SN_DEFINE_PROXY_QSTRING_FUNCTIONS(TYPE)                                                                         \
bool try_to_qstring(const TYPE &src, QString *dst) noexcept { return detail_qt::try_to_qstring(src, dst); }              \
bool try_from_qstring(QStringView src, TYPE *dst) noexcept { return detail_qt::try_from_qstring(src, dst); }             \
void to_qstring(const TYPE &src, QString *dst) { detail_qt::to_qstring(src, dst); }                                      \
void from_qstring(QStringView src, TYPE *dst) { detail_qt::from_qstring(src, dst); }

SN_DEFINE_PROXY_QSTRING_FUNCTIONS(short)
SN_DEFINE_PROXY_QSTRING_FUNCTIONS(unsigned short)
SN_DEFINE_PROXY_QSTRING_FUNCTIONS(int)
SN_DEFINE_PROXY_QSTRING_FUNCTIONS(unsigned int)
SN_DEFINE_PROXY_QSTRING_FUNCTIONS(long)
SN_DEFINE_PROXY_QSTRING_FUNCTIONS(unsigned long)
SN_DEFINE_PROXY_QSTRING_FUNCTIONS(long long)
SN_DEFINE_PROXY_QSTRING_FUNCTIONS(unsigned long long)
SN_DEFINE_PROXY_QSTRING_FUNCTIONS(float)
SN_DEFINE_PROXY_QSTRING_FUNCTIONS(double)

} // namespace sn::builtins
