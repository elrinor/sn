#include "string_builtins.h"

#if SN_USE_STRTOF
#   include <cstdlib>
#endif

#include <cassert>
#include <array>
#include <charconv>
#include <string>

#if SN_USE_FAST_FLOAT
#include <fast_float/fast_float.h>
#endif

#include "sn/detail/preprocessor/preprocessor.h"
#include "sn/detail/format/format.h"

#include "string_exceptions.h"

namespace sn::detail::builtins {

//
// max_integer_lengths.
//
// This code is auto-generated using the `intgen` tool in `/tools`.
//

template<bool is_signed, int size>
static constexpr std::nullptr_t max_integer_lengths = nullptr;
template<>
constexpr std::array<std::uint8_t, 35> max_integer_lengths<true, 2> = {17, 11, 9, 8, 7, 7, 7, 6, 6, 6, 6, 6, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 4, 4, 4, 4};
template<>
constexpr std::array<std::uint8_t, 35> max_integer_lengths<false, 2> = {16, 11, 8, 7, 7, 6, 6, 6, 5, 5, 5, 5, 5, 5, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4};
template<>
constexpr std::array<std::uint8_t, 35> max_integer_lengths<true, 4> = {33, 21, 17, 15, 13, 13, 12, 11, 11, 10, 10, 10, 10, 9, 9, 9, 9, 9, 9, 9, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 7};
template<>
constexpr std::array<std::uint8_t, 35> max_integer_lengths<false, 4> = {32, 21, 16, 14, 13, 12, 11, 11, 10, 10, 9, 9, 9, 9, 8, 8, 8, 8, 8, 8, 8, 8, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7};
template<>
constexpr std::array<std::uint8_t, 35> max_integer_lengths<true, 8> = {65, 41, 33, 29, 26, 24, 23, 21, 20, 20, 19, 19, 18, 18, 17, 17, 17, 16, 16, 16, 16, 15, 15, 15, 15, 15, 15, 14, 14, 14, 14, 14, 14, 14, 14};
template<>
constexpr std::array<std::uint8_t, 35> max_integer_lengths<false, 8> = {64, 41, 32, 28, 25, 23, 22, 21, 20, 19, 18, 18, 17, 17, 16, 16, 16, 16, 15, 15, 15, 15, 14, 14, 14, 14, 14, 14, 14, 13, 13, 13, 13, 13, 13};


//
// bool.
//

bool try_to_string(bool src, std::string *dst) noexcept {
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

void to_string(bool src, std::string *dst) {
    (void) try_to_string(src, dst); // Always succeeds.
}

void from_string(std::string_view src, bool *dst) {
    if (!try_from_string(src, dst))
        sn::detail::throw_from_string_error<bool>(src);
}


//
// Arithmetic types.
//

namespace detail_to_chars {
template<class T>
inline std::to_chars_result wrapped_to_chars(char *first, char *last, T value) {
    // This is a separate function because std::to_chars w/o the base argument is a separate function in some STL
    // implementations, to make the default code path more efficient.
    return std::to_chars(first, last, value);
}

template<class T>
inline std::to_chars_result wrapped_to_chars(char *first, char *last, T value, sn::dynamic_base_tag base) {
    return std::to_chars(first, last, value, base.value());
}

template<class T, int base>
inline std::to_chars_result wrapped_to_chars(char *first, char *last, T value, sn::base_tag<base>) {
    static_assert(base != 10); // Base 10 should be handled by the overload w/o the tag parameter.
    return std::to_chars(first, last, value, base);
}

template<class T, class... Tags>
inline bool try_to_string(T src, std::string *dst, Tags... tags) noexcept {
    // We can actually do better, but it's probably not worth it.
    //
    // Since all modern STL implementations use small string optimization, we can first check if the value fits in the
    // small string buffer. These are the small buffer sizes:
    // - 15 chars for msvc.
    // - 15 chars for gcc's libstdc++.
    // - 22 chars for clang's libc++.
    // Numbers taken from https://tastyhedge.com/blog/memory-layout-of-std-string/.
    //
    // This will make it possible to avoid allocations in most cases.
    std::size_t max_size = max_integer_lengths<std::is_signed_v<T>, sizeof(T)>[sn::detail::base_value(tags...) - 2];

    // TODO(elric): #cpp23 use resize_and_overwrite, using libcxx's __resize_default_init gives x1.5 speedup.
    dst->resize(max_size);
    std::to_chars_result result = wrapped_to_chars(dst->data(), dst->data() + max_size, src, tags...);
    assert(result.ec == std::errc()); // Should never fail.
    dst->resize(result.ptr - dst->data());

    return true;
}

template<class T, class... Tags>
inline void to_string(T src, std::string *dst, Tags... tags) {
    (void) try_to_string(src, dst, tags...);
}
} // namespace detail_to_chars

namespace detail_from_chars {
template<class T>
inline std::from_chars_result wrapped_from_chars(const char *ptr, const char *end, T *value) {
    // This is a separate function because std::from_chars w/o the base argument is a separate function in some STL
    // implementations, to make the default code path more efficient.
    return std::from_chars(ptr, end, *value);
}

template<class T>
inline std::from_chars_result wrapped_from_chars(const char *ptr, const char *end, T *value, sn::dynamic_base_tag base) {
    return std::from_chars(ptr, end, *value, base.value());
}

template<class T, int base>
inline std::from_chars_result wrapped_from_chars(const char *ptr, const char *end, T *value, sn::base_tag<base>) {
    static_assert(base != 10); // Base 10 should be handled by the overload w/o the tag parameter.
    return std::from_chars(ptr, end, *value, base);
}

template<class T, class... Tags>
inline bool try_from_string(std::string_view src, T *dst, Tags... tags) noexcept {
    const char *end = src.data() + src.size();
    std::from_chars_result result = wrapped_from_chars(src.data(), end, dst, tags...);
    return result.ec == std::errc() && result.ptr == end;
}

template<class T, class... Tags>
inline void from_string(std::string_view src, T *dst, Tags... tags) {
    const char *end = src.data() + src.size();
    std::from_chars_result result = wrapped_from_chars(src.data(), end, dst, tags...);

    if (result.ec != std::errc())
        sn::detail::throw_number_from_string_error<T>(src, result.ec);

    if (result.ptr != end)
        sn::detail::throw_number_from_string_error<T>(src, std::errc::invalid_argument); // "Not a number"
}
} // namespace detail_from_chars

#if SN_USE_STRTOF
namespace detail_strtofd {
template<class T>
inline T wrapped_strto(const char *str, const char **end) = delete;

template<>
inline float wrapped_strto<float>(const char *str, const char **end) {
    return std::strtof(str, const_cast<char **>(end));
}

template<>
inline double wrapped_strto<double>(const char *str, const char **end) {
    return std::strtod(str, const_cast<char **>(end));
}

template<class T>
inline bool try_from_string(std::string_view src, T *dst) noexcept {
    if (src.empty() || std::isspace(src[0]) || src[0] == '+')
        return false; // We behave the same as std::from_chars and don't skip whitespaces and don't allow leading '+'.

    const char *src_end = src.data() + src.size();
    const char *end = src_end;
    errno = 0; // strto* does not change errno on success.
    T result = wrapped_strto<T>(src.data(), &end);
    if ((result != 0 || errno == 0) && end == src_end) {
        *dst = result;
        return true;
    } else {
        return false;
    }
}

template<class T>
inline void from_string(std::string_view src, T *dst) {
    // Implementation is pretty much a copy of try_from_string.
    if (src.empty() || std::isspace(src[0]) || src[0] == '+')
        sn::detail::throw_number_from_string_error<T>(src, std::errc::invalid_argument);

    const char *end = src.data() + src.size();
    errno = 0;
    T result = wrapped_strto<T>(src.data(), &end);
    if (result == 0) {
        if (errno == ERANGE)
            sn::detail::throw_number_from_string_error<T>(src, std::errc::result_out_of_range);
        if (errno != 0)
            sn::detail::throw_number_from_string_error<T>(src, std::errc::invalid_argument);
    }
    if (end != src.data() + src.size()) // Tail non-number symbols => not a number.
        sn::detail::throw_number_from_string_error<T>(src, std::errc::invalid_argument);
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
        sn::detail::throw_number_from_string_error<T>(src, result.ec);

    if (result.ptr != end)
        sn::detail::throw_number_from_string_error<T>(src, std::errc::invalid_argument); // "Not a number"
}
} // namespace detail_fast_float
#endif // SN_USE_FAST_FLOAT

#define SN_DEFINE_NUMERIC_STRING_FUNCTIONS(TYPE, FROM_STRING_NAMESPACE, ... /* TAGS */)                                 \
    SN_DEFINE_NUMERIC_STRING_FUNCTIONS_I(TYPE, FROM_STRING_NAMESPACE, _SN_PP_TUPLE_TYPES_TO_DECL_PARAMS(arg, (__VA_ARGS__)), _SN_PP_TUPLE_TYPES_TO_CALL_PARAMS(arg, (__VA_ARGS__)))
#define SN_DEFINE_NUMERIC_STRING_FUNCTIONS_I(TYPE, FROM_STRING_NAMESPACE, DECL_PARAMS, CALL_PARAMS)                     \
    bool try_to_string(TYPE src, std::string *dst SN_PP_TUPLE_ENUM_TRAILING(DECL_PARAMS)) noexcept {                    \
        return detail_to_chars::try_to_string(src, dst SN_PP_TUPLE_ENUM_TRAILING(CALL_PARAMS));                         \
    }                                                                                                                   \
    bool try_from_string(std::string_view src, TYPE *dst SN_PP_TUPLE_ENUM_TRAILING(DECL_PARAMS)) noexcept {             \
        return FROM_STRING_NAMESPACE::try_from_string(src, dst SN_PP_TUPLE_ENUM_TRAILING(CALL_PARAMS));                 \
    }                                                                                                                   \
    void to_string(TYPE src, std::string *dst SN_PP_TUPLE_ENUM_TRAILING(DECL_PARAMS)) {                                 \
        detail_to_chars::to_string(src, dst SN_PP_TUPLE_ENUM_TRAILING(CALL_PARAMS));                                    \
    }                                                                                                                   \
    void from_string(std::string_view src, TYPE *dst SN_PP_TUPLE_ENUM_TRAILING(DECL_PARAMS)) {                          \
        FROM_STRING_NAMESPACE::from_string(src, dst SN_PP_TUPLE_ENUM_TRAILING(CALL_PARAMS));                            \
    }

#if SN_USE_STRTOF
#   define SN_FLOAT_FROM_STRING_NAMESPACE detail_strtofd
#elif SN_USE_FROM_CHARS
#   define SN_FLOAT_FROM_STRING_NAMESPACE detail_from_chars
#elif SN_USE_FAST_FLOAT
#   define SN_FLOAT_FROM_STRING_NAMESPACE detail_fast_float
#else
#   error "Floating point string conversion library not configured"
#endif

SN_DEFINE_NUMERIC_STRING_FUNCTIONS(short, detail_from_chars)
SN_DEFINE_NUMERIC_STRING_FUNCTIONS(unsigned short, detail_from_chars)
SN_DEFINE_NUMERIC_STRING_FUNCTIONS(int, detail_from_chars)
SN_DEFINE_NUMERIC_STRING_FUNCTIONS(unsigned int, detail_from_chars)
SN_DEFINE_NUMERIC_STRING_FUNCTIONS(long, detail_from_chars)
SN_DEFINE_NUMERIC_STRING_FUNCTIONS(unsigned long, detail_from_chars)
SN_DEFINE_NUMERIC_STRING_FUNCTIONS(long long, detail_from_chars)
SN_DEFINE_NUMERIC_STRING_FUNCTIONS(unsigned long long, detail_from_chars)

SN_DEFINE_NUMERIC_STRING_FUNCTIONS(float, SN_FLOAT_FROM_STRING_NAMESPACE)
SN_DEFINE_NUMERIC_STRING_FUNCTIONS(double, SN_FLOAT_FROM_STRING_NAMESPACE)

SN_DEFINE_NUMERIC_STRING_FUNCTIONS(short, detail_from_chars, sn::dynamic_base_tag)
SN_DEFINE_NUMERIC_STRING_FUNCTIONS(unsigned short, detail_from_chars, sn::dynamic_base_tag)
SN_DEFINE_NUMERIC_STRING_FUNCTIONS(int, detail_from_chars, sn::dynamic_base_tag)
SN_DEFINE_NUMERIC_STRING_FUNCTIONS(unsigned int, detail_from_chars, sn::dynamic_base_tag)
SN_DEFINE_NUMERIC_STRING_FUNCTIONS(long, detail_from_chars, sn::dynamic_base_tag)
SN_DEFINE_NUMERIC_STRING_FUNCTIONS(unsigned long, detail_from_chars, sn::dynamic_base_tag)
SN_DEFINE_NUMERIC_STRING_FUNCTIONS(long long, detail_from_chars, sn::dynamic_base_tag)
SN_DEFINE_NUMERIC_STRING_FUNCTIONS(unsigned long long, detail_from_chars, sn::dynamic_base_tag)

SN_DEFINE_NUMERIC_STRING_FUNCTIONS(short, detail_from_chars, sn::base_tag<2>)
SN_DEFINE_NUMERIC_STRING_FUNCTIONS(unsigned short, detail_from_chars, sn::base_tag<2>)
SN_DEFINE_NUMERIC_STRING_FUNCTIONS(int, detail_from_chars, sn::base_tag<2>)
SN_DEFINE_NUMERIC_STRING_FUNCTIONS(unsigned int, detail_from_chars, sn::base_tag<2>)
SN_DEFINE_NUMERIC_STRING_FUNCTIONS(long, detail_from_chars, sn::base_tag<2>)
SN_DEFINE_NUMERIC_STRING_FUNCTIONS(unsigned long, detail_from_chars, sn::base_tag<2>)
SN_DEFINE_NUMERIC_STRING_FUNCTIONS(long long, detail_from_chars, sn::base_tag<2>)
SN_DEFINE_NUMERIC_STRING_FUNCTIONS(unsigned long long, detail_from_chars, sn::base_tag<2>)

SN_DEFINE_NUMERIC_STRING_FUNCTIONS(short, detail_from_chars, sn::base_tag<8>)
SN_DEFINE_NUMERIC_STRING_FUNCTIONS(unsigned short, detail_from_chars, sn::base_tag<8>)
SN_DEFINE_NUMERIC_STRING_FUNCTIONS(int, detail_from_chars, sn::base_tag<8>)
SN_DEFINE_NUMERIC_STRING_FUNCTIONS(unsigned int, detail_from_chars, sn::base_tag<8>)
SN_DEFINE_NUMERIC_STRING_FUNCTIONS(long, detail_from_chars, sn::base_tag<8>)
SN_DEFINE_NUMERIC_STRING_FUNCTIONS(unsigned long, detail_from_chars, sn::base_tag<8>)
SN_DEFINE_NUMERIC_STRING_FUNCTIONS(long long, detail_from_chars, sn::base_tag<8>)
SN_DEFINE_NUMERIC_STRING_FUNCTIONS(unsigned long long, detail_from_chars, sn::base_tag<8>)

SN_DEFINE_NUMERIC_STRING_FUNCTIONS(short, detail_from_chars, sn::base_tag<16>)
SN_DEFINE_NUMERIC_STRING_FUNCTIONS(unsigned short, detail_from_chars, sn::base_tag<16>)
SN_DEFINE_NUMERIC_STRING_FUNCTIONS(int, detail_from_chars, sn::base_tag<16>)
SN_DEFINE_NUMERIC_STRING_FUNCTIONS(unsigned int, detail_from_chars, sn::base_tag<16>)
SN_DEFINE_NUMERIC_STRING_FUNCTIONS(long, detail_from_chars, sn::base_tag<16>)
SN_DEFINE_NUMERIC_STRING_FUNCTIONS(unsigned long, detail_from_chars, sn::base_tag<16>)
SN_DEFINE_NUMERIC_STRING_FUNCTIONS(long long, detail_from_chars, sn::base_tag<16>)
SN_DEFINE_NUMERIC_STRING_FUNCTIONS(unsigned long long, detail_from_chars, sn::base_tag<16>)

} // namespace sn::detail::builtins
