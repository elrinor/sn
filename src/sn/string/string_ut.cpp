#include <gtest/gtest.h>

#include "string.h"

TEST(string, boolean) {
    EXPECT_EQ(sn::from_string<bool>(sn::to_string(true)), true);
    EXPECT_EQ(sn::from_string<bool>(sn::to_string(false)), false);
    EXPECT_EQ(sn::from_string<bool>("0"), false);
    EXPECT_EQ(sn::from_string<bool>("1"), true);
}

TEST(string, ints) {
    EXPECT_EQ(sn::from_string<int>(sn::to_string(0)), 0);
    EXPECT_EQ(sn::from_string<int>(sn::to_string(INT_MAX)), INT_MAX);
    EXPECT_EQ(sn::from_string<int>(sn::to_string(INT_MIN)), INT_MIN);
    EXPECT_EQ(sn::from_string<unsigned int>(sn::to_string(UINT_MAX)), UINT_MAX);
    EXPECT_EQ(sn::from_string<unsigned  long long>(sn::to_string(ULLONG_MAX)), ULLONG_MAX);

    EXPECT_EQ(sn::from_string<int>("0100"), 100);
    EXPECT_EQ(sn::from_string<int>(std::string(100, '0') + sn::to_string(INT_MAX)), INT_MAX);

    EXPECT_ANY_THROW((void)sn::from_string<bool>("da"));
    EXPECT_ANY_THROW((void)sn::from_string<bool>(""));
    EXPECT_ANY_THROW((void)sn::from_string<int>(""));
    EXPECT_ANY_THROW((void)sn::from_string<int>(" 100"));
    EXPECT_ANY_THROW((void)sn::from_string<int>("100 "));
    EXPECT_ANY_THROW((void)sn::from_string<int>("+100"));
    EXPECT_ANY_THROW((void)sn::from_string<int>("0x200"));
    EXPECT_ANY_THROW((void)sn::from_string<int>("0b100"));
}

TEST(string, floats) {
    EXPECT_EQ(sn::from_string<float>(sn::to_string(1.5)), 1.5);
    EXPECT_EQ(sn::to_string(1.5), "1.5");

    EXPECT_ANY_THROW((void)sn::from_string<float>(" 1.5"));
    EXPECT_ANY_THROW((void)sn::from_string<float>("1.5 "));
}
