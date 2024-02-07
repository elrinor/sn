#include <string>

#include <gtest/gtest.h> // NOLINT: not a C system header.

#include "sn/detail/test/enum_test_suite.h"

#include "string_ut.h"
#include "enum_string.h"

namespace sn::detail {
SN_DEFINE_ENUM_STRING_FUNCTIONS(basic_test_enum, sn::case_sensitive)
} // namespace sn::detail

TEST(enum, basic) {
    sn::detail::make_basic_enum_test_suite().run<std::string>();
}

namespace sn::detail {
SN_DEFINE_ENUM_STRING_FUNCTIONS(ci_test_enum, sn::case_insensitive)
} // namespace sn::detail

TEST(enum, case_insensitive) {
    sn::detail::make_ci_enum_test_suite().run<std::string>();
}

namespace sn::detail {
SN_DEFINE_ENUM_STRING_FUNCTIONS(compat_ci_test_enum, sn::case_insensitive)
} // namespace sn::detail

TEST(enum, compatibility) {
    sn::detail::make_compat_ci_enum_test_suite().run<std::string>();
}

namespace sn::detail {
SN_DEFINE_ENUM_STRING_FUNCTIONS(int, sn::case_insensitive, gl1_test_tag)
SN_DEFINE_ENUM_STRING_FUNCTIONS(int, sn::case_insensitive, gl2_test_tag)
} // namespace sn::detail

TEST(enum, tagged) {
    sn::detail::make_tagged_enum_test_suite_1().run<std::string>();
    sn::detail::make_tagged_enum_test_suite_2().run<std::string>();
}

namespace sn::detail {
SN_DEFINE_ENUM_STRING_FUNCTIONS(char_test_enum, sn::case_insensitive)
} // namespace sn::detail

TEST(enum, char) {
    sn::detail::make_char_enum_test_suite().run<std::string>();
}

namespace sn::detail {
SN_DEFINE_ENUM_STRING_FUNCTIONS(schar_test_enum, sn::case_sensitive)
} // namespace sn::detail

TEST(enum, signed_char) {
    sn::detail::make_schar_enum_test_suite().run<std::string>();
}

namespace sn::detail {
SN_DEFINE_ENUM_STRING_FUNCTIONS(utf8_test_enum, sn::case_insensitive)
} // namespace sn::detail

TEST(enum, utf8) {
    sn::detail::make_utf8_enum_test_suite().run<std::string>();
}

namespace sn::detail::test_ns {
SN_DEFINE_ENUM_STRING_FUNCTIONS(adl_test_enum, sn::case_sensitive)
} // namespace sn::detail::test_ns
namespace sn::detail {
SN_DEFINE_ENUM_STRING_FUNCTIONS(adl_test_enum, sn::case_sensitive) // This should compile & hook into ADL-found reflection
} // namespace sn::detail

TEST(enum, namespaces) {
    sn::detail::make_adl_enum_test_suite().run<std::string>();

    // sn::detail functions work and hook into the right reflection, despite being in the wrong namespace.
    sn::detail::adl_test_enum v = sn::detail::ADL_VALUE_0;
    std::string s;
    EXPECT_TRUE(sn::detail::try_from_string("_1", &v));
    EXPECT_EQ(v, sn::detail::ADL_VALUE_1);
    EXPECT_TRUE(sn::detail::try_to_string(sn::detail::ADL_VALUE_1, &s));
    EXPECT_EQ(s, "_1");

    v = sn::detail::ADL_VALUE_0;
    s.clear();
    EXPECT_NO_THROW(sn::detail::from_string("_1", &v));
    EXPECT_EQ(v, sn::detail::ADL_VALUE_1);
    EXPECT_NO_THROW(sn::detail::to_string(sn::detail::ADL_VALUE_1, &s));
    EXPECT_EQ(s, "_1");
}
