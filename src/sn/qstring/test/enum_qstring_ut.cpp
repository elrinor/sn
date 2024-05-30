#include <gtest/gtest.h>

#include "sn/detail/test/enum_test_suite.h"
#include "sn/qstring/enum_qstring.h"

#include "qstring_ut.h"

namespace sn::detail {
SN_DEFINE_ENUM_QSTRING_FUNCTIONS(basic_test_enum, sn::case_sensitive)
} // namespace sn::detail

TEST(enum, basic) {
    sn::detail::run_basic_enum_test_suite(sn::detail::qstring_ops());
}

namespace sn::detail {
SN_DEFINE_ENUM_QSTRING_FUNCTIONS(ci_test_enum, sn::case_insensitive)
} // namespace sn::detail

TEST(enum, case_insensitive) {
    sn::detail::run_ci_enum_test_suite(sn::detail::qstring_ops());
}

namespace sn::detail {
SN_DEFINE_ENUM_QSTRING_FUNCTIONS(compat_ci_test_enum, sn::case_insensitive)
} // namespace sn::detail

TEST(enum, compatibility) {
    sn::detail::run_compat_ci_enum_test_suite(sn::detail::qstring_ops());
}

namespace sn::detail {
SN_DEFINE_ENUM_QSTRING_FUNCTIONS(int, sn::case_insensitive, gl1_test_tag)
SN_DEFINE_ENUM_QSTRING_FUNCTIONS(int, sn::case_insensitive, gl2_test_tag)
SN_DEFINE_ENUM_QSTRING_FUNCTIONS(int, sn::case_insensitive, first_test_tag, second_test_tag)
} // namespace sn::detail

TEST(enum, tagged) {
    sn::detail::run_tagged_enum_test_suite(sn::detail::qstring_ops());
}

namespace sn::detail {
SN_DEFINE_ENUM_QSTRING_FUNCTIONS(char_test_enum, sn::case_insensitive)
} // namespace sn::detail

TEST(enum, char) {
    sn::detail::run_char_enum_test_suite(sn::detail::qstring_ops());
}

namespace sn::detail {
SN_DEFINE_ENUM_QSTRING_FUNCTIONS(schar_test_enum, sn::case_sensitive)
} // namespace sn::detail

TEST(enum, signed_char) {
    sn::detail::run_schar_enum_test_suite(sn::detail::qstring_ops());
}

namespace sn::detail {
SN_DEFINE_ENUM_QSTRING_FUNCTIONS(utf8_test_enum, sn::case_insensitive)
} // namespace sn::detail

TEST(enum, utf8) {
    sn::detail::run_utf8_enum_test_suite(sn::detail::qstring_ops());
}

namespace sn::detail::adl_test_ns {
SN_DEFINE_ENUM_QSTRING_FUNCTIONS(adl_test_enum, sn::case_sensitive)
} // namespace sn::detail::adl_test_ns
namespace sn::detail {
SN_DEFINE_ENUM_QSTRING_FUNCTIONS(adl_test_enum, sn::case_sensitive) // This should compile & hook into ADL-found reflection
} // namespace sn::detail

TEST(enum, namespaces) {
    sn::detail::run_adl_enum_test_suite(sn::detail::qstring_ops());

    // sn::detail functions work and hook into the right reflection, despite being in the wrong namespace.
    sn::detail::adl_test_enum v = sn::detail::ADL_VALUE_0;
    QString s;
    EXPECT_TRUE(sn::detail::try_from_qstring(u"_1", &v));
    EXPECT_EQ(v, sn::detail::ADL_VALUE_1);
    EXPECT_TRUE(sn::detail::try_to_qstring(sn::detail::ADL_VALUE_1, &s));
    EXPECT_EQ(s, u"_1");

    v = sn::detail::ADL_VALUE_0;
    s.clear();
    EXPECT_NO_THROW(sn::detail::from_qstring(u"_1", &v));
    EXPECT_EQ(v, sn::detail::ADL_VALUE_1);
    EXPECT_NO_THROW(sn::detail::to_qstring(sn::detail::ADL_VALUE_1, &s));
    EXPECT_EQ(s, u"_1");
}
