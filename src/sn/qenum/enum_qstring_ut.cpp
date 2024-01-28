#include <gtest/gtest.h>

#include "enum_qstring.h"

//
// Basic enum string conversions.
//

enum class TestEnum {
    VALUE_1 = 1,
    VALUE_2 = 2,
    VALUE_3 = 3,
    UNSERIALIZABLE = 0x11111111
};
using enum TestEnum;

SN_DEFINE_ENUM_REFLECTION(TestEnum, ({
    { VALUE_1, "aaa" },
    { VALUE_2, "bbb" },
    { VALUE_3, "CCC" },
}))

SN_DEFINE_ENUM_QSTRING_FUNCTIONS(TestEnum, sn::case_sensitive)

TEST(enum, basic) {
    EXPECT_EQ(sn::to_qstring(VALUE_1), QStringLiteral("aaa"));
    EXPECT_EQ(sn::to_qstring(VALUE_3), QStringLiteral("CCC"));
    EXPECT_ANY_THROW((void) sn::to_qstring(UNSERIALIZABLE));

    EXPECT_EQ(sn::from_qstring<TestEnum>(QStringLiteral("aaa")), VALUE_1);
    EXPECT_EQ(sn::from_qstring<TestEnum>(QStringLiteral("bbb")), VALUE_2);
    EXPECT_EQ(sn::from_qstring<TestEnum>(QStringLiteral("CCC")), VALUE_3);
    EXPECT_ANY_THROW((void) sn::from_qstring<TestEnum>(QStringLiteral("AAA")));
    EXPECT_ANY_THROW((void) sn::from_qstring<TestEnum>(QStringLiteral("ccc")));
    EXPECT_ANY_THROW((void) sn::from_qstring<TestEnum>(QStringLiteral("1")));
    EXPECT_ANY_THROW((void) sn::from_qstring<TestEnum>(QStringLiteral("2")));
    EXPECT_ANY_THROW((void) sn::from_qstring<TestEnum>(QStringLiteral("3")));
    EXPECT_ANY_THROW((void) sn::from_qstring<TestEnum>(QStringLiteral(" aaa")));
    EXPECT_ANY_THROW((void) sn::from_qstring<TestEnum>(QStringLiteral("aaa ")));
    EXPECT_ANY_THROW((void) sn::from_qstring<TestEnum>(QStringLiteral(" aaa ")));
}
