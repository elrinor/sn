#include <gtest/gtest.h>

#include <functional> // For std::identity.

#include "sn/detail/test/integer_test_suite.h"

#include "string.h"

template<class T>
struct string_callback {
    [[nodiscard]] bool try_to(const T &src, std::string *dst) noexcept {
        return sn::try_to_string(src, dst);
    }

    [[nodiscard]] std::string to(const T &src) {
        return sn::to_string(src);
    }

    [[nodiscard]] bool try_from(std::string_view src, T *dst) noexcept {
        return sn::try_from_string(src, dst);
    }

    [[nodiscard]] T from(std::string_view src) {
        return sn::from_string<T>(src);
    }
};

template<class T>
static void run_pointer_tests() {
    // This one is checking that to_string with non-char pointers doesn't compile.
    EXPECT_FALSE(sn::stringable<char32_t[4]>);
    EXPECT_FALSE(sn::stringable<char16_t[4]>);
    EXPECT_FALSE(sn::stringable<char8_t[4]>);
    EXPECT_FALSE(sn::stringable<wchar_t[4]>);
    EXPECT_FALSE(sn::stringable<const char32_t *>);
    EXPECT_FALSE(sn::stringable<const char16_t *>);
    EXPECT_FALSE(sn::stringable<const char8_t *>);
    EXPECT_FALSE(sn::stringable<const wchar_t *>);

    // Same checks for from_string, albeit this one is more of a sanity check as the first arg is always a std::string_view.
    EXPECT_FALSE(requires(T s) { sn::builtins::from_string(U"123", &s); });
    EXPECT_FALSE(requires(T s) { sn::builtins::from_string(u"123", &s); });
    EXPECT_FALSE(requires(T s) { sn::builtins::from_string(u8"123", &s); });
    EXPECT_FALSE(requires(T s) { sn::builtins::from_string(L"123", &s); });

    // And we also do some sanity checks for non-char pointers.
    EXPECT_FALSE(sn::stringable<void *>);
    EXPECT_FALSE(sn::stringable<int *>);
    EXPECT_FALSE(sn::stringable<const void *>);
    EXPECT_FALSE(sn::stringable<const int *>);

    // And check that char * is supported.
    EXPECT_TRUE(sn::stringable<char[4]>);
    EXPECT_TRUE(sn::stringable<const char *>);
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
    EXPECT_FALSE(sn::stringable<char>);
    EXPECT_FALSE(sn::stringable<unsigned char>);
    EXPECT_FALSE(sn::stringable<signed char>);
}

TEST(string, boolean) {
    EXPECT_EQ(sn::from_string<bool>(sn::to_string(true)), true);
    EXPECT_EQ(sn::from_string<bool>(sn::to_string(false)), false);
    EXPECT_EQ(sn::from_string<bool>("0"), false);
    EXPECT_EQ(sn::from_string<bool>("1"), true);

    EXPECT_ANY_THROW((void) sn::from_string<bool>("da"));
    EXPECT_ANY_THROW((void) sn::from_string<bool>(""));
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
    sn::detail::make_integer_test_suite<T, std::string>(std::identity()).run(string_callback<T>());
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

TEST(string, floats) {
    EXPECT_EQ(sn::from_string<float>(sn::to_string(1.5)), 1.5);
    EXPECT_EQ(sn::to_string(1.5), "1.5");
    EXPECT_EQ(sn::to_string(-1.5), "-1.5");
    EXPECT_EQ(sn::from_string<float>("0.5"), 0.5);
    EXPECT_EQ(sn::from_string<float>(".5"), 0.5);
    EXPECT_EQ(sn::from_string<float>("0"), 0);
    EXPECT_EQ(sn::from_string<float>("0.0"), 0);
    EXPECT_EQ(sn::from_string<float>(prepend_zeros(100, "0.0")), 0);
    EXPECT_EQ(sn::from_string<float>("1.0e2"), 100.0);
    EXPECT_EQ(sn::from_string<float>("1.e2"), 100.0);
    EXPECT_EQ(sn::from_string<float>(".1e2"), 10.0);
    EXPECT_EQ(sn::from_string<float>(".1e+2"), 10.0);
    EXPECT_EQ(sn::from_string<float>("5.0e-1"), 0.5);
    EXPECT_EQ(sn::from_string<float>(".1e+2"), 10.0);
    EXPECT_EQ(sn::from_string<float>("5.0e-1"), 0.5);
    EXPECT_EQ(sn::from_string<float>(".1e+" + prepend_zeros(100, "2")), 10.0);
    EXPECT_EQ(sn::from_string<float>("5.0e-" + prepend_zeros(100, "1")), 0.5);

    EXPECT_ANY_THROW((void) sn::from_string<float>("+1.5"));
    EXPECT_ANY_THROW((void) sn::from_string<float>(" 1.5"));
    EXPECT_ANY_THROW((void) sn::from_string<float>("1.5 "));
    EXPECT_ANY_THROW((void) sn::from_string<float>("\t10.0000"));
    EXPECT_ANY_THROW((void) sn::from_string<float>("10.0000\t"));
}

namespace friendlyns {
struct friendly {
    friendly() = default;
    explicit friendly(int value) : value(value) {}
    friend auto operator<=>(const friendly &, const friendly &) = default;

    SN_DECLARE_FRIEND_STRING_FUNCTIONS(friendly)

    int value = 0;
};

void to_string(const friendly &src, std::string *dst) {
    sn::to_string(src.value, dst);
}

void from_string(std::string_view src, friendly *dst) {
    sn::from_string(src, &dst->value);
}
} // namespace friendlyns

TEST(string, friend) {
    EXPECT_EQ(sn::to_string(friendlyns::friendly(1)), "1");
    EXPECT_EQ(sn::from_string<friendlyns::friendly>("1"), friendlyns::friendly(1));
}
