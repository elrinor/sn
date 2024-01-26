#pragma once

#include <string>
#include <string_view>
#include <concepts>

#include "sn/core/tags.h"

#include "string_fwd.h"

namespace sn::builtins {

//
// Support for std::string / std::string_view / const char *.
//

[[nodiscard]] inline bool try_to_string(std::string_view src, std::string *dst) noexcept {
    *dst = src;
    return true;
}

inline void to_string(std::string_view src, std::string *dst) {
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
// Support for arithmetic types.
//
// No support for char / unsigned char / signed char here, as it's not clear what the default behavior should be.
//

SN_DECLARE_STRING_FUNCTIONS(bool, tn::detail::explicit_type_tag)
SN_DECLARE_STRING_FUNCTIONS(short)
SN_DECLARE_STRING_FUNCTIONS(unsigned short)
SN_DECLARE_STRING_FUNCTIONS(int)
SN_DECLARE_STRING_FUNCTIONS(unsigned int)
SN_DECLARE_STRING_FUNCTIONS(long)
SN_DECLARE_STRING_FUNCTIONS(unsigned long)
SN_DECLARE_STRING_FUNCTIONS(long long)
SN_DECLARE_STRING_FUNCTIONS(unsigned long long)
SN_DECLARE_STRING_FUNCTIONS(float)
SN_DECLARE_STRING_FUNCTIONS(double)


//
// Support for bool, ignoring builtin conversions. If we don't ignore them, then bool overload becomes a catch-all
// for all pointer types.
//

template<std::same_as<bool> T>
[[nodiscard]] bool try_to_string(const T &src, std::string *dst) noexcept {
    return try_to_string(src, dst, tn::detail::explicit_type);
}

template<std::same_as<bool> T>
void to_string(const T &src, std::string *dst) {
    to_string(src, dst, tn::detail::explicit_type);
}

template<std::same_as<bool> T>
[[nodiscard]] bool try_from_string(std::string_view src, T *dst) noexcept {
    return try_from_string(src, dst, tn::detail::explicit_type);
}

template<std::same_as<bool> T>
void from_string(std::string_view src, T *dst) {
    from_string(src, dst, tn::detail::explicit_type);
}

} // namespace sn::builtins
