#include <gtest/gtest.h>

#include "ascii_functions.h"

TEST(enum, char_to_lower_ascii) {
    EXPECT_EQ(sn::detail::to_lower_ascii('A'), 'a');
    EXPECT_EQ(sn::detail::to_lower_ascii('a'), 'a');
    EXPECT_EQ(sn::detail::to_lower_ascii('Z'), 'z');
    EXPECT_EQ(sn::detail::to_lower_ascii('z'), 'z');

    EXPECT_EQ(sn::detail::to_lower_ascii(' '), ' ');
    EXPECT_EQ(sn::detail::to_lower_ascii('0'), '0');

    for (int i = 128; i < 255; i++)
        EXPECT_EQ(sn::detail::to_lower_ascii(static_cast<char>(i)), static_cast<char>(i));
}

TEST(enum, char16_to_lower_ascii) {
    EXPECT_EQ(sn::detail::to_lower_ascii(u'A'), u'a');
    EXPECT_EQ(sn::detail::to_lower_ascii(u'a'), u'a');
    EXPECT_EQ(sn::detail::to_lower_ascii(u'Z'), u'z');
    EXPECT_EQ(sn::detail::to_lower_ascii(u'z'), u'z');

    EXPECT_EQ(sn::detail::to_lower_ascii(u' '), u' ');
    EXPECT_EQ(sn::detail::to_lower_ascii(u'0'), u'0');

    for (int i = 128; i < 1000; i++)
        EXPECT_EQ(sn::detail::to_lower_ascii(static_cast<char16_t>(i)), static_cast<char16_t>(i));
}
