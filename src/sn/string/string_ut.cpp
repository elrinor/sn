#include <gtest/gtest.h>

#include <limits>

#include "string.h"

template<class T>
static void run_pointer_tests() {
    // This one is checking that to_string with non-char pointers doesn't compile. We have to call directly into
    // sn::builtins because the entrypoint in namespace sn is unconstrained.
    EXPECT_FALSE(requires(T s) { sn::builtins::to_string(U"123", &s); });
    EXPECT_FALSE(requires(T s) { sn::builtins::to_string(u"123", &s); });
    EXPECT_FALSE(requires(T s) { sn::builtins::to_string(u8"123", &s); });
    EXPECT_FALSE(requires(T s) { sn::builtins::to_string(L"123", &s); });

    // Same for from_string, albeit this one is more of a sanity check as the first arg is always a std::string_view.
    EXPECT_FALSE(requires(T s) { sn::builtins::from_string(U"123", &s); });
    EXPECT_FALSE(requires(T s) { sn::builtins::from_string(u"123", &s); });
    EXPECT_FALSE(requires(T s) { sn::builtins::from_string(u8"123", &s); });
    EXPECT_FALSE(requires(T s) { sn::builtins::from_string(L"123", &s); });

    // And we also do some sanity checks for non-char pointers.
    EXPECT_FALSE(requires(T s) { sn::builtins::to_string(static_cast<void *>(nullptr), &s); });
    EXPECT_FALSE(requires(T s) { sn::builtins::to_string(static_cast<int *>(nullptr), &s); });
    EXPECT_FALSE(requires(T s) { sn::builtins::to_string(static_cast<const void *>(nullptr), &s); });
    EXPECT_FALSE(requires(T s) { sn::builtins::to_string(static_cast<const int *>(nullptr), &s); });
}

TEST(string, string) {
    run_pointer_tests<std::string>();

    // Char strings work.
    EXPECT_EQ(sn::to_string("123"), "123");
    EXPECT_EQ(sn::to_string(std::string("123")), "123");
    EXPECT_EQ(sn::to_string(std::string_view("123")), "123");
    EXPECT_EQ(sn::from_string<std::string>("123"), "123");
    EXPECT_EQ(sn::from_string<std::string>(std::string("123")), "123");
    EXPECT_EQ(sn::from_string<std::string>(std::string_view("123")), "123");
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
    EXPECT_EQ(sn::to_string(static_cast<T>(0)), "0");
    EXPECT_EQ(sn::from_string<T>("0"), 0);

    EXPECT_EQ(sn::to_string(static_cast<T>(1)), "1");
    EXPECT_EQ(sn::from_string<T>("1"), 1);

    EXPECT_EQ(sn::to_string(static_cast<T>(100)), "100");
    EXPECT_EQ(sn::from_string<T>("100"), 100);

    EXPECT_EQ(sn::from_string<T>(sn::to_string(std::numeric_limits<T>::max())), std::numeric_limits<T>::max());
    EXPECT_EQ(sn::from_string<T>(sn::to_string(std::numeric_limits<T>::min())), std::numeric_limits<T>::min());

    EXPECT_EQ(sn::from_string<T>(prepend_zeros(1, sn::to_string(std::numeric_limits<T>::max()))), std::numeric_limits<T>::max());
    EXPECT_EQ(sn::from_string<T>(prepend_zeros(1, sn::to_string(std::numeric_limits<T>::min()))), std::numeric_limits<T>::min());

    EXPECT_EQ(sn::from_string<T>(prepend_zeros(100, sn::to_string(std::numeric_limits<T>::max()))), std::numeric_limits<T>::max());
    EXPECT_EQ(sn::from_string<T>(prepend_zeros(100, sn::to_string(std::numeric_limits<T>::min()))), std::numeric_limits<T>::min());

    if constexpr (sizeof(T) < sizeof(long long)) {
        EXPECT_ANY_THROW((void) sn::from_string<T>(sn::to_string(static_cast<long long>(std::numeric_limits<T>::max()) + 1)));
        EXPECT_ANY_THROW((void) sn::from_string<T>(sn::to_string(static_cast<long long>(std::numeric_limits<T>::min()) - 1)));
    } else {
        static_assert(sizeof(T) == 8);

        if constexpr (std::is_unsigned_v<T>) {
            EXPECT_ANY_THROW((void) sn::from_string<T>("-1"));
            EXPECT_ANY_THROW((void) sn::from_string<T>("18446744073709551616")); // max unsigned long long +1
        } else {
            EXPECT_ANY_THROW((void) sn::from_string<T>("–9223372036854775809")); // min long long -1
            EXPECT_ANY_THROW((void) sn::from_string<T>("9223372036854775808")); // max long long +1
        }
    }

    EXPECT_ANY_THROW((void) sn::from_string<T>(""));
    EXPECT_ANY_THROW((void) sn::from_string<T>(" 1"));
    EXPECT_ANY_THROW((void) sn::from_string<T>("1 "));
    EXPECT_ANY_THROW((void) sn::from_string<T>("+1"));
    EXPECT_ANY_THROW((void) sn::from_string<T>("--1"));
    EXPECT_ANY_THROW((void) sn::from_string<T>("0x1"));
    EXPECT_ANY_THROW((void) sn::from_string<T>("0b1"));
    EXPECT_ANY_THROW((void) sn::from_string<T>("0-0"));
    EXPECT_ANY_THROW((void) sn::from_string<T>("0-1"));
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

    EXPECT_ANY_THROW((void) sn::from_string<float>(" 1.5"));
    EXPECT_ANY_THROW((void) sn::from_string<float>("1.5 "));
}
