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

TEST(qenum, basic) {
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


//
// Case-insensitive conversions.
//

enum class CiTestEnum {
    CS_VALUE_1 = 1,
    CS_VALUE_2 = 2,
    CS_VALUE_3 = 3,
    CS_VALUE_4 = 4,
};
using enum CiTestEnum;
SN_DEFINE_ENUM_REFLECTION(CiTestEnum, ({
    {CS_VALUE_1, "AAA"},
    {CS_VALUE_2, "bbb"},
    {CS_VALUE_3, "Ccc"},
    {CS_VALUE_4, "111_ab"},
}))
SN_DEFINE_ENUM_QSTRING_FUNCTIONS(CiTestEnum, sn::case_insensitive)

TEST(qenum, case_insensitive) {
    EXPECT_EQ(sn::to_qstring(CS_VALUE_1), QStringLiteral("AAA"));
    EXPECT_EQ(sn::from_qstring<CiTestEnum>(QStringLiteral("AAA")), CS_VALUE_1);
    EXPECT_EQ(sn::from_qstring<CiTestEnum>(QStringLiteral("AaA")), CS_VALUE_1);
    EXPECT_EQ(sn::from_qstring<CiTestEnum>(QStringLiteral("aaa")), CS_VALUE_1);

    EXPECT_EQ(sn::to_qstring(CS_VALUE_2), QStringLiteral("bbb"));
    EXPECT_EQ(sn::from_qstring<CiTestEnum>(QStringLiteral("BBB")), CS_VALUE_2);

    EXPECT_EQ(sn::to_qstring(CS_VALUE_3), QStringLiteral("Ccc"));
    EXPECT_EQ(sn::from_qstring<CiTestEnum>(QStringLiteral("ccc")), CS_VALUE_3);

    EXPECT_EQ(sn::to_qstring(CS_VALUE_4), QStringLiteral("111_ab"));
    EXPECT_EQ(sn::from_qstring<CiTestEnum>(QStringLiteral("111_AB")), CS_VALUE_4);
}


//
// Tagged enum string conversions.
//

struct glenum_1 {};

SN_DEFINE_ENUM_REFLECTION(int, ({{1, "GL_1"}, {2, "GL_2"}}), glenum_1)
SN_DEFINE_ENUM_QSTRING_FUNCTIONS(int, sn::case_insensitive, glenum_1)

struct glenum_2 {};

SN_DEFINE_ENUM_REFLECTION(int, ({{100, "GL_100"}, {200, "GL_200"}}), glenum_2)
SN_DEFINE_ENUM_QSTRING_FUNCTIONS(int, sn::case_insensitive, glenum_2)

TEST(qenum, tagged) {
    EXPECT_EQ(sn::to_qstring(1, glenum_1()), QStringLiteral("GL_1"));
    EXPECT_EQ(sn::from_qstring<int>(QStringLiteral("GL_1"), glenum_1()), 1);

    QString str;
    int val = 0;
    EXPECT_TRUE(sn::try_to_qstring(2, &str, glenum_1()));
    EXPECT_EQ(str, QStringLiteral("GL_2"));
    EXPECT_TRUE(sn::try_from_qstring(str, &val, glenum_1()));
    EXPECT_EQ(val, 2);

    sn::to_qstring(100, &str, glenum_2());
    EXPECT_EQ(str, QStringLiteral("GL_100"));
    sn::from_qstring(str, &val, glenum_2());
    EXPECT_EQ(val, 100);

    EXPECT_ANY_THROW((void) sn::to_qstring(3, glenum_1()));
    EXPECT_ANY_THROW((void) sn::from_qstring<int>(QStringLiteral("GL_100"), glenum_1()));
}
