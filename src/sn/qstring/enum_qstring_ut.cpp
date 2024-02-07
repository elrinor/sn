#include <gtest/gtest.h>

#include "sn/detail/test/enum_test_suite.h"

#include "qstring_ut.h"
#include "enum_qstring.h"

namespace sn::detail {
SN_DEFINE_ENUM_QSTRING_FUNCTIONS(basic_test_enum, sn::case_sensitive)
} // namespace sn::detail

TEST(enum, basic) {
    sn::detail::make_basic_enum_test_suite().run<QString>();
}

namespace sn::detail {
SN_DEFINE_ENUM_QSTRING_FUNCTIONS(ci_test_enum, sn::case_insensitive)
} // namespace sn::detail

TEST(enum, case_insensitive) {
    sn::detail::make_ci_enum_test_suite().run<QString>();
}

namespace sn::detail {
SN_DEFINE_ENUM_QSTRING_FUNCTIONS(compat_ci_test_enum, sn::case_insensitive)
} // namespace sn::detail

TEST(enum, compatibility) {
    sn::detail::make_compat_ci_enum_test_suite().run<QString>();
}

namespace sn::detail {
SN_DEFINE_ENUM_QSTRING_FUNCTIONS(int, sn::case_insensitive, gl1_test_tag)
SN_DEFINE_ENUM_QSTRING_FUNCTIONS(int, sn::case_insensitive, gl2_test_tag)
} // namespace sn::detail

TEST(enum, tagged) {
    sn::detail::make_tagged_enum_test_suite_1().run<QString>();
    sn::detail::make_tagged_enum_test_suite_2().run<QString>();
}

namespace sn::detail {
SN_DEFINE_ENUM_QSTRING_FUNCTIONS(char_test_enum, sn::case_insensitive)
} // namespace sn::detail

TEST(enum, char) {
    sn::detail::make_char_enum_test_suite().run<QString>();
}

namespace sn::detail {
SN_DEFINE_ENUM_QSTRING_FUNCTIONS(schar_test_enum, sn::case_sensitive)
} // namespace sn::detail

TEST(enum, signed_char) {
    sn::detail::make_schar_enum_test_suite().run<QString>();
}

namespace sn::detail::test_ns {
SN_DEFINE_ENUM_QSTRING_FUNCTIONS(adl_test_enum, sn::case_sensitive)
} // namespace sn::detail::test_ns
namespace sn::detail {
SN_DEFINE_ENUM_QSTRING_FUNCTIONS(adl_test_enum, sn::case_sensitive) // This should compile & hook into ADL-found reflection
} // namespace sn::detail

TEST(enum, namespaces) {
    sn::detail::make_adl_enum_test_suite().run<QString>();

    // sn::detail functions work and hook into the right reflection, despite being in the wrong namespace.
    sn::detail::adl_test_enum value = sn::detail::ADL_VALUE_0;
    QString string;
    EXPECT_TRUE(sn::detail::try_from_qstring(u"_1", &value));
    EXPECT_EQ(value, sn::detail::ADL_VALUE_1);
    EXPECT_TRUE(sn::detail::try_to_qstring(sn::detail::ADL_VALUE_1, &string));
    EXPECT_EQ(string, u"_1");

    value = sn::detail::ADL_VALUE_0;
    string.clear();
    EXPECT_NO_THROW(sn::detail::from_qstring(u"_1", &value));
    EXPECT_EQ(value, sn::detail::ADL_VALUE_1);
    EXPECT_NO_THROW(sn::detail::to_qstring(sn::detail::ADL_VALUE_1, &string));
    EXPECT_EQ(string, u"_1");
}
