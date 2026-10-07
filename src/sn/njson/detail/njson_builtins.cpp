#include "njson_builtins.h"

#include <cmath> // For std::isfinite, std::trunc.
#include <limits>
#include <string>
#include <type_traits>
#include <utility> // For std::in_range.

#include "sn/detail/codegen/forwarding.h"
#include "sn/string/string.h"

#include "njson_exceptions.h"

namespace sn::detail::builtins {

//
// bool.
//

bool try_to_njson(bool src, nlohmann::json *dst) noexcept {
    *dst = src;
    return true;
}

void to_njson(bool src, nlohmann::json *dst) {
    *dst = src;
}

bool try_from_njson(const nlohmann::json &src, bool *dst) noexcept {
    const nlohmann::json::boolean_t *value = src.get_ptr<const nlohmann::json::boolean_t *>();
    if (!value)
        return false;

    *dst = *value;
    return true;
}

void from_njson(const nlohmann::json &src, bool *dst) {
    if (!try_from_njson(src, dst))
        sn::detail::throw_from_njson_error<bool>(src);
}


//
// std::string.
//

bool try_to_njson(const std::string &src, nlohmann::json *dst) noexcept {
    *dst = src;
    return true;
}

void to_njson(const std::string &src, nlohmann::json *dst) {
    *dst = src;
}

bool try_from_njson(const nlohmann::json &src, std::string *dst) noexcept {
    const nlohmann::json::string_t *value = src.get_ptr<const nlohmann::json::string_t *>();
    if (!value)
        return false;

    *dst = *value;
    return true;
}

void from_njson(const nlohmann::json &src, std::string *dst) {
    if (!try_from_njson(src, dst))
        sn::detail::throw_from_njson_error<std::string>(src);
}


//
// Arithmetic types.
//

namespace detail_njson {
template<class T>
inline bool try_to_njson(T src, nlohmann::json *dst) noexcept {
    if constexpr (std::is_floating_point_v<T>) {
        if (!std::isfinite(src))
            return false;
        *dst = static_cast<nlohmann::json::number_float_t>(src);
    } else if constexpr (std::is_signed_v<T>) {
        *dst = static_cast<nlohmann::json::number_integer_t>(src);
    } else {
        *dst = static_cast<nlohmann::json::number_unsigned_t>(src);
    }
    return true;
}

template<class T>
inline void to_njson(T src, nlohmann::json *dst) {
    if (!try_to_njson(src, dst))
        sn::detail::throw_to_njson_error<T>(sn::to_string(src));
}

template<class T>
inline bool try_integer_from_njson(const nlohmann::json &src, T *dst) noexcept {
    if (const nlohmann::json::number_integer_t *value = src.get_ptr<const nlohmann::json::number_integer_t *>()) {
        if (!std::in_range<T>(*value))
            return false;
        *dst = static_cast<T>(*value);
        return true;
    }

    if (const nlohmann::json::number_unsigned_t *value = src.get_ptr<const nlohmann::json::number_unsigned_t *>()) {
        if (!std::in_range<T>(*value))
            return false;
        *dst = static_cast<T>(*value);
        return true;
    }

    if (const nlohmann::json::number_float_t *value = src.get_ptr<const nlohmann::json::number_float_t *>()) {
        // Values of T are in [-2^digits, 2^digits) for signed T, and in [0, 2^digits) for unsigned T. Powers of two
        // are exact in a double, unlike the max value of T, so we can check the range w/o rounding issues.
        constexpr double limit = 2.0 * static_cast<double>(std::numeric_limits<T>::max() / 2 + 1);
        constexpr double lower_limit = std::is_signed_v<T> ? -limit : 0.0;

        // Comparisons are false for NaN, and infinities are out of range.
        if (!(*value >= lower_limit && *value < limit) || std::trunc(*value) != *value)
            return false;
        *dst = static_cast<T>(*value);
        return true;
    }

    return false;
}

template<class T>
inline bool try_float_from_njson(const nlohmann::json &src, T *dst) noexcept {
    if (const nlohmann::json::number_integer_t *value = src.get_ptr<const nlohmann::json::number_integer_t *>()) {
        *dst = static_cast<T>(*value);
        return true;
    }

    if (const nlohmann::json::number_unsigned_t *value = src.get_ptr<const nlohmann::json::number_unsigned_t *>()) {
        *dst = static_cast<T>(*value);
        return true;
    }

    if (const nlohmann::json::number_float_t *value = src.get_ptr<const nlohmann::json::number_float_t *>()) {
        // Json doesn't have NaN and infinity, so if we see them, then the value didn't come from json text. We treat
        // them the same way as values out of range.
        if (!std::isfinite(*value) || std::abs(*value) > std::numeric_limits<T>::max())
            return false;
        *dst = static_cast<T>(*value);
        return true;
    }

    return false;
}

template<class T>
inline bool try_from_njson(const nlohmann::json &src, T *dst) noexcept {
    if constexpr (std::is_floating_point_v<T>) {
        return try_float_from_njson(src, dst);
    } else {
        return try_integer_from_njson(src, dst);
    }
}

template<class T>
inline void from_njson(const nlohmann::json &src, T *dst) {
    if (!try_from_njson(src, dst))
        sn::detail::throw_from_njson_error<T>(src);
}
} // namespace detail_njson

#define _SN_DEFINE_FORWARDING_NJSON_FUNCTIONS(TYPE)                                                                     \
    _SN_DEFINE_FORWARDING_FUNCTIONS(njson, TYPE, nlohmann::json *, const nlohmann::json &, TYPE *, detail_njson, detail_njson, ())

_SN_DEFINE_FORWARDING_NJSON_FUNCTIONS(short)
_SN_DEFINE_FORWARDING_NJSON_FUNCTIONS(unsigned short)
_SN_DEFINE_FORWARDING_NJSON_FUNCTIONS(int)
_SN_DEFINE_FORWARDING_NJSON_FUNCTIONS(unsigned int)
_SN_DEFINE_FORWARDING_NJSON_FUNCTIONS(long)
_SN_DEFINE_FORWARDING_NJSON_FUNCTIONS(unsigned long)
_SN_DEFINE_FORWARDING_NJSON_FUNCTIONS(long long)
_SN_DEFINE_FORWARDING_NJSON_FUNCTIONS(unsigned long long)

_SN_DEFINE_FORWARDING_NJSON_FUNCTIONS(float)
_SN_DEFINE_FORWARDING_NJSON_FUNCTIONS(double)

} // namespace sn::detail::builtins
