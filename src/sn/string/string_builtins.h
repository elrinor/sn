#pragma once

#include <string>
#include <string_view>
#include <concepts>

#include "string_fwd.h"

namespace sn::builtins {

//
// Support for std::string.
//

void is_string_supported_type(std::type_identity<std::string>);

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

void is_string_supported_type(std::type_identity<std::string_view>);

[[nodiscard]] inline bool try_to_string(std::string_view src, std::string *dst) noexcept {
    *dst = src;
    return true;
}

inline void to_string(std::string_view src, std::string *dst) {
    *dst = src;
}

[[nodiscard]] inline bool try_from_string(std::string_view src, std::string_view *dst) noexcept = delete;
inline void from_string(std::string_view src, std::string_view *dst) = delete;


//
// Support for char[N], to_string only.
//

template<std::size_t N>
void is_string_supported_type(std::type_identity<char[N]>);

template<std::size_t N>
[[nodiscard]] inline bool try_to_string(const char (&src)[N], std::string *dst) noexcept {
    *dst = src;
    return true;
}

template<std::size_t N>
inline void to_string(const char (&src)[N], std::string *dst) {
    *dst = src;
}

template<std::size_t N>
[[nodiscard]] inline bool try_from_string(std::string_view src, const char (*dst)[N]) noexcept = delete;

template<std::size_t N>
inline void from_string(std::string_view src, const char (*dst)[N]) = delete;


//
// Support for const char *, to_string only.
//

void is_string_supported_type(std::type_identity<const char *>);

[[nodiscard]] inline bool try_to_string(const char *src, std::string *dst) noexcept {
    *dst = src;
    return true;
}

inline void to_string(const char *src, std::string *dst) {
    *dst = src;
}

[[nodiscard]] inline bool try_from_string(std::string_view src, const char **dst) noexcept = delete;
inline void from_string(std::string_view src, const char **dst) = delete;


//
// Support for arithmetic types.
//
// No support for char / unsigned char / signed char here, as it's not clear what the default behavior should be.
//

SN_DECLARE_STRING_FUNCTIONS(bool)
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

} // namespace sn::builtins
