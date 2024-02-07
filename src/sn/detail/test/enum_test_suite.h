#pragma once

#include <ostream>
#include <type_traits>

#include "sn/core/enum_reflection.h"
#include "sn/core/type_name.h"

#include "test_suite.h"

namespace sn::detail {

//
// GTest integration for test case comments.
//

template<class T> requires std::is_enum_v<T>
std::ostream &operator<<(std::ostream &s, T value) {
    // TODO(elric): #cpp23 use std::to_underlying.
    return s << sn::type_name<T>() << "(" << static_cast<std::underlying_type_t<T>>(value) << ")";
}

namespace test_ns {
template<class T> requires std::is_enum_v<T>
std::ostream &operator<<(std::ostream &s, T value) {
    return sn::detail::operator<<(s, value);
}
} // namespace test_ns


//
// Basic test suite.
//

enum class basic_test_enum {
    BASIC_VALUE_1 = 1,
    BASIC_VALUE_2 = 2,
    BASIC_VALUE_3 = 3,
    BASIC_UNSERIALIZABLE = 0x11111111
};
using enum basic_test_enum;

SN_DEFINE_ENUM_REFLECTION(basic_test_enum, ({
    { BASIC_VALUE_1, "aaa" },
    { BASIC_VALUE_2, "bbb" },
    { BASIC_VALUE_3, "CCC" },
}))

inline test_suite<basic_test_enum> make_basic_enum_test_suite() {
    test_suite<basic_test_enum> result;

    result.throwing_to = {BASIC_UNSERIALIZABLE};

    result.throwing_from = {
        "AAA",
        "ccc",
        "1",
        "2",
        "3",
        " aaa",
        "aaa ",
        " aaa ",
        "\taaa",
        "aaa\t",
        "\taaa\t",
    };

    result.valid_fromto = {
        {"aaa", BASIC_VALUE_1},
        {"bbb", BASIC_VALUE_2},
        {"CCC", BASIC_VALUE_3},
    };

    return result;
}


//
// Case-insensitive test suite.
//

enum class ci_test_enum {
    CI_VALUE_1 = 1,
    CI_VALUE_2 = 2,
    CI_VALUE_3 = 3,
    CI_VALUE_4 = 4,
};
using enum ci_test_enum;
SN_DEFINE_ENUM_REFLECTION(ci_test_enum, ({
    {CI_VALUE_1, "AAA"},
    {CI_VALUE_2, "bbb"},
    {CI_VALUE_3, "Ccc"},
    {CI_VALUE_4, "111_ab"},
}))

inline test_suite<ci_test_enum> make_ci_enum_test_suite() {
    test_suite<ci_test_enum> result;

    result.valid_fromto = {
        {"AAA", CI_VALUE_1},
        {"bbb", CI_VALUE_2},
        {"Ccc", CI_VALUE_3},
        {"111_ab", CI_VALUE_4},
    };

    result.valid_from = {
        {"AaA", CI_VALUE_1},
        {"aaa", CI_VALUE_1},
        {"BBB", CI_VALUE_2},
        {"ccc", CI_VALUE_3},
        {"111_AB", CI_VALUE_4},
    };

    return result;
}


//
// Compatibility test suite
//

enum class compat_ci_test_enum {
    COMPAT_CI_VALUE_1,
    COMPAT_CI_VALUE_2,
};
using enum compat_ci_test_enum;

SN_DEFINE_ENUM_REFLECTION(compat_ci_test_enum, ({
    {COMPAT_CI_VALUE_1, "COMPAT_1"},
    {COMPAT_CI_VALUE_2, "COMPAT_2"},
    {COMPAT_CI_VALUE_1, "OLD_1"},
    {COMPAT_CI_VALUE_2, "OLD_2"},
}))

inline test_suite<compat_ci_test_enum> make_compat_ci_enum_test_suite() {
    test_suite<compat_ci_test_enum> result;

    result.valid_fromto = {
        {"COMPAT_1", COMPAT_CI_VALUE_1},
        {"COMPAT_2", COMPAT_CI_VALUE_2}
    };

    result.valid_from = {
        {"compat_1", COMPAT_CI_VALUE_1},
        {"OLD_1", COMPAT_CI_VALUE_1},
        {"old_1", COMPAT_CI_VALUE_1},
        {"compat_2", COMPAT_CI_VALUE_2},
        {"OLD_2", COMPAT_CI_VALUE_2},
        {"old_2", COMPAT_CI_VALUE_2},
    };

    return result;
}


//
// Tagged enum test suite.
//

struct gl1_test_tag {};
struct gl2_test_tag {};

SN_DEFINE_ENUM_REFLECTION(int, ({{1, "GL_1"}, {2, "GL_2"}}), gl1_test_tag)
SN_DEFINE_ENUM_REFLECTION(int, ({{100, "GL_100"}, {200, "GL_200"}}), gl2_test_tag)

inline test_suite<int, gl1_test_tag> make_tagged_enum_test_suite_1() {
    test_suite<int, gl1_test_tag> result;

    result.throwing_to = {100, 3};
    result.throwing_from = {"GL_100"};

    result.valid_fromto = {
        {"GL_1", 1},
        {"GL_2", 2}
    };

    return result;
}

inline test_suite<int, gl2_test_tag> make_tagged_enum_test_suite_2() {
    test_suite<int, gl2_test_tag> result;

    result.throwing_to = {1, 300};
    result.throwing_from = {"GL_1"};

    result.valid_fromto = {
        {"GL_100", 100},
        {"GL_200", 200}
    };

    return result;
}


//
// Test suite for enum T : char. Test that it works despite there being no sn::to_string overload for char.
//

enum char_test_enum : char {
    CHAR_VALUE_1 = '1',
    CHAR_VALUE_2 = '2',
    CHAR_VALUE_UNK = '@'
};

SN_DEFINE_ENUM_REFLECTION(char_test_enum, ({{CHAR_VALUE_1, "CHAR_1"}, {CHAR_VALUE_2, "CHAR_2"}}))

inline test_suite<char_test_enum> make_char_enum_test_suite() {
    test_suite<char_test_enum> result;

    result.throwing_to_message = {
        {CHAR_VALUE_UNK, "'64'"}
    };

    result.valid_fromto = {
        {"CHAR_1", CHAR_VALUE_1},
        {"CHAR_2", CHAR_VALUE_2}
    };

    return result;
}


//
// Same as above but for signed char.
//

enum schar_test_enum : signed char {
    SCHAR_VALUE_1 = -1,
    SCHAR_VALUE_UNK = -100
};

SN_DEFINE_ENUM_REFLECTION(schar_test_enum, ({{SCHAR_VALUE_1, "SCHAR_1"}}))

inline test_suite<schar_test_enum> make_schar_enum_test_suite() {
    test_suite<schar_test_enum> result;

    result.throwing_to_message = {
        {SCHAR_VALUE_UNK, "'-100'"}
    };

    result.valid_fromto = {
        {"SCHAR_1", SCHAR_VALUE_1},
    };

    return result;
}


//
// Test suite for utf8 strings. Checks that our to_lower implementation only works for ascii chars.
//

enum utf8_test_enum {
    UTF8_VALUE_1 = 1,
    UTF8_VALUE_2 = 2,
};

SN_DEFINE_ENUM_REFLECTION(utf8_test_enum, ({
    {UTF8_VALUE_1, "\xd0\x94\xd0\xbe\xd0\xbc"}, // "Dom" (House) in Russian.
    {UTF8_VALUE_2, "LOL"}
}))

inline test_suite<utf8_test_enum> make_utf8_enum_test_suite() {
    test_suite<utf8_test_enum> result;

    result.throwing_from = {
        "\xd0\xb4\xd0\xbe\xd0\xbc" // "dom" (house) in Russian.
    };

    result.valid_from = {
        {"lol", UTF8_VALUE_2} // Should be case-insensitive for ascii chars.
    };

    result.valid_fromto = {
        {"\xd0\x94\xd0\xbe\xd0\xbc", UTF8_VALUE_1},
        {"LOL", UTF8_VALUE_2}
    };

    return result;
}


//
// Test suite for proper ADL routing.
//
// We'll also need to write some additional code in the tests themselves b/c test suites don't let us call into
// customization points directly. See the corresponding tests.
//

namespace test_ns {
enum class adl_test_enum {
    ADL_VALUE_0 = 0,
    ADL_VALUE_1 = 1,
};
using enum adl_test_enum;
SN_DEFINE_ENUM_REFLECTION(adl_test_enum, ({{ADL_VALUE_1, "_1"}}))
} // namespace test_ns

// That's the problematic SN_DEFINE_ENUM_REFLECTION. We are testing that this one won't be used in serialization code.
// Note that while we can issue diagnostics here, this will be messy. Just checking that the ADL-found function doesn't
// exist will violate ODR, so we'll also need counters. TLDR: it's a mess and not worth it.
using test_ns::adl_test_enum;
using enum test_ns::adl_test_enum;
SN_DEFINE_ENUM_REFLECTION(adl_test_enum, ({{ADL_VALUE_1, "WUT"}}))

inline test_suite<adl_test_enum> make_adl_enum_test_suite() {
    test_suite<adl_test_enum> result;

    result.valid_fromto = {
        {"_1", ADL_VALUE_1},
    };

    return result;
}


} // namespace sn::detail
