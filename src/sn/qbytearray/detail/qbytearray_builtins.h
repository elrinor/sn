#pragma once

#include <QtCore/QByteArray>
#include <QtCore/QByteArrayView>

#include "sn/qbytearray/qbytearray_fwd.h"
#include "sn/qbytearray/qbytearray_tags.h"

namespace sn::detail::builtins {

//
// Support for QByteArray.
//

[[nodiscard]] inline bool try_to_qbytearray(const QByteArray &src, QByteArray *dst) noexcept {
    *dst = src;
    return true;
}

inline void to_qbytearray(const QByteArray &src, QByteArray *dst) {
    *dst = src;
}

[[nodiscard]] inline bool try_from_qbytearray(QByteArrayView src, QByteArray *dst) noexcept {
    *dst = src.toByteArray();
    return true;
}

inline void from_qbytearray(QByteArrayView src, QByteArray *dst) {
    *dst = src.toByteArray();
}


//
// Support for QByteArrayView, to_qbytearray only.
//

[[nodiscard]] inline bool try_to_qbytearray(QByteArrayView src, QByteArray *dst) noexcept {
    *dst = src.toByteArray();
    return true;
}

inline void to_qbytearray(QByteArrayView src, QByteArray *dst) {
    *dst = src.toByteArray();
}


//
// Support for const char[N], to_qbytearray only.
//

template<std::size_t N>
[[nodiscard]] inline bool try_to_qbytearray(const char (&src)[N], QByteArray *dst) noexcept {
    *dst = src;
    return true;
}

template<std::size_t N>
inline void to_qbytearray(const char (&src)[N], QByteArray *dst) {
    *dst = src;
}


//
// Support for const char *, to_qbytearray only.
//

[[nodiscard]] inline bool try_to_qbytearray(const char *src, QByteArray *dst) noexcept {
    *dst = src;
    return true;
}

inline void to_qbytearray(const char *src, QByteArray *dst) {
    *dst = src;
}


//
// Support for char *, to_qbytearray only.
//

[[nodiscard]] inline bool try_to_qbytearray(char *src, QByteArray *dst) noexcept {
    *dst = src;
    return true;
}

inline void to_qbytearray(char *src, QByteArray *dst) {
    *dst = src;
}


//
// Support for arithmetic types.
//
// No support for char / unsigned char / signed char here, as it's not clear what the default behavior should be.
//

_SN_DECLARE_QBYTEARRAY_FUNCTIONS_BY_VALUE(bool)

_SN_DECLARE_QBYTEARRAY_FUNCTIONS_BY_VALUE(short)
_SN_DECLARE_QBYTEARRAY_FUNCTIONS_BY_VALUE(unsigned short)
_SN_DECLARE_QBYTEARRAY_FUNCTIONS_BY_VALUE(int)
_SN_DECLARE_QBYTEARRAY_FUNCTIONS_BY_VALUE(unsigned int)
_SN_DECLARE_QBYTEARRAY_FUNCTIONS_BY_VALUE(long)
_SN_DECLARE_QBYTEARRAY_FUNCTIONS_BY_VALUE(unsigned long)
_SN_DECLARE_QBYTEARRAY_FUNCTIONS_BY_VALUE(long long)
_SN_DECLARE_QBYTEARRAY_FUNCTIONS_BY_VALUE(unsigned long long)

_SN_DECLARE_QBYTEARRAY_FUNCTIONS_BY_VALUE(float)
_SN_DECLARE_QBYTEARRAY_FUNCTIONS_BY_VALUE(double)

_SN_DECLARE_QBYTEARRAY_FUNCTIONS_BY_VALUE(short, sn::tags::dynamic_base_tag)
_SN_DECLARE_QBYTEARRAY_FUNCTIONS_BY_VALUE(unsigned short, sn::tags::dynamic_base_tag)
_SN_DECLARE_QBYTEARRAY_FUNCTIONS_BY_VALUE(int, sn::tags::dynamic_base_tag)
_SN_DECLARE_QBYTEARRAY_FUNCTIONS_BY_VALUE(unsigned int, sn::tags::dynamic_base_tag)
_SN_DECLARE_QBYTEARRAY_FUNCTIONS_BY_VALUE(long, sn::tags::dynamic_base_tag)
_SN_DECLARE_QBYTEARRAY_FUNCTIONS_BY_VALUE(unsigned long, sn::tags::dynamic_base_tag)
_SN_DECLARE_QBYTEARRAY_FUNCTIONS_BY_VALUE(long long, sn::tags::dynamic_base_tag)
_SN_DECLARE_QBYTEARRAY_FUNCTIONS_BY_VALUE(unsigned long long, sn::tags::dynamic_base_tag)

_SN_DECLARE_QBYTEARRAY_FUNCTIONS_BY_VALUE(short, sn::tags::base_tag<2>)
_SN_DECLARE_QBYTEARRAY_FUNCTIONS_BY_VALUE(unsigned short, sn::tags::base_tag<2>)
_SN_DECLARE_QBYTEARRAY_FUNCTIONS_BY_VALUE(int, sn::tags::base_tag<2>)
_SN_DECLARE_QBYTEARRAY_FUNCTIONS_BY_VALUE(unsigned int, sn::tags::base_tag<2>)
_SN_DECLARE_QBYTEARRAY_FUNCTIONS_BY_VALUE(long, sn::tags::base_tag<2>)
_SN_DECLARE_QBYTEARRAY_FUNCTIONS_BY_VALUE(unsigned long, sn::tags::base_tag<2>)
_SN_DECLARE_QBYTEARRAY_FUNCTIONS_BY_VALUE(long long, sn::tags::base_tag<2>)
_SN_DECLARE_QBYTEARRAY_FUNCTIONS_BY_VALUE(unsigned long long, sn::tags::base_tag<2>)

_SN_DECLARE_QBYTEARRAY_FUNCTIONS_BY_VALUE(short, sn::tags::base_tag<8>)
_SN_DECLARE_QBYTEARRAY_FUNCTIONS_BY_VALUE(unsigned short, sn::tags::base_tag<8>)
_SN_DECLARE_QBYTEARRAY_FUNCTIONS_BY_VALUE(int, sn::tags::base_tag<8>)
_SN_DECLARE_QBYTEARRAY_FUNCTIONS_BY_VALUE(unsigned int, sn::tags::base_tag<8>)
_SN_DECLARE_QBYTEARRAY_FUNCTIONS_BY_VALUE(long, sn::tags::base_tag<8>)
_SN_DECLARE_QBYTEARRAY_FUNCTIONS_BY_VALUE(unsigned long, sn::tags::base_tag<8>)
_SN_DECLARE_QBYTEARRAY_FUNCTIONS_BY_VALUE(long long, sn::tags::base_tag<8>)
_SN_DECLARE_QBYTEARRAY_FUNCTIONS_BY_VALUE(unsigned long long, sn::tags::base_tag<8>)

_SN_DECLARE_QBYTEARRAY_FUNCTIONS_BY_VALUE(short, sn::tags::base_tag<16>)
_SN_DECLARE_QBYTEARRAY_FUNCTIONS_BY_VALUE(unsigned short, sn::tags::base_tag<16>)
_SN_DECLARE_QBYTEARRAY_FUNCTIONS_BY_VALUE(int, sn::tags::base_tag<16>)
_SN_DECLARE_QBYTEARRAY_FUNCTIONS_BY_VALUE(unsigned int, sn::tags::base_tag<16>)
_SN_DECLARE_QBYTEARRAY_FUNCTIONS_BY_VALUE(long, sn::tags::base_tag<16>)
_SN_DECLARE_QBYTEARRAY_FUNCTIONS_BY_VALUE(unsigned long, sn::tags::base_tag<16>)
_SN_DECLARE_QBYTEARRAY_FUNCTIONS_BY_VALUE(long long, sn::tags::base_tag<16>)
_SN_DECLARE_QBYTEARRAY_FUNCTIONS_BY_VALUE(unsigned long long, sn::tags::base_tag<16>)

#define _SN_DEFINE_TAG_IGNORING_INLINE_QBYTEARRAY_FUNCTIONS_BY_VALUE(TYPE, ... /* TAGS */)                                 \
    inline bool try_to_qbytearray(TYPE src, QString *dst __VA_OPT__(,) __VA_ARGS__) noexcept {                             \
        return try_to_qbytearray(src, dst);                                                                                \
    }                                                                                                                   \
    inline bool try_from_qbytearray(QStringView src, TYPE *dst __VA_OPT__(,) __VA_ARGS__) noexcept {                       \
        return try_from_qbytearray(src, dst);                                                                              \
    }                                                                                                                   \
    inline void to_qbytearray(TYPE src, QString *dst __VA_OPT__(,) __VA_ARGS__) {                                          \
        to_qbytearray(src, dst);                                                                                           \
    }                                                                                                                   \
    inline void from_qbytearray(QStringView src, TYPE *dst __VA_OPT__(,) __VA_ARGS__) {                                    \
        from_qbytearray(src, dst);                                                                                         \
    }

// TODO(elric): just add domains, tag traits, sn::is_ignored_tag<domain, tag> => T/F?

_SN_DEFINE_TAG_IGNORING_INLINE_QBYTEARRAY_FUNCTIONS_BY_VALUE(short, sn::tags::base_tag<10>)
_SN_DEFINE_TAG_IGNORING_INLINE_QBYTEARRAY_FUNCTIONS_BY_VALUE(unsigned short, sn::tags::base_tag<10>)
_SN_DEFINE_TAG_IGNORING_INLINE_QBYTEARRAY_FUNCTIONS_BY_VALUE(int, sn::tags::base_tag<10>)
_SN_DEFINE_TAG_IGNORING_INLINE_QBYTEARRAY_FUNCTIONS_BY_VALUE(unsigned int, sn::tags::base_tag<10>)
_SN_DEFINE_TAG_IGNORING_INLINE_QBYTEARRAY_FUNCTIONS_BY_VALUE(long, sn::tags::base_tag<10>)
_SN_DEFINE_TAG_IGNORING_INLINE_QBYTEARRAY_FUNCTIONS_BY_VALUE(unsigned long, sn::tags::base_tag<10>)
_SN_DEFINE_TAG_IGNORING_INLINE_QBYTEARRAY_FUNCTIONS_BY_VALUE(long long, sn::tags::base_tag<10>)
_SN_DEFINE_TAG_IGNORING_INLINE_QBYTEARRAY_FUNCTIONS_BY_VALUE(unsigned long long, sn::tags::base_tag<10>)

} // namespace sn::detail::builtins
