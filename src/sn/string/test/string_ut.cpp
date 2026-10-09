#include <limits>
#include <string>
#include <string_view>
#include <functional> // For std::identity.
#include <type_traits>

#include <gtest/gtest.h> // NOLINT: not a C system header.

#include "sn/string/string.h"
#include "sn/string/detail/small_string_capacity.h"

#include "tester.h"

template<class T>
static void check_supported() {
    static_assert(sn::concepts::to_stringable<T>);
    static_assert(sn::concepts::from_stringable<T>);
}

template<class T>
static void check_unsupported() {
    static_assert(!sn::concepts::to_stringable<T>);
    static_assert(!sn::concepts::from_stringable<T>);
}

template<class T>
static void check_has_to_string_only() {
    static_assert(sn::concepts::to_stringable<T>);
    static_assert(!sn::concepts::from_stringable<T>);
}

template<class T>
static void run_pointer_tests() {
    // Check all char pointers & array types.
    check_has_to_string_only<char[4]>();
    check_unsupported<char8_t[4]>();
    check_unsupported<char16_t[4]>();
    check_unsupported<char32_t[4]>();
    check_unsupported<wchar_t[4]>();
    check_has_to_string_only<const char *>();
    check_unsupported<const char8_t *>();
    check_unsupported<const char16_t *>();
    check_unsupported<const char32_t *>();
    check_unsupported<const wchar_t *>();
    check_has_to_string_only<char *>();
    check_unsupported<char8_t *>();
    check_unsupported<char16_t *>();
    check_unsupported<char32_t *>();
    check_unsupported<wchar_t *>();

    // Same checks for from_string, albeit this one is more of a sanity check as the first arg is always a std::string_view.
    static_assert(requires(T s) { sn::detail::builtins::from_string("123", &s, nullptr); });
    static_assert(!requires(T s) { sn::detail::builtins::from_string(u8"123", &s, nullptr); });
    static_assert(!requires(T s) { sn::detail::builtins::from_string(u"123", &s, nullptr); });
    static_assert(!requires(T s) { sn::detail::builtins::from_string(U"123", &s, nullptr); });
    static_assert(!requires(T s) { sn::detail::builtins::from_string(L"123", &s, nullptr); });

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
    check_unsupported<const unsigned char *>();
}

TEST(string, string) {
    run_pointer_tests<std::string>();

    // Char strings work.
    const char *str = "1234";
    EXPECT_EQ(sn::to_string(str), "1234");
    EXPECT_EQ(sn::to_string("123"), "123");
    EXPECT_EQ(sn::to_string(std::string("123")), "123");
    EXPECT_EQ(sn::to_string(std::string_view("123")), "123");
    EXPECT_EQ(sn::from_string<std::string>("123"), "123");
    EXPECT_EQ(sn::from_string<std::string>(std::string("123")), "123");
    EXPECT_EQ(sn::from_string<std::string>(std::string_view("123")), "123");
}

TEST(string, char) {
    check_unsupported<char>();
    check_unsupported<unsigned char>();
    check_unsupported<signed char>();
}

TEST(string, boolean) {
    sn::detail::tester<bool> t;

    t.expect_failing_from({
        "",
        "da"
    });
    t.expect_failing_from_with_message("da", "Cannot deserialize 'da' as 'bool'");

    t.expect_valid_from({
        {"0", false},
        {"1", true}
    });

    t.expect_valid_fromto({
        {"true", true},
        {"false", false}
    });
}

static std::string prepend_zeros(int zeros, std::string_view number_string) {
    std::string result;

    if (number_string.starts_with('-')) {
        result = "-";
        number_string = number_string.substr(1);
    }

    result += std::string(zeros, '0');
    result += number_string;
    return result;
}

template<class T>
static void run_integer_tests() {
    sn::detail::tester<T> t;

    std::initializer_list<std::string_view> always_failing = {
        "",
        " 1",
        "1 ",
        "\t1",
        "1\t",
        "+1",
        "--1",
        "0-0",
        "0-1",
    };
    t.expect_failing_from(always_failing);

    std::initializer_list<std::string_view> failing_in_base10 = {
        " 111",
        "111 ",
        "\t111",
        "111\t",
        "0x1",
        "0b1",
    };
    t.expect_failing_from(failing_in_base10);

    if constexpr (sizeof(T) < sizeof(long long)) {
        t.expect_failing_from({
            sn::to_string(static_cast<long long>(std::numeric_limits<T>::max()) + 1).value(),
            sn::to_string(static_cast<long long>(std::numeric_limits<T>::min()) - 1).value()
        });
    } else {
        static_assert(sizeof(T) == 8);

        if constexpr (std::is_unsigned_v<T>) {
            t.expect_failing_from({
                "-1",
                "18446744073709551616" // max unsigned long long +1
            });
        } else {
            t.expect_failing_from({
                "-9223372036854775809", // min long long -1
                "9223372036854775808" // max long long +1
            });
        }
    }

    t.expect_valid_fromto({
        {"0", 0},
        {"1", 1},
        {"100", 100}
    });
    if constexpr (std::is_signed_v<T>)
        t.expect_valid_fromto({{"-1", -1}});

    t.expect_valid_from({
        {"010", 10}, // This is not an octal number.
        {prepend_zeros(1, sn::to_string(std::numeric_limits<T>::max()).value()), std::numeric_limits<T>::max()},
        {prepend_zeros(1, sn::to_string(std::numeric_limits<T>::min()).value()), std::numeric_limits<T>::min()},
        {prepend_zeros(100, sn::to_string(std::numeric_limits<T>::max()).value()), std::numeric_limits<T>::max()},
        {prepend_zeros(100, sn::to_string(std::numeric_limits<T>::min()).value()), std::numeric_limits<T>::min()}
    });

    t.expect_valid_roundtrip({
        std::numeric_limits<T>::max(),
        std::numeric_limits<T>::min()
    });

    static constexpr const char *base_strings_for_100[] = {
        nullptr,
        nullptr,
        "1100100",
        "10201",
        "1210",
        "400",
        "244",
        "202",
        "144",
        "121",
        "100",
        "91",
        "84",
        "79",
        "72",
        "6a",
        "64",
        "5f",
        "5a",
        "55",
        "50",
        "4g",
        "4c",
        "48",
        "44",
        "40",
        "3m",
        "3j",
        "3g",
        "3d",
        "3a",
        "37",
        "34",
        "31",
        "2w",
        "2u",
        "2s"
    };

    auto run_base_tests = [&](int base, auto tag) {
        std::string positive_100 = base_strings_for_100[base];
        t.expect_valid_fromto(positive_100, 100, tag);

        // Longest strings that to_string can produce for T in this base.
        t.expect_valid_roundtrip({std::numeric_limits<T>::max(), std::numeric_limits<T>::min()}, tag);
        t.expect_valid_from(prepend_zeros(100, positive_100), 100, tag);

        t.expect_failing_from(always_failing, tag);
        if (base == 10)
            t.expect_failing_from(failing_in_base10, tag);

        if (base <= 11) {
            t.expect_failing_from("0b1", tag);
        } else {
            t.expect_succeeding_from("0b1", tag);
        }

        if (base <= 33) {
            t.expect_failing_from("0x1", tag);
        } else {
            t.expect_succeeding_from("0x1", tag);
        }

        if (std::is_signed_v<T>) {
            std::string negative_100 = "-" + positive_100;
            t.expect_valid_fromto(negative_100, -100, tag);
            t.expect_valid_from(prepend_zeros(100, negative_100), -100, tag);
        }
    };

    // Test tn::dynamic_base(N).
    for (std::size_t base = 2; base <= 36; base++)
        run_base_tests(base, tn::dynamic_base(base));

    // Test tn::base<N>.
    auto run_static_base_tests = [&]<int... bases>(std::integer_sequence<int, bases...>) {
        (run_base_tests(bases, tn::base<bases>), ...);
    }; // NOLINT: linter chokes here.
    run_static_base_tests(std::integer_sequence<int, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36>());

    // Test base shortcuts in tn::.
    run_base_tests(2, tn::bin);
    run_base_tests(8, tn::oct);
    run_base_tests(10, tn::dec);
    run_base_tests(16, tn::hex);
}

TEST(string, ints) {
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
    sn::detail::tester<T> t;

    t.expect_failing_from({
        "+1",
        "+1.5",
        " 1.5",
        "1.5 ",
        " 1",
        "1 ",
        "\t10.0000",
        "10.0000\t",
    });

    t.expect_valid_from({
        {"0.0", 0.0f},
        {"-0.0", -0.0f},
        {".5", 0.5f},
        {prepend_zeros(100, "0.0"), 0.0f},
        {"1.0e2", 100.0f},
        {"1.e2", 100.0f},
        {".1e2", 10.0f},
        {".1e+2", 10.0f},
        {"5.0e-1", 0.5f},
        {".1e+2", 10.0f},
        {"5.0e-1", 0.5f},
        {".1e+" + prepend_zeros(100, "2"), 10.0f},
        {"5.0e-" + prepend_zeros(100, "1"), 0.5f},
        {"INF", std::numeric_limits<T>::infinity()},
        {"Inf", std::numeric_limits<T>::infinity()},
        {"iNf", std::numeric_limits<T>::infinity()},
        {"inF", std::numeric_limits<T>::infinity()},
        {"-INF", -std::numeric_limits<T>::infinity()},
        {"-Inf", -std::numeric_limits<T>::infinity()},
        {"-iNf", -std::numeric_limits<T>::infinity()},
        {"-inF", -std::numeric_limits<T>::infinity()},
    });

    t.expect_valid_fromto({
        {"0", 0.0f},
        {"-0", -0.0f},
        {"1", 1.0f},
        {"-1", -1.0f},
        {"1.5", 1.5f},
        {"-1.5", -1.5f},
        {"0.5", 0.5f},
        {"inf", std::numeric_limits<T>::infinity()},
        {"-inf", -std::numeric_limits<T>::infinity()},
    });

    // Longest strings that to_string can produce for T.
    if constexpr (std::is_same_v<T, float>) {
        t.expect_valid_fromto({
            {"-3.4028235e+38", std::numeric_limits<float>::lowest()},
            {"-1.1754944e-38", -std::numeric_limits<float>::min()},
            {"-1.00000075e-36", -1.00000075e-36f},
            {"1e-45", std::numeric_limits<float>::denorm_min()},
        });
    } else {
        t.expect_valid_fromto({
            {"-1.7976931348623157e+308", std::numeric_limits<double>::lowest()},
            {"-2.2250738585072014e-308", -std::numeric_limits<double>::min()},
            {"5e-324", std::numeric_limits<double>::denorm_min()},
        });
    }

    t.expect_valid_roundtrip({
        std::numeric_limits<T>::max(),
        std::numeric_limits<T>::lowest(),
        std::numeric_limits<T>::min(),
        -std::numeric_limits<T>::min(),
        std::numeric_limits<T>::denorm_min(),
        -std::numeric_limits<T>::denorm_min(),
        std::numeric_limits<T>::epsilon(),
        static_cast<T>(0.1),
        static_cast<T>(1.0 / 3.0),
        static_cast<T>(-1.0 / 3.0),
    });

    // TODO(elric): test NANs.
}

TEST(string, floats) {
    run_float_tests<float>();
    run_float_tests<double>();
}

namespace friendlyns {
struct friendly {
    friendly() = default;
    explicit friendly(int value) : value(value) {}
    friend auto operator<=>(const friendly &, const friendly &) = default;

    SN_DECLARE_FRIEND_STRING_FUNCTIONS(friendly)

    int value = 0;
};

bool to_string(const friendly &src, std::string *dst, sn::error *err) {
    return sn::to_string(src.value, dst, err);
}

bool from_string(std::string_view src, friendly *dst, sn::error *err) {
    return sn::from_string(src, &dst->value, err);
}
} // namespace friendlyns

TEST(string, friend) { // NOLINT: this is not std::string.
    EXPECT_EQ(sn::to_string(friendlyns::friendly(1)), "1");
    EXPECT_EQ(sn::from_string<friendlyns::friendly>("1"), friendlyns::friendly(1));
    EXPECT_EQ(sn::from_string<friendlyns::friendly>("zz").error().what(), "'zz' is not a number");
}

namespace pointns {
struct point {
    int x = 0;
    int y = 0;
    friend bool operator==(const point &, const point &) = default;
};

SN_DECLARE_STRING_FUNCTIONS(point)

bool to_string(const point &src, std::string *dst, sn::error *err) {
    std::string y;
    if (!sn::to_string(src.x, dst, err) || !sn::to_string(src.y, &y, err))
        return false;
    *dst += ',';
    *dst += y;
    return true;
}

bool from_string(std::string_view src, point *dst, sn::error *err) {
    std::size_t pos = src.find(',');
    if (pos == std::string_view::npos) {
        sn::report_from_string_error<point>(err, src, "missing a comma");
        return false;
    }

    if (!sn::from_string(src.substr(0, pos), &dst->x, err)) {
        sn::error::prepend_path(err, "x");
        return false;
    }

    if (!sn::from_string(src.substr(pos + 1), &dst->y, err)) {
        sn::error::prepend_path(err, "y");
        return false;
    }

    return true;
}
} // namespace pointns

TEST(string, composite) {
    EXPECT_EQ(sn::to_string(pointns::point{1, 2}), "1,2");
    EXPECT_EQ(sn::from_string<pointns::point>("1,2"), (pointns::point{1, 2}));

    sn::expected<pointns::point> result = sn::from_string<pointns::point>("1,zz");
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error().message(), "'zz' is not a number");
    EXPECT_EQ(result.error().path(), "y");
    EXPECT_EQ(result.error().what(), "y: 'zz' is not a number");

    // Error reported by the extension point itself, with a reason.
    sn::expected<pointns::point> no_comma = sn::from_string<pointns::point>("12");
    ASSERT_FALSE(no_comma.has_value());
    EXPECT_TRUE(no_comma.error().message().starts_with("Cannot deserialize '12' as ")) << no_comma.error().message();
    EXPECT_TRUE(no_comma.error().message().ends_with(": missing a comma")) << no_comma.error().message();
    EXPECT_EQ(no_comma.error().path(), "");

    // Speculative parsing doesn't report anything.
    pointns::point p;
    EXPECT_FALSE(sn::from_string("1,zz", &p, nullptr));
}

class Base {};
class Derived : public Base {};
SN_DECLARE_STRING_FUNCTIONS(Base)

TEST(string, slicing) {
    check_supported<Base>();
    check_unsupported<Derived>();
}

TEST(string, small_string_capacity) {
    // to_string relies on this to decide whether it can format numbers right into the output string.
    EXPECT_EQ(std::string().capacity(), sn::detail::small_string_capacity);
}

template<class T, class... Tags>
static void check_no_reallocation(T value, Tags... tags) {
    std::string s;
    std::size_t capacity = s.capacity();
    EXPECT_TRUE(sn::to_string(value, &s, nullptr, tags...));
    EXPECT_EQ(s.capacity(), capacity) << "with value = " << value << " and result = " << s;
}

TEST(string, short_numbers_dont_allocate) {
    // Short results should stay in the small string buffer, regardless of how long the longest result for the type is.
    check_no_reallocation(0.5f);
    check_no_reallocation(0.5);
    check_no_reallocation(-1.5);
    check_no_reallocation(42);
    check_no_reallocation(42ll);
    check_no_reallocation(42ull);
    check_no_reallocation(5, tn::bin);
    check_no_reallocation(5ll, tn::bin);
    check_no_reallocation(5ll, tn::dynamic_base(3));
}

namespace unsupportedns {
struct unsupported {};
} // namespace unsupportedns

TEST(string, unsupported) {
    // If sn::error were declared directly in namespace sn, ADL at sn's own extension point calls would find the
    // user-facing sn::to_string / sn::from_string overloads that take sn::error *, and these checks would fail. See
    // docs/error_handling.md.
    check_unsupported<unsupportedns::unsupported>();
}

TEST(string, number_error_messages) {
    sn::detail::tester<int> ti;
    ti.expect_failing_from_with_message("zz", "'zz' is not a number");
    ti.expect_failing_from_with_message("1zz", "'1zz' is not a number");
    ti.expect_failing_from_with_message("99999999999999999999", "'99999999999999999999' does not fit in the range of int");

    sn::detail::tester<double> td;
    td.expect_failing_from_with_message("zz", "'zz' is not a number");
}
