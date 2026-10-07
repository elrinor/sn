#pragma once

#include <cstddef> // For std::size_t.
#include <string>
#include <string_view>

#include <nlohmann/json.hpp>

#include "sn/njson/njson_fwd.h"

#include "njson_shortcuts.h"
#include "njson_std_builtins.h"

namespace sn::detail::builtins {

//
// Support for nlohmann::json.
//

[[nodiscard]] inline bool try_to_njson(const nlohmann::json &src, nlohmann::json *dst) noexcept {
    *dst = src;
    return true;
}

inline void to_njson(const nlohmann::json &src, nlohmann::json *dst) {
    *dst = src;
}

[[nodiscard]] inline bool try_from_njson(const nlohmann::json &src, nlohmann::json *dst) noexcept {
    *dst = src;
    return true;
}

inline void from_njson(const nlohmann::json &src, nlohmann::json *dst) {
    *dst = src;
}


//
// Support for std::string.
//

SN_DECLARE_NJSON_FUNCTIONS(std::string)


//
// Support for std::string_view, to_njson only.
//

[[nodiscard]] inline bool try_to_njson(std::string_view src, nlohmann::json *dst) noexcept {
    *dst = std::string(src);
    return true;
}

inline void to_njson(std::string_view src, nlohmann::json *dst) {
    *dst = std::string(src);
}


//
// Support for const char[N], to_njson only.
//

template<std::size_t N>
[[nodiscard]] inline bool try_to_njson(const char (&src)[N], nlohmann::json *dst) noexcept {
    *dst = std::string(src);
    return true;
}

template<std::size_t N>
inline void to_njson(const char (&src)[N], nlohmann::json *dst) {
    *dst = std::string(src);
}


//
// Support for const char *, to_njson only.
//

[[nodiscard]] inline bool try_to_njson(const char *src, nlohmann::json *dst) noexcept {
    *dst = std::string(src);
    return true;
}

inline void to_njson(const char *src, nlohmann::json *dst) {
    *dst = std::string(src);
}


//
// Support for char *, to_njson only.
//

[[nodiscard]] inline bool try_to_njson(char *src, nlohmann::json *dst) noexcept {
    *dst = std::string(src);
    return true;
}

inline void to_njson(char *src, nlohmann::json *dst) {
    *dst = std::string(src);
}


//
// Support for arithmetic types.
//
// No support for char / unsigned char / signed char here, as it's not clear what the default behavior should be.
//
// Integers can be deserialized from json numbers that have an integral value, so 5.0 is OK for an int. Python's json
// module writes floats this way. Json doesn't support NaN and infinity, so they can't be serialized.
//

_SN_DECLARE_NJSON_FUNCTIONS_BY_VALUE(bool)

_SN_DECLARE_NJSON_FUNCTIONS_BY_VALUE(short)
_SN_DECLARE_NJSON_FUNCTIONS_BY_VALUE(unsigned short)
_SN_DECLARE_NJSON_FUNCTIONS_BY_VALUE(int)
_SN_DECLARE_NJSON_FUNCTIONS_BY_VALUE(unsigned int)
_SN_DECLARE_NJSON_FUNCTIONS_BY_VALUE(long)
_SN_DECLARE_NJSON_FUNCTIONS_BY_VALUE(unsigned long)
_SN_DECLARE_NJSON_FUNCTIONS_BY_VALUE(long long)
_SN_DECLARE_NJSON_FUNCTIONS_BY_VALUE(unsigned long long)

_SN_DECLARE_NJSON_FUNCTIONS_BY_VALUE(float)
_SN_DECLARE_NJSON_FUNCTIONS_BY_VALUE(double)

} // namespace sn::detail::builtins
