#include <array>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility> // For std::pair.

#include <gtest/gtest.h> // NOLINT: not a C system header.

#include "sn/string/detail/string_enum_table.h"

namespace sn::detail {

//
// Generated reflections. Enum tables for small enums are tested in enum_string_ut.cpp, here we test larger ones, and
// it's easier to generate these than to write them down.
//

template<std::size_t size, std::size_t capacity = 16>
struct generated_strings {
    std::array<std::array<char, capacity>, size> data = {{}};
    std::array<std::size_t, size> sizes = {{}};
};

/**
 * @return                              Strings "value_0", "value_1", etc.
 */
template<std::size_t size>
constexpr generated_strings<size> generate_strings() {
    generated_strings<size> result;
    for (std::size_t i = 0; i < size; i++) {
        std::string_view prefix = "value_";
        std::size_t pos = 0;
        for (char c : prefix)
            result.data[i][pos++] = c;

        std::size_t digits = 1;
        for (std::size_t tmp = i; tmp >= 10; tmp /= 10)
            digits++;
        std::size_t tmp = i;
        for (std::size_t j = 0; j < digits; j++) {
            result.data[i][pos + digits - 1 - j] = static_cast<char>('0' + tmp % 10);
            tmp /= 10;
        }
        result.sizes[i] = pos + digits;
    }
    return result;
}

/**
 * @return                              Strings of sizes 1 to `size`, each one being a prefix of the next one.
 */
template<std::size_t size>
constexpr generated_strings<size, size> generate_prefix_strings() {
    generated_strings<size, size> result;
    for (std::size_t i = 0; i < size; i++) {
        for (std::size_t j = 0; j <= i; j++)
            result.data[i][j] = static_cast<char>('a' + (j * 7) % 26);
        result.sizes[i] = i + 1;
    }
    return result;
}

/**
 * @return                              `size` different strings of `string_size` chars each. Must have `size <= 100`
 *                                      and `string_size >= 2`.
 */
template<std::size_t size, std::size_t string_size>
constexpr generated_strings<size, string_size> generate_fixed_size_strings() {
    generated_strings<size, string_size> result;
    for (std::size_t i = 0; i < size; i++) {
        result.data[i][0] = static_cast<char>('0' + i / 10);
        result.data[i][1] = static_cast<char>('0' + i % 10);
        for (std::size_t j = 2; j < string_size; j++)
            result.data[i][j] = static_cast<char>('a' + (i + j) % 26);
        result.sizes[i] = string_size;
    }
    return result;
}

template<class Strings, std::size_t size>
constexpr std::array<std::pair<std::int64_t, std::string_view>, size> make_reflection(const Strings &strings, std::int64_t first, std::int64_t step) {
    std::array<std::pair<std::int64_t, std::string_view>, size> result = {{}};
    for (std::size_t i = 0; i < size; i++)
        result[i] = {first + static_cast<std::int64_t>(i) * step, std::string_view(strings.data[i].data(), strings.sizes[i])};
    return result;
}

/**
 * Reflection for `size` values, where i-th value is `first + i * step`, and its string is "value_<i>".
 */
template<std::size_t size, std::int64_t first, std::int64_t step>
struct generated_reflection {
    static constexpr generated_strings<size> strings = generate_strings<size>();
    static constexpr std::array<std::pair<std::int64_t, std::string_view>, size> value = make_reflection<generated_strings<size>, size>(strings, first, step);
};

/**
 * Reflection for values 1 to `size`, where the string for value i has i chars and is a prefix of the next one.
 */
template<std::size_t size>
struct prefix_reflection {
    static constexpr generated_strings<size, size> strings = generate_prefix_strings<size>();
    static constexpr std::array<std::pair<std::int64_t, std::string_view>, size> value = make_reflection<generated_strings<size, size>, size>(strings, 1, 1);
};

/**
 * Reflection for `size` sequential values, with strings of `string_size` chars each.
 */
template<std::size_t size, std::size_t string_size>
struct fixed_size_reflection {
    static constexpr generated_strings<size, string_size> strings = generate_fixed_size_strings<size, string_size>();
    static constexpr std::array<std::pair<std::int64_t, std::string_view>, size> value = make_reflection<generated_strings<size, string_size>, size>(strings, 0, 1);
};

/**
 * Enum table for a reflection, built the same way as `_SN_DEFINE_ENUM_STRING_TABLE` does it.
 */
template<class Reflection, enum_table_options options = enum_table_options(), class Hash = enum_table_hash>
struct table_for {
    static constexpr const auto &reflection = Reflection::value;
    static constexpr enum_table_spec spec = make_enum_table_spec(reflection, options);
    static constexpr enum_to_string_map<spec, Hash> to_string_map = make_enum_to_string_map<spec, Hash>(reflection);
    static constexpr string_to_enum_map<spec, Hash> from_string_map = make_string_to_enum_map<spec, Hash>(reflection);
    static constexpr string_enum_table_base<spec, Hash> value = string_enum_table_base<spec, Hash>(to_string_map, from_string_map);
};

template<std::size_t size, std::int64_t first, std::int64_t step, enum_table_options options = enum_table_options(), class Hash = enum_table_hash>
using generated_table = table_for<generated_reflection<size, first, step>, options, Hash>;

template<class Reflection>
[[nodiscard]] constexpr bool is_in_reflection(std::int64_t value) {
    for (const auto &[other, string] : Reflection::value)
        if (other == value)
            return true;
    return false;
}

/**
 * Checks all lookups in a table: every value and string of the reflection is found, and things that are close to
 * them are not.
 *
 * @param step                          Difference between consecutive values of the reflection, if they are evenly
 *                                      spaced. Zero otherwise.
 */
template<class Reflection, enum_table_options options = enum_table_options(), class Hash = enum_table_hash>
static void run_table_test(std::int64_t step) {
    using table = table_for<Reflection, options, Hash>;

    for (const auto &[value, string] : table::reflection) {
        std::string_view found_string;
        EXPECT_TRUE(table::value.find_string(static_cast<std::uint64_t>(value), &found_string)) << "with value = " << value;
        EXPECT_EQ(found_string, string) << "with value = " << value;

        // Copy the string so that it doesn't point into the table.
        std::string tmp(string);
        std::uint64_t found_value = 0;
        EXPECT_TRUE(table::value.find_value(tmp, &found_value)) << "with string = " << string;
        EXPECT_EQ(static_cast<std::int64_t>(found_value), value) << "with string = " << string;

        // Changing case of the first letter should only work for case-insensitive tables.
        std::size_t letter = tmp.find_first_of("abcdefghijklmnopqrstuvwxyz");
        if (letter != std::string::npos) {
            tmp[letter] = static_cast<char>(tmp[letter] - 'a' + 'A');
            if (options.mode == case_insensitive) {
                EXPECT_TRUE(table::value.find_value(tmp, &found_value)) << "with string = " << tmp;
                EXPECT_EQ(static_cast<std::int64_t>(found_value), value) << "with string = " << tmp;
            } else {
                EXPECT_FALSE(table::value.find_value(tmp, &found_value)) << "with string = " << tmp;
            }
        }

        // Values between the listed ones should not be found.
        for (std::int64_t other = value + 1; other < value + step && other < value + 4; other++)
            EXPECT_FALSE(table::value.find_string(static_cast<std::uint64_t>(other), &found_string)) << "with value = " << other;

        // Values that are the same as a listed one modulo 2^8, 2^16 or 2^32 should not be found either. Hash tables
        // store value - min in narrow types, and this is what would break if they compared keys in these types.
        for (std::uint64_t offset : {static_cast<std::uint64_t>(1) << 8, static_cast<std::uint64_t>(1) << 16, static_cast<std::uint64_t>(1) << 32}) {
            std::uint64_t other = static_cast<std::uint64_t>(value) + offset;
            if (!is_in_reflection<Reflection>(static_cast<std::int64_t>(other)))
                EXPECT_FALSE(table::value.find_string(other, &found_string)) << "with value = " << other;
        }

        // Same for strings that are almost right.
        tmp = std::string(string) + "_";
        EXPECT_FALSE(table::value.find_value(tmp, &found_value)) << "with string = " << tmp;
        tmp = std::string(string.substr(0, string.size() - 1)) + "_";
        EXPECT_FALSE(table::value.find_value(tmp, &found_value)) << "with string = " << tmp;
        tmp = std::string(string) + std::string(1, '\0');
        EXPECT_FALSE(table::value.find_value(tmp, &found_value)) << "with string = " << string << " + NUL";
        tmp = std::string(string);
        tmp.back() = '\0';
        EXPECT_FALSE(table::value.find_value(tmp, &found_value)) << "with string = " << string << " with a NUL at the end";
        for (std::size_t i = 0; i < string.size(); i++) {
            tmp = std::string(string);
            tmp[i] = static_cast<char>(tmp[i] | 0x80);
            EXPECT_FALSE(table::value.find_value(tmp, &found_value)) << "with string = " << string << " with a high bit at " << i;
        }
    }

    std::string_view found_string;
    std::int64_t first = table::reflection.front().first;
    std::int64_t last = table::reflection.back().first;
    EXPECT_FALSE(table::value.find_string(static_cast<std::uint64_t>(first - 1), &found_string));
    EXPECT_FALSE(table::value.find_string(static_cast<std::uint64_t>(last + 1), &found_string));
    if (step != 0)
        EXPECT_FALSE(table::value.find_string(static_cast<std::uint64_t>(last + step), &found_string));

    std::uint64_t found_value = 0;
    EXPECT_FALSE(table::value.find_value("", &found_value));
    EXPECT_FALSE(table::value.find_value("value_", &found_value));
    EXPECT_FALSE(table::value.find_value("no_such_value", &found_value));
}

template<std::size_t size, std::int64_t first, std::int64_t step, enum_table_options options = enum_table_options(), class Hash = enum_table_hash>
static void run_generated_table_test() {
    run_table_test<generated_reflection<size, first, step>, options, Hash>(step);
}


//
// Table kind selection.
//

constexpr enum_table_options default_options = sn::case_sensitive;
constexpr enum_table_options flat_options = sn::case_sensitive | sn::flat_enum_table;
constexpr enum_table_options hashed_options = sn::case_sensitive | sn::hashed_enum_table;
constexpr enum_table_options ci_options = sn::case_insensitive;

// Sequential values always go into a flat table.
static_assert(generated_table<1, 0, 1>::spec.flat);
static_assert(generated_table<10, 0, 1>::spec.flat);
static_assert(generated_table<1000, 0, 1>::spec.flat);
static_assert(generated_table<1000, -500, 1>::spec.flat);
static_assert(generated_table<1000, 1000000, 1>::spec.flat);

// Small ranges of values always go into a flat table.
static_assert(generated_table<2, 0, 255>::spec.flat);
static_assert(generated_table<2, 0, 255>::spec.to_string_slots == 256);
static_assert(!generated_table<2, 0, 256>::spec.flat);
static_assert(generated_table<2, -128, 255>::spec.flat);
static_assert(!generated_table<2, -128, 256>::spec.flat);

// Larger ranges go into a flat table if it's at most 4x larger than the number of values.
static_assert(generated_table<100, 0, 4>::spec.flat);
static_assert(generated_table<100, 0, 4>::spec.to_string_slots == 397);
static_assert(!generated_table<100, 0, 5>::spec.flat);
static_assert(generated_table<100, 0, 5>::spec.to_string_slots == 128);

// Unless the table kind is forced.
static_assert(generated_table<2, 0, 1000, flat_options>::spec.flat);
static_assert(generated_table<2, 0, 1000, flat_options>::spec.to_string_slots == 1001);
static_assert(!generated_table<10, 0, 1, hashed_options>::spec.flat);
static_assert(generated_table<10, 0, 1, hashed_options>::spec.to_string_slots == 16);

// String-to-enum tables are always hash tables.
static_assert(generated_table<10, 0, 1>::spec.from_string_slots == 16);
static_assert(generated_table<100, 0, 5>::spec.from_string_slots == 128);
static_assert(generated_table<1000, 0, 1>::spec.from_string_slots == 2048);
static_assert(!generated_table<10, 0, 1>::spec.fold_case);
static_assert(generated_table<10, 0, 1, ci_options>::spec.fold_case);

// Offsets into the strings are 1 byte up to 255 bytes of strings, and 2 bytes after that.
static_assert(table_for<fixed_size_reflection<51, 5>>::spec.to_string_data_size == 255);
static_assert(table_for<fixed_size_reflection<32, 8>>::spec.to_string_data_size == 256);
static_assert(table_for<fixed_size_reflection<1, 255>>::spec.to_string_data_size == 255);
static_assert(table_for<fixed_size_reflection<1, 256>>::spec.to_string_data_size == 256);
static_assert(table_for<fixed_size_reflection<1, 257>>::spec.to_string_data_size == 257);
static_assert(sizeof(enum_table_strings<86, 255>) == 87 + 255);
static_assert(sizeof(enum_table_strings<86, 256>) == 2 * 87 + 256);


//
// Tests. Both hash implementations should work on all platforms, so tables are tested with both.
//

template<class Hash>
class string_enum_table_test : public testing::Test {};

using hash_types = testing::Types<enum_table_hash_64, enum_table_hash_32>;
TYPED_TEST_SUITE(string_enum_table_test, hash_types);

TYPED_TEST(string_enum_table_test, sequential) {
    run_generated_table_test<1, 0, 1, default_options, TypeParam>();
    run_generated_table_test<2, 0, 1, default_options, TypeParam>();
    run_generated_table_test<10, 0, 1, default_options, TypeParam>();
    run_generated_table_test<255, 0, 1, default_options, TypeParam>();
    run_generated_table_test<256, 0, 1, default_options, TypeParam>();
    run_generated_table_test<1000, 0, 1, default_options, TypeParam>();
}

TYPED_TEST(string_enum_table_test, sequential_with_offset) {
    run_generated_table_test<10, -5, 1, default_options, TypeParam>();
    run_generated_table_test<300, -150, 1, default_options, TypeParam>();
    run_generated_table_test<300, 1000000, 1, default_options, TypeParam>();
}

TYPED_TEST(string_enum_table_test, gaps) {
    run_generated_table_test<2, 0, 255, default_options, TypeParam>();
    run_generated_table_test<2, -128, 255, default_options, TypeParam>();
    run_generated_table_test<100, 0, 4, default_options, TypeParam>();
    run_generated_table_test<100, -200, 4, default_options, TypeParam>();
}

TYPED_TEST(string_enum_table_test, sparse) {
    run_generated_table_test<2, 0, 256, default_options, TypeParam>();
    run_generated_table_test<2, -128, 256, default_options, TypeParam>();
    run_generated_table_test<100, 0, 5, default_options, TypeParam>();
    run_generated_table_test<300, 0, 1000, default_options, TypeParam>();
    run_generated_table_test<300, -150000, 1000, default_options, TypeParam>();
    run_generated_table_test<1000, 0, 7919, default_options, TypeParam>();
    run_generated_table_test<16, 0, 0x100000000, default_options, TypeParam>();
}

TYPED_TEST(string_enum_table_test, forced_kinds) {
    run_generated_table_test<2, 0, 1000, flat_options, TypeParam>();
    run_generated_table_test<10, 0, 1, hashed_options, TypeParam>();
    run_generated_table_test<300, 0, 1, hashed_options, TypeParam>();
    run_generated_table_test<300, -150, 1, hashed_options, TypeParam>();
}

TYPED_TEST(string_enum_table_test, case_insensitive) {
    run_generated_table_test<10, 0, 1, ci_options, TypeParam>();
    run_generated_table_test<300, 0, 1000, ci_options, TypeParam>();
}

TYPED_TEST(string_enum_table_test, string_sizes) {
    // Strings are hashed and compared in machine words, so all sizes around word boundaries should be covered, and
    // every string here is a prefix of the next one.
    run_table_test<prefix_reflection<70>, default_options, TypeParam>(1);
    run_table_test<prefix_reflection<70>, ci_options, TypeParam>(1);
}

TYPED_TEST(string_enum_table_test, data_size_boundaries) {
    run_table_test<fixed_size_reflection<51, 5>, default_options, TypeParam>(1);
    run_table_test<fixed_size_reflection<51, 5>, ci_options, TypeParam>(1);
    run_table_test<fixed_size_reflection<32, 8>, default_options, TypeParam>(1);
    run_table_test<fixed_size_reflection<32, 8>, ci_options, TypeParam>(1);
    run_table_test<fixed_size_reflection<1, 255>, default_options, TypeParam>(1);
    run_table_test<fixed_size_reflection<1, 256>, default_options, TypeParam>(1);
    run_table_test<fixed_size_reflection<1, 257>, default_options, TypeParam>(1);
}

// Hash that gives the same hash to all strings with seed 0, and works normally with other seeds. String-to-enum tables
// should then build with seed 1, and use it for lookups.
struct colliding_hash : enum_table_hash_64 {
    [[nodiscard]] static constexpr hash_type hash_string(const char *p, std::size_t size, bool fold_case, hash_type seed) noexcept {
        return seed == 0 ? 1 : enum_table_hash_64::hash_string(p, size, fold_case, seed);
    }
};

TEST(string_enum_table, hash_collisions) {
    run_generated_table_test<1, 0, 1, default_options, colliding_hash>();
    run_generated_table_test<10, 0, 1, default_options, colliding_hash>();
    run_generated_table_test<300, 0, 1000, ci_options, colliding_hash>();
}

TEST(string_enum_table, duplicate_strings) {
    using map_type = hashed_string_enum_map<4, 6, std::uint8_t, true>;
    using cs_map_type = hashed_string_enum_map<4, 6, std::uint8_t, false>;
    std::array<std::pair<std::string_view, std::uint64_t>, 2> pairs = {{{"abc", 0}, {"ABC", 1}}};

    // Strings that only differ in case are duplicates for a case-insensitive table, but not for a case-sensitive one.
    EXPECT_THROW((void) map_type(pairs, 0), std::logic_error);

    cs_map_type cs_map(pairs, 0);
    std::uint64_t value = 0;
    EXPECT_TRUE(cs_map.find("abc", &value));
    EXPECT_EQ(value, 0u);
    EXPECT_TRUE(cs_map.find("ABC", &value));
    EXPECT_EQ(value, 1u);
    EXPECT_FALSE(cs_map.find("Abc", &value));

    pairs[1].first = "abc";
    EXPECT_THROW((void) cs_map_type(pairs, 0), std::logic_error);
}

TEST(string_enum_table, spec_errors) {
    std::array<std::pair<int, std::string_view>, 2> two_empty_strings = {{{0, ""}, {1, ""}}};
    EXPECT_THROW((void) make_enum_table_spec(two_empty_strings, sn::case_sensitive), std::logic_error);

    std::array<std::pair<int, std::string_view>, 2> huge_range = {{{0, "a"}, {1000000, "b"}}};
    EXPECT_THROW((void) make_enum_table_spec(huge_range, sn::case_sensitive | sn::flat_enum_table), std::logic_error);
    EXPECT_FALSE(make_enum_table_spec(huge_range, sn::case_sensitive).flat);
    EXPECT_FALSE(make_enum_table_spec(huge_range, sn::case_sensitive | sn::hashed_enum_table).flat);
}

} // namespace sn::detail
