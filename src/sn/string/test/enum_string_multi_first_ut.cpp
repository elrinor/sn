#include <gtest/gtest.h>

#include "enum_string_multi_ut.h"

TEST(enum, multi_ts_1) {
    // This just checks that we're not getting linker errors when using SN_DEFINE_ENUM_STRING_FUNCTIONS in a header file.
    EXPECT_EQ(sn::to_string(MULTI_1), "OH");
}
