#include "string_builtins.h"

#if SN_USE_STRTOF
#   include <cstdlib>
#endif

#include <cassert>
#include <array>
#include <charconv>
#include <concepts> // For std::integral.
#include <string>

#if SN_USE_FAST_FLOAT
#include <fast_float/fast_float.h>
#endif

#include "sn/detail/codegen/forwarding.h"

#include "small_string_capacity.h"
#include "string_errors.h"

namespace sn::detail::builtins {

//
// max_integer_lengths_v.
//
// This code is auto-generated using the `intgen` tool in `/tools`.
//

template<bool is_signed, int size>
static constexpr std::nullptr_t max_integer_lengths_v = nullptr;
template<>
constexpr std::array<std::uint8_t, 35> max_integer_lengths_v<true, 2> = {17, 11, 9, 8, 7, 7, 7, 6, 6, 6, 6, 6, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 4, 4, 4, 4};
template<>
constexpr std::array<std::uint8_t, 35> max_integer_lengths_v<false, 2> = {16, 11, 8, 7, 7, 6, 6, 6, 5, 5, 5, 5, 5, 5, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4};
template<>
constexpr std::array<std::uint8_t, 35> max_integer_lengths_v<true, 4> = {33, 21, 17, 15, 13, 13, 12, 11, 11, 10, 10, 10, 10, 9, 9, 9, 9, 9, 9, 9, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 7};
template<>
constexpr std::array<std::uint8_t, 35> max_integer_lengths_v<false, 4> = {32, 21, 16, 14, 13, 12, 11, 11, 10, 10, 9, 9, 9, 9, 8, 8, 8, 8, 8, 8, 8, 8, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7};
template<>
constexpr std::array<std::uint8_t, 35> max_integer_lengths_v<true, 8> = {65, 41, 33, 29, 26, 24, 23, 21, 20, 20, 19, 19, 18, 18, 17, 17, 17, 16, 16, 16, 16, 15, 15, 15, 15, 15, 15, 14, 14, 14, 14, 14, 14, 14, 14};
template<>
constexpr std::array<std::uint8_t, 35> max_integer_lengths_v<false, 8> = {64, 41, 32, 28, 25, 23, 22, 21, 20, 19, 18, 18, 17, 17, 16, 16, 16, 16, 15, 15, 15, 15, 14, 14, 14, 14, 14, 14, 14, 13, 13, 13, 13, 13, 13};


//
// max_float_length_v.
//
// Maximal length of the shortest round-trip representation of a floating point number, which is what std::to_chars
// produces. E.g. "-1.00000075e-36" for float, and "-1.7976931348623157e+308" for double.
//

template<class T>
static constexpr std::nullptr_t max_float_length_v = nullptr;
template<>
constexpr std::size_t max_float_length_v<float> = 15;
template<>
constexpr std::size_t max_float_length_v<double> = 24;


//
// max_arithmetic_length_v.
//
// Maximal length of a string representation of an arithmetic type, in any base. For integers, base 2 gives the longest
// strings.
//

template<class T>
static constexpr std::nullptr_t max_arithmetic_length_v = nullptr;
template<std::integral T>
constexpr std::size_t max_arithmetic_length_v<T> = max_integer_lengths_v<std::is_signed_v<T>, sizeof(T)>[0];
template<std::floating_point T>
constexpr std::size_t max_arithmetic_length_v<T> = max_float_length_v<T>;


//
// bool.
//

bool to_string(bool src, std::string *dst, sn::error *) {
    *dst = src ? "true" : "false";
    return true;
}

bool from_string(std::string_view src, bool *dst, sn::error *err) {
    if (src == "true" || src == "1") {
        *dst = true;
        return true;
    } else if (src == "false" || src == "0") {
        *dst = false;
        return true;
    } else {
        sn::detail::report_from_string_error(src, dst, err);
        return false;
    }
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
inline std::to_chars_result wrapped_to_chars(char *first, char *last, T value, sn::tags::dynamic_base_tag base) {
    return std::to_chars(first, last, value, base.value());
}

template<class T, int base>
inline std::to_chars_result wrapped_to_chars(char *first, char *last, T value, sn::tags::base_tag<base>) {
    static_assert(base != 10); // Base 10 should be handled by the overload w/o the tag parameter.
    return std::to_chars(first, last, value, base);
}

template<class T, class... Tags>
inline bool to_string(T src, std::string *dst, sn::error *, Tags... tags) {
    std::size_t max_size;
    if constexpr (std::is_integral_v<T>) {
        max_size = max_integer_lengths_v<std::is_signed_v<T>, sizeof(T)>[sn::detail::base_value(tags...) - 2];
    } else {
        static_assert(sizeof...(Tags) == 0);
        max_size = max_float_length_v<T>;
    }

    // If dst can hold the longest possible result without reallocating, we format right into it. This is the fastest
    // option. It also lets libc++'s base 10 std::to_chars take its fast path, which skips computing the number's length
    // when the buffer is large enough for any value. For other bases libc++ computes the length anyway, and so does
    // libstdc++ for all bases.
    //
    // Otherwise reserving the longest possible result in dst would allocate even for short results, e.g. for "0.5"
    // in a new string on msvc or libstdc++. So we format into a stack buffer and then copy, which only allocates if the
    // actual result doesn't fit.
    //
    // Checking max_size against small_string_capacity doesn't change the outcome, because every string can hold at
    // least small_string_capacity chars. It's there for performance. For most types and bases max_size is known at
    // compile time, so the compiler evaluates this check itself and drops the capacity() call.
    if (max_size <= small_string_capacity || max_size <= dst->capacity()) {
        dst->resize_and_overwrite(max_size, [&](char *data, size_t size) {
            std::to_chars_result result = wrapped_to_chars(data, data + size, src, tags...);
            assert(result.ec == std::errc()); // Should never fail.
            return result.ptr - data;
        });
    } else {
        std::array<char, max_arithmetic_length_v<T>> buffer;
        std::to_chars_result result = wrapped_to_chars(buffer.data(), buffer.data() + max_size, src, tags...);
        assert(result.ec == std::errc()); // Should never fail.
        dst->assign(buffer.data(), static_cast<std::size_t>(result.ptr - buffer.data()));
    }

    return true;
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
inline std::from_chars_result wrapped_from_chars(const char *ptr, const char *end, T *value, sn::tags::dynamic_base_tag base) {
    return std::from_chars(ptr, end, *value, base.value());
}

template<class T, int base>
inline std::from_chars_result wrapped_from_chars(const char *ptr, const char *end, T *value, sn::tags::base_tag<base>) {
    static_assert(base != 10); // Base 10 should be handled by the overload w/o the tag parameter.
    return std::from_chars(ptr, end, *value, base);
}

template<class T, class... Tags>
inline bool from_string(std::string_view src, T *dst, sn::error *err, Tags... tags) {
    const char *end = src.data() + src.size();
    std::from_chars_result result = wrapped_from_chars(src.data(), end, dst, tags...);
    if (result.ec == std::errc() && result.ptr == end) [[likely]]
        return true;

    // Trailing non-number characters mean "not a number".
    sn::detail::report_number_from_string_error(src, dst, err, result.ec == std::errc() ? std::errc::invalid_argument : result.ec);
    return false;
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
inline bool from_string(std::string_view src, T *dst, sn::error *err) {
    if (src.empty() || std::isspace(src[0]) || src[0] == '+') {
        // We behave the same as std::from_chars and don't skip whitespaces and don't allow leading '+'.
        sn::detail::report_number_from_string_error(src, dst, err, std::errc::invalid_argument);
        return false;
    }

    const char *src_end = src.data() + src.size();
    const char *end = src_end;
    errno = 0; // strto* does not change errno on success.
    T result = wrapped_strto<T>(src.data(), &end);
    if ((result != 0 || errno == 0) && end == src_end) {
        *dst = result;
        return true;
    }

    if (result == 0 && errno == ERANGE) {
        sn::detail::report_number_from_string_error(src, dst, err, std::errc::result_out_of_range);
    } else {
        sn::detail::report_number_from_string_error(src, dst, err, std::errc::invalid_argument); // Including tail non-number symbols.
    }
    return false;
}
} // namespace detail_strtofd
#endif // SN_USE_STRTOF

#if SN_USE_FAST_FLOAT
namespace detail_fast_float {
template<class T>
inline bool from_string(std::string_view src, T *dst, sn::error *err) {
    const char *end = src.data() + src.size();
    fast_float::from_chars_result result = fast_float::from_chars(src.data(), end, *dst);
    if (result.ec == std::errc() && result.ptr == end) [[likely]]
        return true;

    // Trailing non-number characters mean "not a number".
    sn::detail::report_number_from_string_error(src, dst, err, result.ec == std::errc() ? std::errc::invalid_argument : result.ec);
    return false;
}
} // namespace detail_fast_float
#endif // SN_USE_FAST_FLOAT

#define _SN_DEFINE_FORWARDING_STRING_FUNCTIONS(TYPE, FROM_STRING_NAMESPACE, ... /* TAGS */)                             \
    _SN_DEFINE_FORWARDING_FUNCTIONS(string, TYPE, std::string *, std::string_view, TYPE *, detail_to_chars, FROM_STRING_NAMESPACE, (__VA_ARGS__))

#if SN_USE_STRTOF
#   define SN_FLOAT_FROM_STRING_NAMESPACE detail_strtofd
#elif SN_USE_FROM_CHARS
#   define SN_FLOAT_FROM_STRING_NAMESPACE detail_from_chars
#elif SN_USE_FAST_FLOAT
#   define SN_FLOAT_FROM_STRING_NAMESPACE detail_fast_float
#else
#   error "Floating point string conversion library not configured"
#endif

_SN_DEFINE_FORWARDING_STRING_FUNCTIONS(short, detail_from_chars)
_SN_DEFINE_FORWARDING_STRING_FUNCTIONS(unsigned short, detail_from_chars)
_SN_DEFINE_FORWARDING_STRING_FUNCTIONS(int, detail_from_chars)
_SN_DEFINE_FORWARDING_STRING_FUNCTIONS(unsigned int, detail_from_chars)
_SN_DEFINE_FORWARDING_STRING_FUNCTIONS(long, detail_from_chars)
_SN_DEFINE_FORWARDING_STRING_FUNCTIONS(unsigned long, detail_from_chars)
_SN_DEFINE_FORWARDING_STRING_FUNCTIONS(long long, detail_from_chars)
_SN_DEFINE_FORWARDING_STRING_FUNCTIONS(unsigned long long, detail_from_chars)

_SN_DEFINE_FORWARDING_STRING_FUNCTIONS(float, SN_FLOAT_FROM_STRING_NAMESPACE)
_SN_DEFINE_FORWARDING_STRING_FUNCTIONS(double, SN_FLOAT_FROM_STRING_NAMESPACE)

_SN_DEFINE_FORWARDING_STRING_FUNCTIONS(short, detail_from_chars, sn::tags::dynamic_base_tag)
_SN_DEFINE_FORWARDING_STRING_FUNCTIONS(unsigned short, detail_from_chars, sn::tags::dynamic_base_tag)
_SN_DEFINE_FORWARDING_STRING_FUNCTIONS(int, detail_from_chars, sn::tags::dynamic_base_tag)
_SN_DEFINE_FORWARDING_STRING_FUNCTIONS(unsigned int, detail_from_chars, sn::tags::dynamic_base_tag)
_SN_DEFINE_FORWARDING_STRING_FUNCTIONS(long, detail_from_chars, sn::tags::dynamic_base_tag)
_SN_DEFINE_FORWARDING_STRING_FUNCTIONS(unsigned long, detail_from_chars, sn::tags::dynamic_base_tag)
_SN_DEFINE_FORWARDING_STRING_FUNCTIONS(long long, detail_from_chars, sn::tags::dynamic_base_tag)
_SN_DEFINE_FORWARDING_STRING_FUNCTIONS(unsigned long long, detail_from_chars, sn::tags::dynamic_base_tag)

_SN_DEFINE_FORWARDING_STRING_FUNCTIONS(short, detail_from_chars, sn::tags::base_tag<2>)
_SN_DEFINE_FORWARDING_STRING_FUNCTIONS(unsigned short, detail_from_chars, sn::tags::base_tag<2>)
_SN_DEFINE_FORWARDING_STRING_FUNCTIONS(int, detail_from_chars, sn::tags::base_tag<2>)
_SN_DEFINE_FORWARDING_STRING_FUNCTIONS(unsigned int, detail_from_chars, sn::tags::base_tag<2>)
_SN_DEFINE_FORWARDING_STRING_FUNCTIONS(long, detail_from_chars, sn::tags::base_tag<2>)
_SN_DEFINE_FORWARDING_STRING_FUNCTIONS(unsigned long, detail_from_chars, sn::tags::base_tag<2>)
_SN_DEFINE_FORWARDING_STRING_FUNCTIONS(long long, detail_from_chars, sn::tags::base_tag<2>)
_SN_DEFINE_FORWARDING_STRING_FUNCTIONS(unsigned long long, detail_from_chars, sn::tags::base_tag<2>)

_SN_DEFINE_FORWARDING_STRING_FUNCTIONS(short, detail_from_chars, sn::tags::base_tag<8>)
_SN_DEFINE_FORWARDING_STRING_FUNCTIONS(unsigned short, detail_from_chars, sn::tags::base_tag<8>)
_SN_DEFINE_FORWARDING_STRING_FUNCTIONS(int, detail_from_chars, sn::tags::base_tag<8>)
_SN_DEFINE_FORWARDING_STRING_FUNCTIONS(unsigned int, detail_from_chars, sn::tags::base_tag<8>)
_SN_DEFINE_FORWARDING_STRING_FUNCTIONS(long, detail_from_chars, sn::tags::base_tag<8>)
_SN_DEFINE_FORWARDING_STRING_FUNCTIONS(unsigned long, detail_from_chars, sn::tags::base_tag<8>)
_SN_DEFINE_FORWARDING_STRING_FUNCTIONS(long long, detail_from_chars, sn::tags::base_tag<8>)
_SN_DEFINE_FORWARDING_STRING_FUNCTIONS(unsigned long long, detail_from_chars, sn::tags::base_tag<8>)

_SN_DEFINE_FORWARDING_STRING_FUNCTIONS(short, detail_from_chars, sn::tags::base_tag<16>)
_SN_DEFINE_FORWARDING_STRING_FUNCTIONS(unsigned short, detail_from_chars, sn::tags::base_tag<16>)
_SN_DEFINE_FORWARDING_STRING_FUNCTIONS(int, detail_from_chars, sn::tags::base_tag<16>)
_SN_DEFINE_FORWARDING_STRING_FUNCTIONS(unsigned int, detail_from_chars, sn::tags::base_tag<16>)
_SN_DEFINE_FORWARDING_STRING_FUNCTIONS(long, detail_from_chars, sn::tags::base_tag<16>)
_SN_DEFINE_FORWARDING_STRING_FUNCTIONS(unsigned long, detail_from_chars, sn::tags::base_tag<16>)
_SN_DEFINE_FORWARDING_STRING_FUNCTIONS(long long, detail_from_chars, sn::tags::base_tag<16>)
_SN_DEFINE_FORWARDING_STRING_FUNCTIONS(unsigned long long, detail_from_chars, sn::tags::base_tag<16>)

} // namespace sn::detail::builtins
