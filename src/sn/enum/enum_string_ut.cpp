#include <gtest/gtest.h>

#include "enum_reflection.h"
#include "enum_string.h"

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

SN_DEFINE_ENUM_STRING_FUNCTIONS(TestEnum, sn::case_sensitive)

TEST(enum, basic) {
    EXPECT_EQ(sn::to_string(VALUE_1), "aaa");
    EXPECT_EQ(sn::to_string(VALUE_3), "CCC");
    EXPECT_ANY_THROW((void) sn::to_string(UNSERIALIZABLE));

    EXPECT_EQ(sn::from_string<TestEnum>("aaa"), VALUE_1);
    EXPECT_EQ(sn::from_string<TestEnum>("bbb"), VALUE_2);
    EXPECT_EQ(sn::from_string<TestEnum>("CCC"), VALUE_3);
    EXPECT_ANY_THROW((void) sn::from_string<TestEnum>("AAA"));
    EXPECT_ANY_THROW((void) sn::from_string<TestEnum>("ccc"));
    EXPECT_ANY_THROW((void) sn::from_string<TestEnum>("1"));
    EXPECT_ANY_THROW((void) sn::from_string<TestEnum>("2"));
    EXPECT_ANY_THROW((void) sn::from_string<TestEnum>("3"));
    EXPECT_ANY_THROW((void) sn::from_string<TestEnum>(" aaa"));
    EXPECT_ANY_THROW((void) sn::from_string<TestEnum>("aaa "));
    EXPECT_ANY_THROW((void) sn::from_string<TestEnum>(" aaa "));
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
SN_DEFINE_ENUM_STRING_FUNCTIONS(CiTestEnum, sn::case_insensitive)

TEST(enum, case_insensitive) {
    EXPECT_EQ(sn::to_string(CS_VALUE_1), "AAA");
    EXPECT_EQ(sn::from_string<CiTestEnum>("AAA"), CS_VALUE_1);
    EXPECT_EQ(sn::from_string<CiTestEnum>("AaA"), CS_VALUE_1);
    EXPECT_EQ(sn::from_string<CiTestEnum>("aaa"), CS_VALUE_1);

    EXPECT_EQ(sn::to_string(CS_VALUE_2), "bbb");
    EXPECT_EQ(sn::from_string<CiTestEnum>("BBB"), CS_VALUE_2);

    EXPECT_EQ(sn::to_string(CS_VALUE_3), "Ccc");
    EXPECT_EQ(sn::from_string<CiTestEnum>("ccc"), CS_VALUE_3);

    EXPECT_EQ(sn::to_string(CS_VALUE_4), "111_ab");
    EXPECT_EQ(sn::from_string<CiTestEnum>("111_AB"), CS_VALUE_4);
}


//
// Meddling with `using namespace`.
//

namespace ns1 {
enum class Enum1 {
    NSZERO_1 = 0,
    NSVALUE_1 = 100,
};
using enum Enum1;
SN_DEFINE_ENUM_REFLECTION(Enum1, ({{NSVALUE_1, "100"}}))
SN_DEFINE_ENUM_STRING_FUNCTIONS(Enum1, sn::case_sensitive)
} // namespace ns1

namespace ns2 {
enum class Enum2 {
    NSVALUE_2 = 200
};
using enum Enum2;

using namespace ns1; // NOLINT: Intentional, we're testing that using-namespace doesn't interfere with anything.
SN_DEFINE_ENUM_REFLECTION(Enum2, ({{NSVALUE_2, "200"}}))
SN_DEFINE_ENUM_STRING_FUNCTIONS(Enum2, sn::case_sensitive)

// This will compile, but please don't do this in your code. There are ways to make it fail to compile, e.g. by
// adding something like this:
//
// static_assert(sn::detail::is_reflected_enum_v<T>, "Please don't be stupid");
//
// However, this won't work out of the box b/c you'll be violating ODR. Need to pass a local tag type to
// is_reflected_enum_v every time it's called to differentiate between call sites. So, a mess, and not really worth it.
SN_DEFINE_ENUM_REFLECTION(Enum1, ({{NSVALUE_1, "WUT"}}))

// This will compile & hook into ADL-found reflection, not the one above.
SN_DEFINE_ENUM_STRING_FUNCTIONS(Enum1, sn::case_sensitive)
} // namespace ns2

TEST(enum, namespaces) {
    EXPECT_EQ(sn::to_string(ns1::NSVALUE_1), "100");
    EXPECT_EQ(sn::to_string(ns2::NSVALUE_2), "200");
    EXPECT_EQ(sn::to_string(ns2::NSVALUE_1), "100");
    EXPECT_EQ(sn::from_string<ns2::Enum1>("100"), ns2::NSVALUE_1);

    // ns2/Enum1 functions work and do hook into the right reflection, despite being in the wrong namespace.
    ns2::Enum1 enum1 = ns2::NSZERO_1;
    std::string string1;
    EXPECT_TRUE(ns2::try_from_string("100", &enum1));
    EXPECT_EQ(enum1, ns2::NSVALUE_1);
    EXPECT_TRUE(ns2::try_to_string(ns2::NSVALUE_1, &string1));
    EXPECT_EQ(string1, "100");
    enum1 = ns2::NSZERO_1;
    string1.clear();
    EXPECT_NO_THROW(ns2::from_string("100", &enum1));
    EXPECT_EQ(enum1, ns2::NSVALUE_1);
    EXPECT_NO_THROW(ns2::to_string(ns2::NSVALUE_1, &string1));
    EXPECT_EQ(string1, "100");
}


//
// Tagged enum string conversions.
//

struct glenum_1 {};

SN_DEFINE_ENUM_REFLECTION(int, ({{1, "GL_1"}, {2, "GL_2"}}), glenum_1)
SN_DEFINE_ENUM_STRING_FUNCTIONS(int, sn::case_insensitive, glenum_1)

struct glenum_2 {};

SN_DEFINE_ENUM_REFLECTION(int, ({{100, "GL_100"}, {200, "GL_200"}}), glenum_2)
SN_DEFINE_ENUM_STRING_FUNCTIONS(int, sn::case_insensitive, glenum_2)

TEST(enum, tagged) {
    EXPECT_EQ(sn::to_string(1, glenum_1()), "GL_1");
    EXPECT_EQ(sn::from_string<int>("GL_1", glenum_1()), 1);

    std::string str;
    int val = 0;
    EXPECT_TRUE(sn::try_to_string(2, &str, glenum_1()));
    EXPECT_EQ(str, "GL_2");
    EXPECT_TRUE(sn::try_from_string(str, &val, glenum_1()));
    EXPECT_EQ(val, 2);

    sn::to_string(100, &str, glenum_2());
    EXPECT_EQ(str, "GL_100");
    sn::from_string(str, &val, glenum_2());
    EXPECT_EQ(val, 100);

    EXPECT_ANY_THROW((void) sn::to_string(3, glenum_1()));
    EXPECT_ANY_THROW((void) sn::from_string<int>("GL_100", glenum_1()));
}


//
// Test that enum T : char works, despite there being no sn::to_string overload for char.
//

enum CharEnum : char {
    CHAR_1 = '1',
    CHAR_2 = '2',
    CHAR_UNK = '@'
};

SN_DEFINE_ENUM_REFLECTION(CharEnum, ({{CHAR_1, "CHAR_1"}, {CHAR_2, "CHAR_2"}}))
SN_DEFINE_ENUM_STRING_FUNCTIONS(CharEnum, sn::case_insensitive)

TEST(enum, char) {
    EXPECT_EQ(sn::to_string(CHAR_1), "CHAR_1");
    EXPECT_EQ(sn::from_string<CharEnum>("CHAR_2"), '2');

    EXPECT_ANY_THROW((void) sn::to_string(CHAR_UNK));
    try {
        (void) sn::to_string(CHAR_UNK);
    } catch (const std::exception &e) {
        // 64 is the ascii code for '@'.
        EXPECT_NE(std::string_view(e.what()).find("'64'"), std::string_view::npos) << e.what();
    }
}


//
// Same as above but for signed char.
//

enum SignedCharEnum : signed char {
    SCHAR_1 = -1,
    SCHAR_UNK = -100
};

SN_DEFINE_ENUM_REFLECTION(SignedCharEnum, ({{SCHAR_1, "SCHAR_1"}}))
SN_DEFINE_ENUM_STRING_FUNCTIONS(SignedCharEnum, sn::case_sensitive)

TEST(enum, signed_char) {
    EXPECT_EQ(sn::to_string(SCHAR_1), "SCHAR_1");
    EXPECT_EQ(sn::from_string<SignedCharEnum>("SCHAR_1"), -1);

    EXPECT_ANY_THROW((void) sn::to_string(SCHAR_UNK));
    try {
        (void) sn::to_string(SCHAR_UNK);
    } catch (const std::exception &e) {
        EXPECT_NE(std::string_view(e.what()).find("'-100'"), std::string_view::npos) << e.what();
    }
}


//
// Test compatibility conversion support.
//

enum class CompatEnum {
    COMPAT_1,
    COMPAT_2,
};
using enum CompatEnum;

SN_DEFINE_ENUM_REFLECTION(CompatEnum, ({
    {COMPAT_1, "COMPAT_1"},
    {COMPAT_2, "COMPAT_2"},
    {COMPAT_1, "OLD_1"},
    {COMPAT_2, "OLD_2"},
}))
SN_DEFINE_ENUM_STRING_FUNCTIONS(CompatEnum, sn::case_insensitive)

TEST(enum, compatibility) {
    EXPECT_EQ(sn::to_string(COMPAT_1), "COMPAT_1");
    EXPECT_EQ(sn::from_string<CompatEnum>("compat_1"), COMPAT_1);
    EXPECT_EQ(sn::from_string<CompatEnum>("OLD_1"), COMPAT_1);
    EXPECT_EQ(sn::from_string<CompatEnum>("old_1"), COMPAT_1);

    EXPECT_EQ(sn::to_string(COMPAT_2), "COMPAT_2");
    EXPECT_EQ(sn::from_string<CompatEnum>("compat_2"), COMPAT_2);
    EXPECT_EQ(sn::from_string<CompatEnum>("OLD_2"), COMPAT_2);
    EXPECT_EQ(sn::from_string<CompatEnum>("old_2"), COMPAT_2);
}



