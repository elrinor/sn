#include <cmath>
#include <limits>
#include <string>
#include <string_view>
#include <list>
#include <set>

#include <gtest/gtest.h> // NOLINT: not a C system header.
#include <nlohmann/json.hpp>

#include "sn/core/exception.h"
#include "sn/core/type_name.h"
#include "sn/detail/format/format.h"
#include "sn/njson/njson.h"

template<class T>
static void check_supported() {
    static_assert(sn::concepts::to_njsonable<T>);
    static_assert(sn::concepts::try_to_njsonable<T>);
    static_assert(sn::concepts::from_njsonable<T>);
    static_assert(sn::concepts::try_from_njsonable<T>);
}

template<class T>
static void check_unsupported() {
    static_assert(!sn::concepts::to_njsonable<T>);
    static_assert(!sn::concepts::try_to_njsonable<T>);
    static_assert(!sn::concepts::from_njsonable<T>);
    static_assert(!sn::concepts::try_from_njsonable<T>);
}

template<class T>
static void check_has_to_njson_only() {
    static_assert(sn::concepts::to_njsonable<T>);
    static_assert(sn::concepts::try_to_njsonable<T>);
    static_assert(!sn::concepts::from_njsonable<T>);
    static_assert(!sn::concepts::try_from_njsonable<T>);
}

template<class T>
static void expect_valid_from(const nlohmann::json &src, const T &expected) {
    T dst = T();
    EXPECT_TRUE(sn::try_from_njson(src, &dst)) << "with src = " << src.dump();
    EXPECT_EQ(dst, expected) << "with src = " << src.dump();

    dst = T();
    EXPECT_NO_THROW(sn::from_njson(src, &dst)) << "with src = " << src.dump();
    EXPECT_EQ(dst, expected) << "with src = " << src.dump();
}

template<class T>
static void expect_valid_to(const T &src, const nlohmann::json &expected) {
    nlohmann::json dst;
    EXPECT_TRUE(sn::try_to_njson(src, &dst)) << "with expected = " << expected.dump();
    EXPECT_EQ(dst, expected);
    EXPECT_EQ(dst.type(), expected.type()) << "with expected = " << expected.dump();

    dst = nlohmann::json();
    EXPECT_NO_THROW(sn::to_njson(src, &dst)) << "with expected = " << expected.dump();
    EXPECT_EQ(dst, expected);
    EXPECT_EQ(dst.type(), expected.type()) << "with expected = " << expected.dump();
}

template<class T>
static void expect_valid_roundtrip(const T &value) {
    nlohmann::json json;
    EXPECT_TRUE(sn::try_to_njson(value, &json));
    T dst = T();
    EXPECT_TRUE(sn::try_from_njson(json, &dst)) << "with json = " << json.dump();
    EXPECT_EQ(dst, value);

    // Also go through json text, as this is how it's normally used.
    EXPECT_EQ(sn::from_njson<T>(nlohmann::json::parse(sn::to_njson(value).dump())), value);
}

template<class T>
static void expect_throwing_from(const nlohmann::json &src) {
    T dst = T();
    EXPECT_FALSE(sn::try_from_njson(src, &dst)) << "with src = " << src.dump() << " and T = " << sn::type_name<T>();
    EXPECT_THROW(sn::from_njson(src, &dst), sn::exception) << "with src = " << src.dump() << " and T = " << sn::type_name<T>();
}

template<class T>
static void expect_throwing_to(const T &src) {
    nlohmann::json dst;
    EXPECT_FALSE(sn::try_to_njson(src, &dst)) << "with T = " << sn::type_name<T>();
    EXPECT_THROW(sn::to_njson(src, &dst), sn::exception) << "with T = " << sn::type_name<T>();
}

TEST(njson, json) {
    check_supported<nlohmann::json>();

    nlohmann::json value = nlohmann::json::parse(R"({"a": [1, 2.5, "3", null, true], "b": {}})");
    expect_valid_to(value, value);
    expect_valid_from(value, value);
}

TEST(njson, pointers) {
    // Check all char pointers & array types.
    check_has_to_njson_only<char[4]>();
    check_unsupported<char8_t[4]>();
    check_unsupported<char16_t[4]>();
    check_unsupported<char32_t[4]>();
    check_unsupported<wchar_t[4]>();
    check_has_to_njson_only<const char *>();
    check_unsupported<const char8_t *>();
    check_unsupported<const char16_t *>();
    check_unsupported<const char32_t *>();
    check_unsupported<const wchar_t *>();
    check_has_to_njson_only<char *>();
    check_unsupported<char8_t *>();
    check_unsupported<char16_t *>();
    check_unsupported<char32_t *>();
    check_unsupported<wchar_t *>();

    // And we also do some sanity checks for non-char pointers.
    check_unsupported<unsigned char *>();
    check_unsupported<signed char *>();
    check_unsupported<const unsigned char *>();
    check_unsupported<const signed char *>();
    check_unsupported<void *>();
    check_unsupported<int *>();
    check_unsupported<unsigned int *>();
    check_unsupported<const void *>();
    check_unsupported<const int *>();
    check_unsupported<std::nullptr_t>();
}

TEST(njson, string) { // NOLINT: this is not std::string.
    check_supported<std::string>();
    check_has_to_njson_only<std::string_view>();

    const char *str = "1234";
    char mutable_str[] = "12345";
    EXPECT_EQ(sn::to_njson(str), nlohmann::json("1234"));
    EXPECT_EQ(sn::to_njson(static_cast<char *>(mutable_str)), nlohmann::json("12345"));
    EXPECT_EQ(sn::to_njson("123"), nlohmann::json("123"));
    EXPECT_EQ(sn::to_njson(std::string("123")), nlohmann::json("123"));
    EXPECT_EQ(sn::to_njson(std::string_view("123")), nlohmann::json("123"));

    expect_valid_from(nlohmann::json("123"), std::string("123"));
    expect_valid_from(nlohmann::json(""), std::string(""));
    expect_valid_roundtrip(std::string("\"quoted\" \\ \n \xc3\xa9")); // Last one is 'e' with an acute accent in UTF-8.

    expect_throwing_from<std::string>(nlohmann::json(1));
    expect_throwing_from<std::string>(nlohmann::json(true));
    expect_throwing_from<std::string>(nlohmann::json(nullptr));
    expect_throwing_from<std::string>(nlohmann::json::array({"1"}));
    expect_throwing_from<std::string>(nlohmann::json::object());
}

TEST(njson, char) {
    check_unsupported<char>();
    check_unsupported<unsigned char>();
    check_unsupported<signed char>();
    check_unsupported<char8_t>();
    check_unsupported<char16_t>();
    check_unsupported<char32_t>();
    check_unsupported<wchar_t>();
}

TEST(njson, boolean) {
    check_supported<bool>();

    expect_valid_to(true, nlohmann::json(true));
    expect_valid_to(false, nlohmann::json(false));
    expect_valid_from(nlohmann::json(true), true);
    expect_valid_from(nlohmann::json(false), false);

    expect_throwing_from<bool>(nlohmann::json(0));
    expect_throwing_from<bool>(nlohmann::json(1));
    expect_throwing_from<bool>(nlohmann::json(1.0));
    expect_throwing_from<bool>(nlohmann::json("true"));
    expect_throwing_from<bool>(nlohmann::json(nullptr));
}

template<class T>
static void run_integer_tests() {
    check_supported<T>();

    using limits = std::numeric_limits<T>;

    // Signed types are written as number_integer, unsigned as number_unsigned, same as in nlohmann::json.
    expect_valid_to(T(0), nlohmann::json(T(0)));
    expect_valid_to(T(100), nlohmann::json(T(100)));
    expect_valid_to(limits::max(), nlohmann::json(limits::max()));
    expect_valid_to(limits::min(), nlohmann::json(limits::min()));
    EXPECT_EQ(sn::to_njson(T(1)).is_number_unsigned(), std::is_unsigned_v<T>);

    expect_valid_roundtrip(T(0));
    expect_valid_roundtrip(T(100));
    expect_valid_roundtrip(limits::max());
    expect_valid_roundtrip(limits::min());

    // Json text "1" is parsed as number_unsigned, and "-1" as number_integer, both should work.
    expect_valid_from(nlohmann::json::parse("1"), T(1));
    expect_valid_from(nlohmann::json(static_cast<std::int64_t>(1)), T(1));
    expect_valid_from(nlohmann::json(static_cast<std::uint64_t>(1)), T(1));
    if constexpr (std::is_signed_v<T>) {
        expect_valid_from(nlohmann::json::parse("-1"), T(-1));
    } else {
        expect_throwing_from<T>(nlohmann::json::parse("-1"));
    }

    // Integral floats are OK, other floats are not.
    expect_valid_from(nlohmann::json(5.0), T(5));
    expect_valid_from(nlohmann::json::parse("5.0"), T(5));
    expect_valid_from(nlohmann::json::parse("5e2"), T(500));
    expect_valid_from(nlohmann::json(-0.0), T(0));
    expect_throwing_from<T>(nlohmann::json(5.5));
    expect_throwing_from<T>(nlohmann::json(0.5));
    expect_throwing_from<T>(nlohmann::json(-0.5));
    expect_throwing_from<T>(nlohmann::json(1e300));
    expect_throwing_from<T>(nlohmann::json(-1e300));
    expect_throwing_from<T>(nlohmann::json(std::numeric_limits<double>::infinity()));
    expect_throwing_from<T>(nlohmann::json(-std::numeric_limits<double>::infinity()));
    expect_throwing_from<T>(nlohmann::json(std::numeric_limits<double>::quiet_NaN()));
    if constexpr (std::is_signed_v<T>) {
        expect_valid_from(nlohmann::json(-5.0), T(-5));
    } else {
        expect_throwing_from<T>(nlohmann::json(-5.0));
    }

    // Range checks for floats. Max values of 64-bit types are not representable as a double, so we check the edges
    // with powers of two, which are.
    constexpr double limit = 2.0 * static_cast<double>(limits::max() / 2 + 1);
    expect_throwing_from<T>(nlohmann::json(limit));
    if constexpr (sizeof(T) < sizeof(std::int64_t)) {
        expect_valid_from(nlohmann::json(static_cast<double>(limits::max())), limits::max());
        expect_valid_from(nlohmann::json(static_cast<double>(limits::min())), limits::min());
        expect_throwing_from<T>(nlohmann::json(static_cast<double>(limits::max()) + 1));
        expect_throwing_from<T>(nlohmann::json(static_cast<double>(limits::min()) - 1));
    } else {
        expect_valid_from(nlohmann::json(limit / 2), static_cast<T>(limits::max() / 2 + 1));
        if constexpr (std::is_signed_v<T>)
            expect_valid_from(nlohmann::json(-limit), limits::min());
    }

    // Range checks for integers.
    if constexpr (sizeof(T) < sizeof(std::int64_t)) {
        expect_throwing_from<T>(nlohmann::json(static_cast<std::int64_t>(limits::max()) + 1));
        expect_throwing_from<T>(nlohmann::json(static_cast<std::int64_t>(limits::min()) - 1));
        expect_throwing_from<T>(nlohmann::json::parse(sn::detail::format("{}", static_cast<std::int64_t>(limits::max()) + 1)));
    } else if constexpr (std::is_signed_v<T>) {
        expect_throwing_from<T>(nlohmann::json(static_cast<std::uint64_t>(limits::max()) + 1));
        expect_throwing_from<T>(nlohmann::json::parse("9223372036854775808"));
    } else {
        expect_throwing_from<T>(nlohmann::json(static_cast<std::int64_t>(-1)));
        expect_throwing_from<T>(nlohmann::json(std::numeric_limits<std::int64_t>::min()));
    }

    // Other json types.
    expect_throwing_from<T>(nlohmann::json("1"));
    expect_throwing_from<T>(nlohmann::json(true));
    expect_throwing_from<T>(nlohmann::json(nullptr));
    expect_throwing_from<T>(nlohmann::json::array({1}));
    expect_throwing_from<T>(nlohmann::json::object({{"a", 1}}));
}

TEST(njson, ints) {
    run_integer_tests<short>();
    run_integer_tests<unsigned short>();
    run_integer_tests<int>();
    run_integer_tests<unsigned int>();
    run_integer_tests<long>();
    run_integer_tests<unsigned long>();
    run_integer_tests<long long>();
    run_integer_tests<unsigned long long>();
}

template<class T>
static void run_float_tests() {
    check_supported<T>();

    using limits = std::numeric_limits<T>;

    expect_valid_to(T(0), nlohmann::json(0.0));
    expect_valid_to(T(1.5), nlohmann::json(1.5));
    expect_valid_to(T(-1.5), nlohmann::json(-1.5));
    EXPECT_TRUE(sn::to_njson(T(1)).is_number_float());
    EXPECT_TRUE(std::signbit(sn::to_njson(-T(0)).template get<double>()));

    expect_valid_roundtrip(T(0));
    expect_valid_roundtrip(-T(0));
    expect_valid_roundtrip(T(0.1));
    expect_valid_roundtrip(T(1e10));
    expect_valid_roundtrip(limits::max());
    expect_valid_roundtrip(limits::lowest());
    expect_valid_roundtrip(limits::min());
    expect_valid_roundtrip(limits::denorm_min());
    expect_valid_roundtrip(limits::epsilon());

    // Json doesn't support NaN and infinity.
    expect_throwing_to(limits::infinity());
    expect_throwing_to(-limits::infinity());
    expect_throwing_to(limits::quiet_NaN());
    expect_throwing_from<T>(nlohmann::json(std::numeric_limits<double>::infinity()));
    expect_throwing_from<T>(nlohmann::json(-std::numeric_limits<double>::infinity()));
    expect_throwing_from<T>(nlohmann::json(std::numeric_limits<double>::quiet_NaN()));

    // Integers are OK.
    expect_valid_from(nlohmann::json::parse("1"), T(1));
    expect_valid_from(nlohmann::json::parse("-1"), T(-1));
    expect_valid_from(nlohmann::json(std::numeric_limits<std::uint64_t>::max()), static_cast<T>(std::numeric_limits<std::uint64_t>::max()));
    expect_valid_from(nlohmann::json(std::numeric_limits<std::int64_t>::min()), static_cast<T>(std::numeric_limits<std::int64_t>::min()));

    // Values out of range are not.
    if constexpr (std::is_same_v<T, float>) {
        expect_throwing_from<T>(nlohmann::json(1e300));
        expect_throwing_from<T>(nlohmann::json(-1e300));
        expect_throwing_from<T>(nlohmann::json::parse("1e39"));
    } else {
        expect_valid_from(nlohmann::json(1e300), T(1e300));
    }

    // Other json types.
    expect_throwing_from<T>(nlohmann::json("1"));
    expect_throwing_from<T>(nlohmann::json(true));
    expect_throwing_from<T>(nlohmann::json(nullptr));
    expect_throwing_from<T>(nlohmann::json::array({1.0}));
    expect_throwing_from<T>(nlohmann::json::object());
}

TEST(njson, floats) {
    run_float_tests<float>();
    run_float_tests<double>();
}

TEST(njson, nlohmann_non_finite) {
    // We pin down the way nlohmann::json handles NaN and infinity in this test. Not a test for sn. This is why
    // sn::to_njson fails for them - otherwise they would silently become null in json text.
    EXPECT_EQ(nlohmann::json(std::numeric_limits<double>::quiet_NaN()).dump(), "null");
    EXPECT_EQ(nlohmann::json(std::numeric_limits<double>::infinity()).dump(), "null");
    EXPECT_EQ(nlohmann::json(-std::numeric_limits<double>::infinity()).dump(), "null");

    // Can't cast to void here, GCC doesn't allow to discard results of functions marked with warn_unused_result.
    nlohmann::json parsed;
    EXPECT_ANY_THROW(parsed = nlohmann::json::parse("NaN"));
    EXPECT_ANY_THROW(parsed = nlohmann::json::parse("Infinity"));
}

TEST(njson, messages) {
    auto from_njson_message = [](const nlohmann::json &src) {
        try {
            (void) sn::from_njson<int>(src);
        } catch (const sn::exception &e) {
            return std::string(e.what());
        }
        return std::string("<no exception>");
    };

    EXPECT_EQ(from_njson_message(nlohmann::json("abc")), sn::detail::format("Cannot deserialize json value '\"abc\"' as '{}'", sn::type_name<int>()));
    EXPECT_EQ(from_njson_message(nlohmann::json(1.5)), sn::detail::format("Cannot deserialize json value '1.5' as '{}'", sn::type_name<int>()));
    EXPECT_EQ(from_njson_message(nlohmann::json(nullptr)), sn::detail::format("Cannot deserialize json value 'null' as '{}'", sn::type_name<int>()));

    // Long values are cut, non-ASCII chars are escaped.
    std::string long_string = std::string(100, 'a');
    EXPECT_EQ(from_njson_message(nlohmann::json(long_string)), sn::detail::format("Cannot deserialize json value '\"{}...' as '{}'", std::string(60, 'a'), sn::type_name<int>()));
    EXPECT_EQ(from_njson_message(nlohmann::json("\xc3\xa9")), sn::detail::format("Cannot deserialize json value '\"\\u00e9\"' as '{}'", sn::type_name<int>()));
    EXPECT_EQ(from_njson_message(nlohmann::json("\xff")), sn::detail::format("Cannot deserialize json value '\"\\ufffd\"' as '{}'", sn::type_name<int>()));

    try {
        (void) sn::to_njson(std::numeric_limits<double>::infinity());
        ADD_FAILURE() << "Expected an exception";
    } catch (const sn::exception &e) {
        EXPECT_EQ(std::string_view(e.what()), sn::detail::format("Cannot serialize 'inf' of type '{}' to json", sn::type_name<double>()));
    }
}

namespace friendlyns {
struct friendly {
    friendly() = default;
    explicit friendly(int value) : value(value) {}
    friend auto operator<=>(const friendly &, const friendly &) = default;

    SN_DECLARE_FRIEND_NJSON_FUNCTIONS(friendly)

    int value = 0;
};

void to_njson(const friendly &src, nlohmann::json *dst) {
    sn::to_njson(src.value, dst);
}

void from_njson(const nlohmann::json &src, friendly *dst) {
    sn::from_njson(src, &dst->value);
}
} // namespace friendlyns

TEST(njson, friend) {
    EXPECT_EQ(sn::to_njson(friendlyns::friendly(1)), nlohmann::json(1));
    EXPECT_EQ(sn::from_njson<friendlyns::friendly>(nlohmann::json(1)), friendlyns::friendly(1));
}

class Base {};
class Derived : public Base {};
SN_DECLARE_NJSON_FUNCTIONS(Base)

TEST(njson, slicing) {
    check_supported<Base>();
    check_unsupported<Derived>();
}

namespace nlohmannns {
// Type with nlohmann-style conversion functions.
struct nlohmann_only {
    int value = 0;
};

inline void to_json(nlohmann::json &dst, const nlohmann_only &src) { // NOLINT: nlohmann API uses non-const references.
    dst = src.value;
}

inline void from_json(const nlohmann::json &src, nlohmann_only &dst) { // NOLINT: nlohmann API uses non-const references.
    dst.value = src.get<int>();
}

// Type with both sn-style and nlohmann-style conversion functions.
struct both {
    int value = 0;
};

inline void to_json(nlohmann::json &dst, const both &src) { // NOLINT: nlohmann API uses non-const references.
    dst = "nlohmann";
}

inline void from_json(const nlohmann::json &, both &dst) { // NOLINT: nlohmann API uses non-const references.
    dst.value = -1;
}

SN_DECLARE_NJSON_FUNCTIONS(both)

bool try_to_njson(const both &src, nlohmann::json *dst) noexcept {
    *dst = src.value;
    return true;
}

void to_njson(const both &src, nlohmann::json *dst) {
    *dst = src.value;
}

bool try_from_njson(const nlohmann::json &src, both *dst) noexcept {
    return sn::try_from_njson(src, &dst->value);
}

void from_njson(const nlohmann::json &src, both *dst) {
    sn::from_njson(src, &dst->value);
}
} // namespace nlohmannns

TEST(njson, nlohmann_extension_points) {
    // nlohmann::json can convert these, but sn doesn't use nlohmann's conversions.
    check_unsupported<nlohmannns::nlohmann_only>();
    check_unsupported<std::set<int>>();
    check_unsupported<std::list<int>>();
    check_unsupported<nlohmann::ordered_json>();

    // sn and nlohmann extension points don't interfere with each other.
    check_supported<nlohmannns::both>();
    EXPECT_EQ(sn::to_njson(nlohmannns::both{1}), nlohmann::json(1));
    EXPECT_EQ(sn::from_njson<nlohmannns::both>(nlohmann::json(2)).value, 2);
    EXPECT_EQ(nlohmann::json(nlohmannns::both{1}), nlohmann::json("nlohmann"));
    EXPECT_EQ(nlohmann::json(1).get<nlohmannns::both>().value, -1);
}
