#include "qstring_builtins.h"

#include <cmath> // For std::signbit.

#include "sn/detail/preprocessor/preprocessor.h"

#include "qstring_exceptions.h"

namespace sn::detail::builtins {

//
// bool.
//

bool try_to_qstring(bool src, QString *dst) noexcept {
    *dst = src ? QStringLiteral("true") : QStringLiteral("false");
    return true;
}

bool try_from_qstring(QStringView src, bool *dst) noexcept {
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

void to_qstring(bool src, QString *dst) {
    (void) try_to_qstring(src, dst); // Always succeeds.
}

void from_qstring(QStringView src, bool *dst) {
    if (!try_from_qstring(src, dst))
        sn::detail::throw_from_string_error<bool>(src);
}


//
// Arithmetic types.
//

namespace detail_qt {
#define SN_DEFINE_WRAPPED_QSTRINGVIEW_TO(TYPE, METHOD_NAME)                                                             \
template<class... Args>                                                                                                 \
inline void wrapped_qstringview_to(QStringView src, TYPE *dst, bool *ok, Args... args) {                                \
    *dst = src.METHOD_NAME(ok, args...);                                                                                \
}                                                                                                                       \

SN_DEFINE_WRAPPED_QSTRINGVIEW_TO(short, toShort)
SN_DEFINE_WRAPPED_QSTRINGVIEW_TO(unsigned short, toUShort)
SN_DEFINE_WRAPPED_QSTRINGVIEW_TO(int, toInt)
SN_DEFINE_WRAPPED_QSTRINGVIEW_TO(unsigned int, toUInt)
SN_DEFINE_WRAPPED_QSTRINGVIEW_TO(long, toLong)
SN_DEFINE_WRAPPED_QSTRINGVIEW_TO(unsigned long, toULong)
SN_DEFINE_WRAPPED_QSTRINGVIEW_TO(long long, toLongLong)
SN_DEFINE_WRAPPED_QSTRINGVIEW_TO(unsigned long long, toULongLong)
SN_DEFINE_WRAPPED_QSTRINGVIEW_TO(float, toFloat)
SN_DEFINE_WRAPPED_QSTRINGVIEW_TO(double, toDouble)

template<class T>
inline void wrapped_qstringview_to_ex(QStringView src, T *dst, bool *ok) {
    wrapped_qstringview_to(src, dst, ok);
}

template<class T>
inline void wrapped_qstringview_to_ex(QStringView src, T *dst, bool *ok, sn::dynamic_base_tag base) {
    wrapped_qstringview_to(src, dst, ok, base.value());
}

template<class T, int base>
inline void wrapped_qstringview_to_ex(QStringView src, T *dst, bool *ok, sn::base_tag<base>) {
    wrapped_qstringview_to(src, dst, ok, base);
}

template<class T>
inline QString wrapped_qstring_number(T value) {
    return QString::number(value);
}

template<class T>
inline QString wrapped_qstring_number(T value, sn::dynamic_base_tag base) {
    return QString::number(value, base.value());
}

template<class T, int base>
inline QString wrapped_qstring_number(T value, sn::base_tag<base>) {
    return QString::number(value, base);
}

template<class T, class... Tags>
inline bool try_to_qstring(T src, QString *dst, Tags... tags) noexcept {
    // Qt doesn't round-trip negative zero, while std functions do. We want to be consistent with std functions.
    if constexpr (std::is_floating_point_v<T>) {
        if (src == 0 && std::signbit(src)) {
            *dst = QStringLiteral("-0");
            return true;
        }
    }

    *dst = wrapped_qstring_number(src, tags...);
    return true;
}

template<class T, class... Tags>
inline void to_qstring(T src, QString *dst, Tags... tags) {
    (void) try_to_qstring(src, dst, tags...);
}

template<class T, class... Tags>
inline bool try_from_qstring(QStringView src, T *dst, Tags... tags) noexcept {
    // Qt number handling doesn't match what we have in sn::from_string, and we want all our functions to work the same.
    // What's wrong with the Qt code:
    // - Qt skips leading and trailing whitespaces.
    // - Qt handles '+' prefix.
    // - Qt handles 0x and 0b prefixes for integers.
    if (src.empty())
        return false;

    if constexpr (std::is_floating_point_v<T>) {
        if (src.front().isSpace() || src.front() == QLatin1Char('+'))
            return false; // Leading whitespaces / '+' prefix.
        if (src.back().isSpace())
            return false; // Trailing whitespaces.
    } else {
        if (src.front().isSpace() || src.front() == QLatin1Char('+'))
            return false; // Leading whitespaces / '+' prefix.
        if (src.back().isSpace())
            return false; // Trailing whitespaces.
        if (src.size() >= 2 && !src[1].isDigit())
            return false; // 0x and 0b prefixes.
    }

    bool result;
    wrapped_qstringview_to_ex(src, dst, &result, tags...);
    return result;
}

template<class T, class... Tags>
inline void from_qstring(QStringView src, T *dst, Tags... tags) {
    if (!try_from_qstring(src, dst, tags...))
        sn::detail::throw_from_string_error<T>(src);
}

} // namespace detail_qt

#define SN_DEFINE_NUMERIC_QSTRING_FUNCTIONS(TYPE, ... /* TAGS */)                                                       \
    SN_DEFINE_NUMERIC_QSTRING_FUNCTIONS_I(TYPE, _SN_PP_TUPLE_TYPES_TO_DECL_PARAMS(arg, (__VA_ARGS__)), _SN_PP_TUPLE_TYPES_TO_CALL_PARAMS(arg, (__VA_ARGS__)))
#define SN_DEFINE_NUMERIC_QSTRING_FUNCTIONS_I(TYPE, DECL_PARAMS, CALL_PARAMS)                                           \
    bool try_to_qstring(TYPE src, QString *dst SN_PP_TUPLE_ENUM_TRAILING(DECL_PARAMS)) noexcept {                       \
        return detail_qt::try_to_qstring(src, dst SN_PP_TUPLE_ENUM_TRAILING(CALL_PARAMS));                              \
    }                                                                                                                   \
    bool try_from_qstring(QStringView src, TYPE *dst SN_PP_TUPLE_ENUM_TRAILING(DECL_PARAMS)) noexcept {                 \
        return detail_qt::try_from_qstring(src, dst SN_PP_TUPLE_ENUM_TRAILING(CALL_PARAMS));                            \
    }                                                                                                                   \
    void to_qstring(TYPE src, QString *dst SN_PP_TUPLE_ENUM_TRAILING(DECL_PARAMS)) {                                    \
        detail_qt::to_qstring(src, dst SN_PP_TUPLE_ENUM_TRAILING(CALL_PARAMS));                                         \
    }                                                                                                                   \
    void from_qstring(QStringView src, TYPE *dst SN_PP_TUPLE_ENUM_TRAILING(DECL_PARAMS)) {                              \
        detail_qt::from_qstring(src, dst SN_PP_TUPLE_ENUM_TRAILING(CALL_PARAMS));                                       \
    }

SN_DEFINE_NUMERIC_QSTRING_FUNCTIONS(short)
SN_DEFINE_NUMERIC_QSTRING_FUNCTIONS(unsigned short)
SN_DEFINE_NUMERIC_QSTRING_FUNCTIONS(int)
SN_DEFINE_NUMERIC_QSTRING_FUNCTIONS(unsigned int)
SN_DEFINE_NUMERIC_QSTRING_FUNCTIONS(long)
SN_DEFINE_NUMERIC_QSTRING_FUNCTIONS(unsigned long)
SN_DEFINE_NUMERIC_QSTRING_FUNCTIONS(long long)
SN_DEFINE_NUMERIC_QSTRING_FUNCTIONS(unsigned long long)

SN_DEFINE_NUMERIC_QSTRING_FUNCTIONS(float)
SN_DEFINE_NUMERIC_QSTRING_FUNCTIONS(double)

SN_DEFINE_NUMERIC_QSTRING_FUNCTIONS(short, sn::dynamic_base_tag)
SN_DEFINE_NUMERIC_QSTRING_FUNCTIONS(unsigned short, sn::dynamic_base_tag)
SN_DEFINE_NUMERIC_QSTRING_FUNCTIONS(int, sn::dynamic_base_tag)
SN_DEFINE_NUMERIC_QSTRING_FUNCTIONS(unsigned int, sn::dynamic_base_tag)
SN_DEFINE_NUMERIC_QSTRING_FUNCTIONS(long, sn::dynamic_base_tag)
SN_DEFINE_NUMERIC_QSTRING_FUNCTIONS(unsigned long, sn::dynamic_base_tag)
SN_DEFINE_NUMERIC_QSTRING_FUNCTIONS(long long, sn::dynamic_base_tag)
SN_DEFINE_NUMERIC_QSTRING_FUNCTIONS(unsigned long long, sn::dynamic_base_tag)

SN_DEFINE_NUMERIC_QSTRING_FUNCTIONS(short, sn::base_tag<2>)
SN_DEFINE_NUMERIC_QSTRING_FUNCTIONS(unsigned short, sn::base_tag<2>)
SN_DEFINE_NUMERIC_QSTRING_FUNCTIONS(int, sn::base_tag<2>)
SN_DEFINE_NUMERIC_QSTRING_FUNCTIONS(unsigned int, sn::base_tag<2>)
SN_DEFINE_NUMERIC_QSTRING_FUNCTIONS(long, sn::base_tag<2>)
SN_DEFINE_NUMERIC_QSTRING_FUNCTIONS(unsigned long, sn::base_tag<2>)
SN_DEFINE_NUMERIC_QSTRING_FUNCTIONS(long long, sn::base_tag<2>)
SN_DEFINE_NUMERIC_QSTRING_FUNCTIONS(unsigned long long, sn::base_tag<2>)

SN_DEFINE_NUMERIC_QSTRING_FUNCTIONS(short, sn::base_tag<8>)
SN_DEFINE_NUMERIC_QSTRING_FUNCTIONS(unsigned short, sn::base_tag<8>)
SN_DEFINE_NUMERIC_QSTRING_FUNCTIONS(int, sn::base_tag<8>)
SN_DEFINE_NUMERIC_QSTRING_FUNCTIONS(unsigned int, sn::base_tag<8>)
SN_DEFINE_NUMERIC_QSTRING_FUNCTIONS(long, sn::base_tag<8>)
SN_DEFINE_NUMERIC_QSTRING_FUNCTIONS(unsigned long, sn::base_tag<8>)
SN_DEFINE_NUMERIC_QSTRING_FUNCTIONS(long long, sn::base_tag<8>)
SN_DEFINE_NUMERIC_QSTRING_FUNCTIONS(unsigned long long, sn::base_tag<8>)

SN_DEFINE_NUMERIC_QSTRING_FUNCTIONS(short, sn::base_tag<16>)
SN_DEFINE_NUMERIC_QSTRING_FUNCTIONS(unsigned short, sn::base_tag<16>)
SN_DEFINE_NUMERIC_QSTRING_FUNCTIONS(int, sn::base_tag<16>)
SN_DEFINE_NUMERIC_QSTRING_FUNCTIONS(unsigned int, sn::base_tag<16>)
SN_DEFINE_NUMERIC_QSTRING_FUNCTIONS(long, sn::base_tag<16>)
SN_DEFINE_NUMERIC_QSTRING_FUNCTIONS(unsigned long, sn::base_tag<16>)
SN_DEFINE_NUMERIC_QSTRING_FUNCTIONS(long long, sn::base_tag<16>)
SN_DEFINE_NUMERIC_QSTRING_FUNCTIONS(unsigned long long, sn::base_tag<16>)

} // namespace sn::detail::builtins
