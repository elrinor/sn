#include <gtest/gtest.h>

#include "string.h"

template<class T>
void run_pointer_tests() {
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

TEST(string, ints) {
    EXPECT_EQ(sn::from_string<int>(sn::to_string(0)), 0);
    EXPECT_EQ(sn::from_string<int>(sn::to_string(INT_MAX)), INT_MAX);
    EXPECT_EQ(sn::from_string<int>(sn::to_string(INT_MIN)), INT_MIN);
    EXPECT_EQ(sn::from_string<unsigned int>(sn::to_string(UINT_MAX)), UINT_MAX);
    EXPECT_EQ(sn::from_string<unsigned long long>(sn::to_string(ULLONG_MAX)), ULLONG_MAX);

    EXPECT_EQ(sn::from_string<int>("0100"), 100);
    EXPECT_EQ(sn::from_string<int>(std::string(100, '0') + sn::to_string(INT_MAX)), INT_MAX);

    EXPECT_ANY_THROW((void) sn::from_string<int>(""));
    EXPECT_ANY_THROW((void) sn::from_string<int>(" 100"));
    EXPECT_ANY_THROW((void) sn::from_string<int>("100 "));
    EXPECT_ANY_THROW((void) sn::from_string<int>("+100"));
    EXPECT_ANY_THROW((void) sn::from_string<int>("0x200"));
    EXPECT_ANY_THROW((void) sn::from_string<int>("0b100"));
}

TEST(string, floats) {
    EXPECT_EQ(sn::from_string<float>(sn::to_string(1.5)), 1.5);
    EXPECT_EQ(sn::to_string(1.5), "1.5");

    EXPECT_ANY_THROW((void) sn::from_string<float>(" 1.5"));
    EXPECT_ANY_THROW((void) sn::from_string<float>("1.5 "));
}
