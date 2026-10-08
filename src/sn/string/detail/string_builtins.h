#pragma once

#include <string>
#include <string_view>

#include "sn/string/string_fwd.h"
#include "sn/string/string_tags.h"

#include "string_shortcuts.h"

namespace sn::detail {

/**
 * Capacity of the small string buffer, which is what `std::string().capacity()` returns. Pinned by a test in
 * `string_ut.cpp`.
 */
#if defined(_LIBCPP_VERSION)
inline constexpr std::size_t small_string_capacity = 3 * sizeof(std::size_t) - 2; // 22 chars on 64-bit platforms, 10 on 32-bit.
#else
inline constexpr std::size_t small_string_capacity = 15; // libstdc++ and msvc.
#endif

} // namespace sn::detail

namespace sn::detail::builtins {

//
// Support for std::string.
//

[[nodiscard]] inline bool try_to_string(const std::string &src, std::string *dst) noexcept {
    *dst = src;
    return true;
}

inline void to_string(const std::string &src, std::string *dst) {
    *dst = src;
}

[[nodiscard]] inline bool try_from_string(std::string_view src, std::string *dst) noexcept {
    *dst = src;
    return true;
}

inline void from_string(std::string_view src, std::string *dst) {
    *dst = src;
}


//
// Support for std::string_view, to_string only.
//

[[nodiscard]] inline bool try_to_string(std::string_view src, std::string *dst) noexcept {
    *dst = src;
    return true;
}

inline void to_string(std::string_view src, std::string *dst) {
    *dst = src;
}


//
// Support for const char[N], to_string only.
//

template<std::size_t N>
[[nodiscard]] inline bool try_to_string(const char (&src)[N], std::string *dst) noexcept {
    *dst = src;
    return true;
}

template<std::size_t N>
inline void to_string(const char (&src)[N], std::string *dst) {
    *dst = src;
}


//
// Support for const char *, to_string only.
//

[[nodiscard]] inline bool try_to_string(const char *src, std::string *dst) noexcept {
    *dst = src;
    return true;
}

inline void to_string(const char *src, std::string *dst) {
    *dst = src;
}


//
// Support for char *, to_string only.
//

[[nodiscard]] inline bool try_to_string(char *src, std::string *dst) noexcept {
    *dst = src;
    return true;
}

inline void to_string(char *src, std::string *dst) {
    *dst = src;
}


//
// Support for arithmetic types.
//
// No support for char / unsigned char / signed char here, as it's not clear what the default behavior should be.
//

_SN_DECLARE_STRING_FUNCTIONS_BY_VALUE(bool)

_SN_DECLARE_STRING_FUNCTIONS_BY_VALUE(short)
_SN_DECLARE_STRING_FUNCTIONS_BY_VALUE(unsigned short)
_SN_DECLARE_STRING_FUNCTIONS_BY_VALUE(int)
_SN_DECLARE_STRING_FUNCTIONS_BY_VALUE(unsigned int)
_SN_DECLARE_STRING_FUNCTIONS_BY_VALUE(long)
_SN_DECLARE_STRING_FUNCTIONS_BY_VALUE(unsigned long)
_SN_DECLARE_STRING_FUNCTIONS_BY_VALUE(long long)
_SN_DECLARE_STRING_FUNCTIONS_BY_VALUE(unsigned long long)

_SN_DECLARE_STRING_FUNCTIONS_BY_VALUE(float)
_SN_DECLARE_STRING_FUNCTIONS_BY_VALUE(double)

_SN_DECLARE_STRING_FUNCTIONS_BY_VALUE(short, sn::tags::dynamic_base_tag)
_SN_DECLARE_STRING_FUNCTIONS_BY_VALUE(unsigned short, sn::tags::dynamic_base_tag)
_SN_DECLARE_STRING_FUNCTIONS_BY_VALUE(int, sn::tags::dynamic_base_tag)
_SN_DECLARE_STRING_FUNCTIONS_BY_VALUE(unsigned int, sn::tags::dynamic_base_tag)
_SN_DECLARE_STRING_FUNCTIONS_BY_VALUE(long, sn::tags::dynamic_base_tag)
_SN_DECLARE_STRING_FUNCTIONS_BY_VALUE(unsigned long, sn::tags::dynamic_base_tag)
_SN_DECLARE_STRING_FUNCTIONS_BY_VALUE(long long, sn::tags::dynamic_base_tag)
_SN_DECLARE_STRING_FUNCTIONS_BY_VALUE(unsigned long long, sn::tags::dynamic_base_tag)

_SN_DECLARE_STRING_FUNCTIONS_BY_VALUE(short, sn::tags::base_tag<2>)
_SN_DECLARE_STRING_FUNCTIONS_BY_VALUE(unsigned short, sn::tags::base_tag<2>)
_SN_DECLARE_STRING_FUNCTIONS_BY_VALUE(int, sn::tags::base_tag<2>)
_SN_DECLARE_STRING_FUNCTIONS_BY_VALUE(unsigned int, sn::tags::base_tag<2>)
_SN_DECLARE_STRING_FUNCTIONS_BY_VALUE(long, sn::tags::base_tag<2>)
_SN_DECLARE_STRING_FUNCTIONS_BY_VALUE(unsigned long, sn::tags::base_tag<2>)
_SN_DECLARE_STRING_FUNCTIONS_BY_VALUE(long long, sn::tags::base_tag<2>)
_SN_DECLARE_STRING_FUNCTIONS_BY_VALUE(unsigned long long, sn::tags::base_tag<2>)

_SN_DECLARE_STRING_FUNCTIONS_BY_VALUE(short, sn::tags::base_tag<8>)
_SN_DECLARE_STRING_FUNCTIONS_BY_VALUE(unsigned short, sn::tags::base_tag<8>)
_SN_DECLARE_STRING_FUNCTIONS_BY_VALUE(int, sn::tags::base_tag<8>)
_SN_DECLARE_STRING_FUNCTIONS_BY_VALUE(unsigned int, sn::tags::base_tag<8>)
_SN_DECLARE_STRING_FUNCTIONS_BY_VALUE(long, sn::tags::base_tag<8>)
_SN_DECLARE_STRING_FUNCTIONS_BY_VALUE(unsigned long, sn::tags::base_tag<8>)
_SN_DECLARE_STRING_FUNCTIONS_BY_VALUE(long long, sn::tags::base_tag<8>)
_SN_DECLARE_STRING_FUNCTIONS_BY_VALUE(unsigned long long, sn::tags::base_tag<8>)

_SN_DECLARE_STRING_FUNCTIONS_BY_VALUE(short, sn::tags::base_tag<16>)
_SN_DECLARE_STRING_FUNCTIONS_BY_VALUE(unsigned short, sn::tags::base_tag<16>)
_SN_DECLARE_STRING_FUNCTIONS_BY_VALUE(int, sn::tags::base_tag<16>)
_SN_DECLARE_STRING_FUNCTIONS_BY_VALUE(unsigned int, sn::tags::base_tag<16>)
_SN_DECLARE_STRING_FUNCTIONS_BY_VALUE(long, sn::tags::base_tag<16>)
_SN_DECLARE_STRING_FUNCTIONS_BY_VALUE(unsigned long, sn::tags::base_tag<16>)
_SN_DECLARE_STRING_FUNCTIONS_BY_VALUE(long long, sn::tags::base_tag<16>)
_SN_DECLARE_STRING_FUNCTIONS_BY_VALUE(unsigned long long, sn::tags::base_tag<16>)

_SN_DEFINE_INLINE_STRING_TAG_EATING_FUNCTIONS(short, sn::tags::base_tag<10>)
_SN_DEFINE_INLINE_STRING_TAG_EATING_FUNCTIONS(unsigned short, sn::tags::base_tag<10>)
_SN_DEFINE_INLINE_STRING_TAG_EATING_FUNCTIONS(int, sn::tags::base_tag<10>)
_SN_DEFINE_INLINE_STRING_TAG_EATING_FUNCTIONS(unsigned int, sn::tags::base_tag<10>)
_SN_DEFINE_INLINE_STRING_TAG_EATING_FUNCTIONS(long, sn::tags::base_tag<10>)
_SN_DEFINE_INLINE_STRING_TAG_EATING_FUNCTIONS(unsigned long, sn::tags::base_tag<10>)
_SN_DEFINE_INLINE_STRING_TAG_EATING_FUNCTIONS(long long, sn::tags::base_tag<10>)
_SN_DEFINE_INLINE_STRING_TAG_EATING_FUNCTIONS(unsigned long long, sn::tags::base_tag<10>)

} // namespace sn::detail::builtins
