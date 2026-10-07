#include <cstddef>
#include <cstdint>
#include <limits>
#include <ostream>
#include <string>
#include <type_traits>
#include <utility> // For std::to_underlying.

#include <gtest/gtest.h> // NOLINT: not a C system header.

#include "sn/core/type_name.h"
#include "sn/core/tag.h"
#include "sn/reflection/enum_reflection.h"
#include "sn/string/enum_string.h"

#include "tester.h"

namespace sn::detail {

//
// GTest integration for test case comments.
//

template<class T> requires std::is_enum_v<T>
std::ostream &operator<<(std::ostream &s, T value) {
    return s << sn::type_name<T>() << "(" << std::to_underlying(value) << ")";
}

namespace adl_test_ns {
template<class T> requires std::is_enum_v<T>
std::ostream &operator<<(std::ostream &s, T value) {
    return sn::detail::operator<<(s, value);
}
} // namespace adl_test_ns


//
// Basic tests.
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
SN_DEFINE_ENUM_STRING_FUNCTIONS(basic_test_enum, sn::case_sensitive)

TEST(string_enum, basic) {
    sn::detail::tester<basic_test_enum> tester;

    tester.expect_throwing_to({BASIC_UNSERIALIZABLE});

    tester.expect_throwing_from({
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
    });

    tester.expect_valid_fromto({
        {"aaa", BASIC_VALUE_1},
        {"bbb", BASIC_VALUE_2},
        {"CCC", BASIC_VALUE_3},
    });
}


//
// Case-insensitive tests.
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
SN_DEFINE_ENUM_STRING_FUNCTIONS(ci_test_enum, sn::case_insensitive)

TEST(string_enum, case_insensitive) {
    tester<ci_test_enum> t;

    t.expect_valid_fromto({
        {"AAA", CI_VALUE_1},
        {"bbb", CI_VALUE_2},
        {"Ccc", CI_VALUE_3},
        {"111_ab", CI_VALUE_4},
    });

    t.expect_valid_from({
        {"AaA", CI_VALUE_1},
        {"aaa", CI_VALUE_1},
        {"BBB", CI_VALUE_2},
        {"ccc", CI_VALUE_3},
        {"111_AB", CI_VALUE_4},
    });
}


//
// Compatibility tests.
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
SN_DEFINE_ENUM_STRING_FUNCTIONS(compat_ci_test_enum, sn::case_insensitive)

TEST(string_enum, compatibility) {
    tester<compat_ci_test_enum> t;

    t.expect_valid_fromto({
        {"COMPAT_1", COMPAT_CI_VALUE_1},
        {"COMPAT_2", COMPAT_CI_VALUE_2}
    });

    t.expect_valid_from({
        {"compat_1", COMPAT_CI_VALUE_1},
        {"OLD_1", COMPAT_CI_VALUE_1},
        {"old_1", COMPAT_CI_VALUE_1},
        {"compat_2", COMPAT_CI_VALUE_2},
        {"OLD_2", COMPAT_CI_VALUE_2},
        {"old_2", COMPAT_CI_VALUE_2},
    });
}


//
// Tagged enum tests.
//

struct gl1_test_tag : sn::tags::tag {};
struct gl2_test_tag : sn::tags::tag {};
struct first_test_tag : sn::tags::tag {};
struct second_test_tag : sn::tags::tag {};

SN_DEFINE_ENUM_REFLECTION(int, ({{1, "GL_1"}, {2, "GL_2"}}), gl1_test_tag)
SN_DEFINE_ENUM_REFLECTION(int, ({{100, "GL_100"}, {200, "GL_200"}}), gl2_test_tag)
SN_DEFINE_ENUM_REFLECTION(int, ({{0, "GL_0"}}), first_test_tag, second_test_tag)

SN_DEFINE_ENUM_STRING_FUNCTIONS(int, sn::case_insensitive, gl1_test_tag)
SN_DEFINE_ENUM_STRING_FUNCTIONS(int, sn::case_insensitive, gl2_test_tag)
SN_DEFINE_ENUM_STRING_FUNCTIONS(int, sn::case_insensitive, first_test_tag, second_test_tag)

TEST(string_enum, tagged) {
    tester<int> t;

    t.expect_throwing_to({100, 3}, gl1_test_tag());
    t.expect_throwing_from({"GL_100"}, gl1_test_tag());
    t.expect_valid_fromto({
        {"GL_1", 1},
        {"GL_2", 2}
    }, gl1_test_tag());

    t.expect_throwing_to({1, 300}, gl2_test_tag());
    t.expect_throwing_from({"GL_1"}, gl2_test_tag());
    t.expect_valid_fromto({
        {"GL_100", 100},
        {"GL_200", 200}
    }, gl2_test_tag());

    t.expect_throwing_to({1}, first_test_tag(), second_test_tag());
    t.expect_throwing_from({"GL_1"}, first_test_tag(), second_test_tag());
    t.expect_valid_fromto({{"GL_0", 0}}, first_test_tag(), second_test_tag());
}


//
// Tests for enum T : char. Check that it works despite there being no sn::to_string overload for char.
//

enum char_test_enum : char {
    CHAR_VALUE_1 = '1',
    CHAR_VALUE_2 = '2',
    CHAR_VALUE_UNK = '@'
};

SN_DEFINE_ENUM_REFLECTION(char_test_enum, ({{CHAR_VALUE_1, "CHAR_1"}, {CHAR_VALUE_2, "CHAR_2"}}))
SN_DEFINE_ENUM_STRING_FUNCTIONS(char_test_enum, sn::case_insensitive)

TEST(string_enum, char) {
    tester<char_test_enum> t;

    t.expect_throwing_to_with_message({{CHAR_VALUE_UNK, "'64'"}});

    t.expect_valid_fromto({
        {"CHAR_1", CHAR_VALUE_1},
        {"CHAR_2", CHAR_VALUE_2}
    });
}


//
// Same as above but for signed char.
//

enum schar_test_enum : signed char {
    SCHAR_VALUE_1 = -1,
    SCHAR_VALUE_UNK = -100
};

SN_DEFINE_ENUM_REFLECTION(schar_test_enum, ({{SCHAR_VALUE_1, "SCHAR_1"}}))
SN_DEFINE_ENUM_STRING_FUNCTIONS(schar_test_enum, sn::case_sensitive)

TEST(string_enum, signed_char) {
    tester<schar_test_enum> t;

    t.expect_throwing_to_with_message({
        {SCHAR_VALUE_UNK, "'-100'"}
    });

    t.expect_valid_fromto({
        {"SCHAR_1", SCHAR_VALUE_1},
    });
}


//
// Tests for reflections that don't list the smallest value first. All values should still be serializable, and the
// first string listed for a value should be the one that's used in to_string.
//

enum class unordered_test_enum {
    UNORDERED_VALUE_1 = 1,
    UNORDERED_VALUE_2 = 2,
    UNORDERED_VALUE_3 = 3,
};
using enum unordered_test_enum;

SN_DEFINE_ENUM_REFLECTION(unordered_test_enum, ({
    {UNORDERED_VALUE_3, "three"},
    {UNORDERED_VALUE_1, "one"},
    {UNORDERED_VALUE_2, "two"},
    {UNORDERED_VALUE_1, "old_one"},
}))
SN_DEFINE_ENUM_STRING_FUNCTIONS(unordered_test_enum, sn::case_sensitive)

TEST(string_enum, unordered) {
    tester<unordered_test_enum> t;

    t.expect_valid_fromto({
        {"one", UNORDERED_VALUE_1},
        {"two", UNORDERED_VALUE_2},
        {"three", UNORDERED_VALUE_3},
    });

    t.expect_valid_from({
        {"old_one", UNORDERED_VALUE_1},
    });
}


//
// Same as above but for negative values. Enum values are type-erased into std::uint64_t, so a negative value that's
// listed first is not the smallest one after type erasure.
//

enum class negative_test_enum {
    NEGATIVE_VALUE_MINUS_1 = -1,
    NEGATIVE_VALUE_0 = 0,
    NEGATIVE_VALUE_1 = 1,
    NEGATIVE_UNSERIALIZABLE = -2,
};
using enum negative_test_enum;

SN_DEFINE_ENUM_REFLECTION(negative_test_enum, ({
    {NEGATIVE_VALUE_MINUS_1, "minus_one"},
    {NEGATIVE_VALUE_0, "zero"},
    {NEGATIVE_VALUE_1, "one"},
}))
SN_DEFINE_ENUM_STRING_FUNCTIONS(negative_test_enum, sn::case_sensitive)

// The table should only cover values from -1 to 1. If the range of values was computed after type erasure, it would
// have been 2^64 values wide, and the table would have been a hashed one.
static_assert(_enum_table_container<negative_test_enum>::table_spec.flat);
static_assert(_enum_table_container<negative_test_enum>::table_spec.to_string_slots == 3);
static_assert(_enum_table_container<negative_test_enum>::table_spec.max_delta == 2);

TEST(string_enum, negative) {
    tester<negative_test_enum> t;

    t.expect_throwing_to_with_message({
        {NEGATIVE_UNSERIALIZABLE, "'-2'"}
    });

    t.expect_valid_fromto({
        {"minus_one", NEGATIVE_VALUE_MINUS_1},
        {"zero", NEGATIVE_VALUE_0},
        {"one", NEGATIVE_VALUE_1},
    });
}


//
// Tests for enums with gaps between values. Such enums still use a flat table, and the gaps should not be
// serializable.
//

enum class gap_test_enum {
    GAP_VALUE_0 = 0,
    GAP_VALUE_1 = 1,
    GAP_VALUE_3 = 3,
    GAP_VALUE_7 = 7,
};
using enum gap_test_enum;

SN_DEFINE_ENUM_REFLECTION(gap_test_enum, ({
    {GAP_VALUE_0, "zero"},
    {GAP_VALUE_1, "one"},
    {GAP_VALUE_3, "three"},
    {GAP_VALUE_7, "seven"},
}))
SN_DEFINE_ENUM_STRING_FUNCTIONS(gap_test_enum, sn::case_sensitive)

static_assert(_enum_table_container<gap_test_enum>::table_spec.flat);

TEST(string_enum, gaps) {
    tester<gap_test_enum> t;

    t.expect_throwing_to({
        static_cast<gap_test_enum>(-1),
        static_cast<gap_test_enum>(2),
        static_cast<gap_test_enum>(4),
        static_cast<gap_test_enum>(5),
        static_cast<gap_test_enum>(6),
        static_cast<gap_test_enum>(8),
    });

    t.expect_valid_fromto({
        {"zero", GAP_VALUE_0},
        {"one", GAP_VALUE_1},
        {"three", GAP_VALUE_3},
        {"seven", GAP_VALUE_7},
    });
}


//
// Tests for enums with values that are far apart. These use a hash table.
//

enum class sparse_test_enum : std::uint32_t {
    SPARSE_VALUE_1 = 1,
    SPARSE_VALUE_1000 = 1000,
    SPARSE_VALUE_100000 = 100000,
    SPARSE_VALUE_MAX = 0xFFFFFFFFu,
};
using enum sparse_test_enum;

SN_DEFINE_ENUM_REFLECTION(sparse_test_enum, ({
    {SPARSE_VALUE_1, "one"},
    {SPARSE_VALUE_1000, "thousand"},
    {SPARSE_VALUE_100000, "hundred_thousand"},
    {SPARSE_VALUE_MAX, "max"},
}))
SN_DEFINE_ENUM_STRING_FUNCTIONS(sparse_test_enum, sn::case_sensitive)

static_assert(!_enum_table_container<sparse_test_enum>::table_spec.flat);

TEST(string_enum, sparse) {
    tester<sparse_test_enum> t;

    t.expect_throwing_to({
        static_cast<sparse_test_enum>(0),
        static_cast<sparse_test_enum>(2),
        static_cast<sparse_test_enum>(999),
        static_cast<sparse_test_enum>(1001),
        static_cast<sparse_test_enum>(0xFFFFFFFEu),
    });

    t.expect_valid_fromto({
        {"one", SPARSE_VALUE_1},
        {"thousand", SPARSE_VALUE_1000},
        {"hundred_thousand", SPARSE_VALUE_100000},
        {"max", SPARSE_VALUE_MAX},
    });
}


//
// Tests for 64-bit enums that use the whole range of the underlying type.
//

enum class int64_test_enum : std::int64_t {
    INT64_VALUE_MIN = std::numeric_limits<std::int64_t>::min(),
    INT64_VALUE_0 = 0,
    INT64_VALUE_MAX = std::numeric_limits<std::int64_t>::max(),
};
using enum int64_test_enum;

SN_DEFINE_ENUM_REFLECTION(int64_test_enum, ({
    {INT64_VALUE_MIN, "min"},
    {INT64_VALUE_0, "zero"},
    {INT64_VALUE_MAX, "max"},
}))
SN_DEFINE_ENUM_STRING_FUNCTIONS(int64_test_enum, sn::case_sensitive)

enum class uint64_test_enum : std::uint64_t {
    UINT64_VALUE_0 = 0,
    UINT64_VALUE_BIG = 0x10000000000u,
    UINT64_VALUE_MAX = std::numeric_limits<std::uint64_t>::max(),
};
using enum uint64_test_enum;

SN_DEFINE_ENUM_REFLECTION(uint64_test_enum, ({
    {UINT64_VALUE_0, "zero"},
    {UINT64_VALUE_BIG, "big"},
    {UINT64_VALUE_MAX, "max"},
}))
SN_DEFINE_ENUM_STRING_FUNCTIONS(uint64_test_enum, sn::case_sensitive)

TEST(string_enum, int64) {
    tester<int64_test_enum> t1;
    t1.expect_throwing_to({static_cast<int64_test_enum>(1), static_cast<int64_test_enum>(-1)});
    t1.expect_valid_fromto({
        {"min", INT64_VALUE_MIN},
        {"zero", INT64_VALUE_0},
        {"max", INT64_VALUE_MAX},
    });

    tester<uint64_test_enum> t2;
    t2.expect_throwing_to({static_cast<uint64_test_enum>(1), static_cast<uint64_test_enum>(0x10000000001u)});
    t2.expect_valid_fromto({
        {"zero", UINT64_VALUE_0},
        {"big", UINT64_VALUE_BIG},
        {"max", UINT64_VALUE_MAX},
    });
}


//
// Tests for strings of different sizes. Strings are hashed and compared in machine words, so we want to check sizes
// around the word boundaries, and strings that only differ in a single char.
//

enum class size_test_enum {
    SIZE_VALUE_1,
    SIZE_VALUE_3,
    SIZE_VALUE_4,
    SIZE_VALUE_7,
    SIZE_VALUE_8,
    SIZE_VALUE_9,
    SIZE_VALUE_15,
    SIZE_VALUE_16,
    SIZE_VALUE_17,
    SIZE_VALUE_32,
    SIZE_VALUE_33,
    SIZE_VALUE_70,
    SIZE_VALUE_70_OTHER,
};
using enum size_test_enum;

struct size_ci_test_tag : sn::tags::tag {};

#define SIZE_TEST_ENUM_REFLECTION ({                                                                                    \
    {SIZE_VALUE_1, "a"},                                                                                                \
    {SIZE_VALUE_3, "abc"},                                                                                              \
    {SIZE_VALUE_4, "abcd"},                                                                                             \
    {SIZE_VALUE_7, "abcdefg"},                                                                                          \
    {SIZE_VALUE_8, "abcdefgh"},                                                                                         \
    {SIZE_VALUE_9, "abcdefghi"},                                                                                        \
    {SIZE_VALUE_15, "abcdefghijklmno"},                                                                                 \
    {SIZE_VALUE_16, "abcdefghijklmnop"},                                                                                \
    {SIZE_VALUE_17, "abcdefghijklmnopq"},                                                                               \
    {SIZE_VALUE_32, "abcdefghijklmnopqrstuvwxyz012345"},                                                                \
    {SIZE_VALUE_33, "abcdefghijklmnopqrstuvwxyz0123456"},                                                               \
    {SIZE_VALUE_70, "abcdefghijklmnopqrstuvwxyz0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ_abcdefg"},                          \
    {SIZE_VALUE_70_OTHER, "abcdefghijklmnopqrstuvwxyz0123456789ABCDEFGHIJ_LMNOPQRSTUVWXYZ_abcdefg"},                    \
})

SN_DEFINE_ENUM_REFLECTION(size_test_enum, SIZE_TEST_ENUM_REFLECTION)
SN_DEFINE_ENUM_REFLECTION(size_test_enum, SIZE_TEST_ENUM_REFLECTION, size_ci_test_tag)
SN_DEFINE_ENUM_STRING_FUNCTIONS(size_test_enum, sn::case_sensitive)
SN_DEFINE_ENUM_STRING_FUNCTIONS(size_test_enum, sn::case_insensitive, size_ci_test_tag)

TEST(string_enum, string_sizes) {
    tester<size_test_enum> t;

    for (const auto &[value, string] : sn::reflect_enum<size_test_enum>()) {
        t.expect_valid_fromto(string, value);
        t.expect_valid_fromto(string, value, size_ci_test_tag());

        // Changing a single char should make the string unrecognizable, wherever the char is.
        for (std::size_t i = 0; i < string.size(); i++) {
            std::string changed(string);
            changed[i] = '#';
            t.expect_throwing_from(changed);
            t.expect_throwing_from(changed, size_ci_test_tag());
        }

        // Changing case should only work in case-insensitive mode. Only the last string has uppercase chars.
        for (std::size_t i = 0; i < string.size(); i++) {
            std::string changed(string);
            if (changed[i] >= 'a' && changed[i] <= 'z') {
                changed[i] = static_cast<char>(changed[i] - 'a' + 'A');
            } else if (changed[i] >= 'A' && changed[i] <= 'Z') {
                changed[i] = static_cast<char>(changed[i] - 'A' + 'a');
            } else {
                continue;
            }

            if (value != SIZE_VALUE_70 && value != SIZE_VALUE_70_OTHER) {
                t.expect_throwing_from(changed);
                t.expect_valid_from(changed, value, size_ci_test_tag());
            }
        }

        // And so should adding a char.
        t.expect_throwing_from(std::string(string) + "_");
        t.expect_throwing_from(std::string(string) + "_", size_ci_test_tag());
    }

    t.expect_throwing_from({"", "ab", "abcde", "abcdefghijklmnopqrstuvwxyz"});
}


//
// Tests for empty strings. An empty string is a valid string for an enum value.
//

enum class empty_test_enum {
    EMPTY_VALUE_NONE = 0,
    EMPTY_VALUE_1 = 1,
    EMPTY_VALUE_2 = 2,
};
using enum empty_test_enum;

struct empty_alias_test_tag : sn::tags::tag {};

SN_DEFINE_ENUM_REFLECTION(empty_test_enum, ({
    {EMPTY_VALUE_NONE, ""},
    {EMPTY_VALUE_1, "one"},
    {EMPTY_VALUE_2, "two"},
}))
SN_DEFINE_ENUM_REFLECTION(empty_test_enum, ({
    {EMPTY_VALUE_1, "one"},
    {EMPTY_VALUE_1, ""},
    {EMPTY_VALUE_2, "two"},
}), empty_alias_test_tag)
SN_DEFINE_ENUM_STRING_FUNCTIONS(empty_test_enum, sn::case_sensitive)
SN_DEFINE_ENUM_STRING_FUNCTIONS(empty_test_enum, sn::case_insensitive, empty_alias_test_tag)

TEST(string_enum, empty_string) {
    tester<empty_test_enum> t;

    t.expect_valid_fromto({
        {"", EMPTY_VALUE_NONE},
        {"one", EMPTY_VALUE_1},
        {"two", EMPTY_VALUE_2},
    });

    // If an empty string is not the first string for a value, then it's only accepted as input.
    t.expect_throwing_to({EMPTY_VALUE_NONE}, empty_alias_test_tag());
    t.expect_valid_from({{"", EMPTY_VALUE_1}}, empty_alias_test_tag());
    t.expect_valid_fromto({
        {"one", EMPTY_VALUE_1},
        {"two", EMPTY_VALUE_2},
    }, empty_alias_test_tag());

    // And if there is no empty string in the reflection, then it's not accepted.
    tester<gap_test_enum> t2;
    t2.expect_throwing_from({""});
}


//
// Tests for explicitly specified table kinds. The results should be the same for all table kinds.
//

enum class kind_test_enum {
    KIND_VALUE_0 = 0,
    KIND_VALUE_1 = 1,
    KIND_VALUE_2 = 2,
    KIND_VALUE_1000 = 1000,
};
using enum kind_test_enum;

struct auto_test_tag : sn::tags::tag {};
struct flat_test_tag : sn::tags::tag {};
struct hashed_test_tag : sn::tags::tag {};

#define KIND_TEST_ENUM_REFLECTION ({                                                                                    \
    {KIND_VALUE_0, "zero"},                                                                                             \
    {KIND_VALUE_1, "one"},                                                                                              \
    {KIND_VALUE_2, "two"},                                                                                              \
    {KIND_VALUE_1000, "thousand"},                                                                                      \
})

SN_DEFINE_ENUM_REFLECTION(kind_test_enum, KIND_TEST_ENUM_REFLECTION)
SN_DEFINE_ENUM_REFLECTION(kind_test_enum, KIND_TEST_ENUM_REFLECTION, auto_test_tag)
SN_DEFINE_ENUM_REFLECTION(kind_test_enum, KIND_TEST_ENUM_REFLECTION, flat_test_tag)
SN_DEFINE_ENUM_REFLECTION(kind_test_enum, KIND_TEST_ENUM_REFLECTION, hashed_test_tag)
SN_DEFINE_ENUM_STRING_FUNCTIONS(kind_test_enum, sn::case_sensitive)
SN_DEFINE_ENUM_STRING_FUNCTIONS(kind_test_enum, sn::case_sensitive | sn::auto_enum_table, auto_test_tag)
SN_DEFINE_ENUM_STRING_FUNCTIONS(kind_test_enum, sn::case_sensitive | sn::flat_enum_table, flat_test_tag)
SN_DEFINE_ENUM_STRING_FUNCTIONS(kind_test_enum, sn::hashed_enum_table | sn::case_insensitive, hashed_test_tag)

static_assert(!_enum_table_container<kind_test_enum>::table_spec.flat);
static_assert(!_enum_table_container<kind_test_enum, auto_test_tag>::table_spec.flat); // Same as the default.
static_assert(_enum_table_container<kind_test_enum, flat_test_tag>::table_spec.flat);
static_assert(!_enum_table_container<kind_test_enum, hashed_test_tag>::table_spec.flat);
static_assert(!_enum_table_container<kind_test_enum, flat_test_tag>::table_spec.fold_case);
static_assert(_enum_table_container<kind_test_enum, hashed_test_tag>::table_spec.fold_case);

// A table that would be flat by default can also be forced into being a hash table.
struct hashed_gap_test_tag : sn::tags::tag {};
SN_DEFINE_ENUM_REFLECTION(gap_test_enum, ({
    {GAP_VALUE_0, "zero"},
    {GAP_VALUE_1, "one"},
    {GAP_VALUE_3, "three"},
    {GAP_VALUE_7, "seven"},
}), hashed_gap_test_tag)
SN_DEFINE_ENUM_STRING_FUNCTIONS(gap_test_enum, sn::case_sensitive | sn::hashed_enum_table, hashed_gap_test_tag)

static_assert(!_enum_table_container<gap_test_enum, hashed_gap_test_tag>::table_spec.flat);

TEST(string_enum, table_kinds) {
    tester<kind_test_enum> t;

    auto run = [&] (auto... tags) {
        t.expect_throwing_to({
            static_cast<kind_test_enum>(-1),
            static_cast<kind_test_enum>(3),
            static_cast<kind_test_enum>(999),
            static_cast<kind_test_enum>(1001),
        }, tags...);

        t.expect_valid_fromto({
            {"zero", KIND_VALUE_0},
            {"one", KIND_VALUE_1},
            {"two", KIND_VALUE_2},
            {"thousand", KIND_VALUE_1000},
        }, tags...);
    };

    run();
    run(auto_test_tag());
    run(flat_test_tag());
    run(hashed_test_tag());

    t.expect_throwing_from({"ZERO"});
    t.expect_throwing_from({"ZERO"}, auto_test_tag());
    t.expect_throwing_from({"ZERO"}, flat_test_tag());
    t.expect_valid_from({{"ZERO", KIND_VALUE_0}}, hashed_test_tag());

    tester<gap_test_enum> t2;
    t2.expect_throwing_to({static_cast<gap_test_enum>(-1), static_cast<gap_test_enum>(2), static_cast<gap_test_enum>(8)}, hashed_gap_test_tag());
    t2.expect_valid_fromto({
        {"zero", GAP_VALUE_0},
        {"one", GAP_VALUE_1},
        {"three", GAP_VALUE_3},
        {"seven", GAP_VALUE_7},
    }, hashed_gap_test_tag());
}


//
// Tests for utf8 strings. Check that our to_lower implementation only works for ascii chars.
//

enum utf8_test_enum {
    UTF8_VALUE_1 = 1,
    UTF8_VALUE_2 = 2,
};

SN_DEFINE_ENUM_REFLECTION(utf8_test_enum, ({
    {UTF8_VALUE_1, "\xd0\x94\xd0\xbe\xd0\xbc"}, // "Dom" (House) in Russian.
    {UTF8_VALUE_2, "LOL"}
}))
SN_DEFINE_ENUM_STRING_FUNCTIONS(utf8_test_enum, sn::case_insensitive)

TEST(string_enum, utf8) {
    tester<utf8_test_enum> t;

    t.expect_throwing_from({
        "\xd0\xb4\xd0\xbe\xd0\xbc" // "dom" (house) in Russian.
    });

    t.expect_valid_from({
        {"lol", UTF8_VALUE_2} // Should be case-insensitive for ascii chars.
    });

    t.expect_valid_fromto({
        {"\xd0\x94\xd0\xbe\xd0\xbc", UTF8_VALUE_1},
        {"LOL", UTF8_VALUE_2}
    });
}


//
// Tests for proper ADL routing.
//

namespace adl_test_ns {
enum class adl_test_enum {
    ADL_VALUE_0 = 0,
    ADL_VALUE_1 = 1,
};
using enum adl_test_enum;
SN_DEFINE_ENUM_REFLECTION(adl_test_enum, ({{ADL_VALUE_1, "_1"}}))
} // namespace adl_test_ns

// That's the problematic SN_DEFINE_ENUM_REFLECTION. We are testing that this one won't be used in serialization code.
// Note that while we can issue diagnostics here, this will be messy. Just checking that the ADL-found function doesn't
// exist will violate ODR, so we'll also need counters. TLDR: it's a mess and not worth it.
using adl_test_ns::adl_test_enum;
using enum adl_test_ns::adl_test_enum;
SN_DEFINE_ENUM_REFLECTION(adl_test_enum, ({{ADL_VALUE_1, "WUT"}}))

namespace adl_test_ns {
SN_DEFINE_ENUM_STRING_FUNCTIONS(adl_test_enum, sn::case_sensitive)
} // namespace adl_test_ns
SN_DEFINE_ENUM_STRING_FUNCTIONS(adl_test_enum, sn::case_sensitive) // This should compile & hook into ADL-found reflection

TEST(string_enum, namespaces) {
    tester<adl_test_enum> t;

    t.expect_valid_fromto({
        {"_1", ADL_VALUE_1},
    });

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

} // namespace sn::detail
