#pragma once

#include <string>
#include <string_view>

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
