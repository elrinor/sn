#include <gtest/gtest.h>

#include "ascii_functions.h"

TEST(enum_table, char_to_lower_ascii) {
    EXPECT_EQ(sn::detail::to_lower_ascii('A'), 'a');
    EXPECT_EQ(sn::detail::to_lower_ascii('a'), 'a');
    EXPECT_EQ(sn::detail::to_lower_ascii('Z'), 'z');
    EXPECT_EQ(sn::detail::to_lower_ascii('z'), 'z');

    EXPECT_EQ(sn::detail::to_lower_ascii(' '), ' ');
    EXPECT_EQ(sn::detail::to_lower_ascii('0'), '0');

    for (int i = 128; i < 255; i++)
        EXPECT_EQ(sn::detail::to_lower_ascii(static_cast<char>(i)), static_cast<char>(i));
}
