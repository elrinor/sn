#include "string_builtins.h"

#if SN_USE_STRTOF
#   include <cstdlib>
#endif

#include <cassert>
#include <charconv>

#if SN_USE_FAST_FLOAT
#   include <fast_float/fast_float.h>
#endif

#include "sn/core/format.h"

#include "string_exceptions.h"

namespace sn::builtins {

//
// bool.
//

bool try_to_string(const bool &src, std::string *dst) noexcept {
    *dst = src ? "true" : "false";
    return true;
}

bool try_from_string(std::string_view src, bool *dst) noexcept {
    if (src == "true" || src == "1") {
        *dst = true;
        return true;
    } else if (src == "false" || src == "0") {
        *dst = false;
        return true;
    } else {
        return false;
    }
}

void to_string(const bool &src, std::string *dst) {
    (void) try_to_string(src, dst); // Always succeeds.
}

void from_string(std::string_view src, bool *dst) {
    if (!try_from_string(src, dst))
        throw_from_string_error<bool>(src);
}


//
// Arithmetic types.
//

namespace detail_to_string {
template<class T>
inline bool try_to_string(const T &src, std::string *dst) noexcept {
    *dst = sn::detail::format("{}", src);
    return true;
}

template<class T>
inline void to_string(const T &src, std::string *dst) {
    *dst = sn::detail::format("{}", src);
}
} // namespace detail_to_string

namespace detail_from_chars {
template<class T>
inline bool try_from_string(std::string_view src, T *dst) noexcept {
    const char *end = src.data() + src.size();
    std::from_chars_result result = std::from_chars(src.data(), end, *dst);
    return result.ec == std::errc() && result.ptr == end;
}

template<class T>
inline void from_string(std::string_view src, T *dst) {
    const char *end = src.data() + src.size();
    std::from_chars_result result = std::from_chars(src.data(), end, *dst);

    if (result.ec != std::errc())
        sn::throw_number_from_string_error<T>(src, result.ec);

    if (result.ptr != end)
        sn::throw_number_from_string_error<T>(src, std::errc::invalid_argument); // "Not a number"
}
} // namespace detail_from_chars

#if SN_USE_STRTOF
namespace detail_strtofd {
template<class T>
inline T strto(const char *str, const char **end);

template<>
inline float strto<float>(const char *str, const char **end) {
    return std::strtof(str, const_cast<char **>(end));
}

template<>
inline double strto<double>(const char *str, const char **end) {
    return std::strtod(str, const_cast<char **>(end));
}

template<class T>
inline bool try_from_string(std::string_view src, T *dst) noexcept {
    if (src.empty() || std::isspace(src[0]))
        return false; // We behave the same as std::from_chars and don't skip whitespaces.

    const char *end = src.data() + src.size();
    errno = 0; // strto* do not change the setting of the errno on success.
    T result = strto<T>(src.data(), &end);
    if (result != 0 || errno == 0) {
        assert(end == src.data() + src.size());
        *dst = result;
        return true;
    } else {
        return false;
    }
}

template<class T>
inline void from_string(std::string_view src, T *dst) {
    if (src.empty() || std::isspace(src[0]))
        throw_number_from_string_error<T>(src, std::errc::invalid_argument);

    // Implementation pretty much a copy of try_from_string.
    const char *end = src.data() + src.size();
    errno = 0;
    T result = strto<T>(src.data(), &end);
    if (result == 0) {
        if (errno == ERANGE)
            throw_number_from_string_error<T>(src, std::errc::result_out_of_range);
        if (errno != 0)
            throw_number_from_string_error<T>(src, std::errc::invalid_argument);
    }
    if (end != src.data() + src.size()) // Tail non-number symbols => not a number.
        throw_number_from_string_error<T>(src, std::errc::invalid_argument);
    *dst = result;
}
} // namespace detail_strtofd
#endif // SN_USE_STRTOF

#if SN_USE_FAST_FLOAT
namespace detail_fast_float {
template<class T>
inline bool try_from_string(std::string_view src, T *dst) noexcept {
    const char *end = src.data() + src.size();
    fast_float::from_chars_result result = fast_float::from_chars(src.data(), end, *dst);
    return result.ec == std::errc() && result.ptr == end;
}

template<class T>
inline void from_string(std::string_view src, T *dst) {
    const char *end = src.data() + src.size();
    fast_float::from_chars_result result = fast_float::from_chars(src.data(), end, *dst);

    if (result.ec != std::errc())
        throw_number_from_string_error<T>(src, result.ec);

    if (result.ptr != end)
        throw_number_from_string_error<T>(src, std::errc::invalid_argument); // "Not a number"
}
} // namespace detail_fast_float
#endif // SN_USE_FAST_FLOAT

#define MM_DEFINE_PROXY_SERIALIZATION_FUNCTIONS(TYPE, FROM_STRING_NAMESPACE)                                            \
    bool try_to_string(const TYPE &src, std::string *dst) noexcept { return detail_to_string::try_to_string(src, dst); } \
    bool try_from_string(std::string_view src, TYPE *dst) noexcept { return FROM_STRING_NAMESPACE::try_from_string(src, dst); }  \
    void to_string(const TYPE &src, std::string *dst) { detail_to_string::to_string(src, dst); }                        \
    void from_string(std::string_view src, TYPE *dst) { FROM_STRING_NAMESPACE::from_string(src, dst); }

#if SN_USE_STRTOF
#   define SN_FLOAT_FROM_STRING_NAMESPACE detail_strtofd
#elif SN_USE_FROM_CHARS
#   define SN_FLOAT_FROM_STRING_NAMESPACE detail_from_chars
#elif SN_USE_FAST_FLOAT
#   define SN_FLOAT_FROM_STRING_NAMESPACE detail_fast_float
#else
#   error "Floating point string conversion library not configured"
#endif

MM_DEFINE_PROXY_SERIALIZATION_FUNCTIONS(short, detail_from_chars)
MM_DEFINE_PROXY_SERIALIZATION_FUNCTIONS(unsigned short, detail_from_chars)
MM_DEFINE_PROXY_SERIALIZATION_FUNCTIONS(int, detail_from_chars)
MM_DEFINE_PROXY_SERIALIZATION_FUNCTIONS(unsigned int, detail_from_chars)
MM_DEFINE_PROXY_SERIALIZATION_FUNCTIONS(long, detail_from_chars)
MM_DEFINE_PROXY_SERIALIZATION_FUNCTIONS(unsigned long, detail_from_chars)
MM_DEFINE_PROXY_SERIALIZATION_FUNCTIONS(long long, detail_from_chars)
MM_DEFINE_PROXY_SERIALIZATION_FUNCTIONS(unsigned long long, detail_from_chars)
MM_DEFINE_PROXY_SERIALIZATION_FUNCTIONS(float, SN_FLOAT_FROM_STRING_NAMESPACE)
MM_DEFINE_PROXY_SERIALIZATION_FUNCTIONS(double, SN_FLOAT_FROM_STRING_NAMESPACE)

} // namespace sn::builtins
