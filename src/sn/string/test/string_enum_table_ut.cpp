#include <array>
#include <cstddef>
#include <cstdint>
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

template<std::size_t size>
struct generated_strings {
    std::array<std::array<char, 16>, size> data = {{}};
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
 * Reflection for `size` values, where i-th value is `first + i * step`, and its string is "value_<i>".
 */
template<std::size_t size, std::int64_t first, std::int64_t step>
struct generated_reflection {
    static constexpr generated_strings<size> strings = generate_strings<size>();

    static constexpr std::array<std::pair<std::int64_t, std::string_view>, size> value = [] {
        std::array<std::pair<std::int64_t, std::string_view>, size> result = {{}};
        for (std::size_t i = 0; i < size; i++)
            result[i] = {first + static_cast<std::int64_t>(i) * step, std::string_view(strings.data[i].data(), strings.sizes[i])};
        return result;
    }();
};

template<std::size_t size, std::int64_t first, std::int64_t step, enum_table_options options = enum_table_options()>
struct generated_table {
    static constexpr const auto &reflection = generated_reflection<size, first, step>::value;
    static constexpr enum_table_spec spec = make_enum_table_spec(reflection, options);
    static constexpr enum_to_string_map<spec> to_string_map = make_enum_to_string_map<spec>(reflection);
    static constexpr string_to_enum_map<spec> from_string_map = make_string_to_enum_map<spec>(reflection);
    static constexpr string_enum_table_base<spec> value = string_enum_table_base<spec>(to_string_map, from_string_map);
};

template<std::size_t size, std::int64_t first, std::int64_t step, enum_table_options options = enum_table_options()>
static void run_generated_table_test() {
    using table = generated_table<size, first, step, options>;

    for (const auto &[value, string] : table::reflection) {
        std::string_view found_string;
        EXPECT_TRUE(table::value.find_string(static_cast<std::uint64_t>(value), &found_string)) << "with value = " << value;
        EXPECT_EQ(found_string, string) << "with value = " << value;

        // Copy the string so that it doesn't point into the table.
        std::string tmp(string);
        std::uint64_t found_value = 0;
        EXPECT_TRUE(table::value.find_value(tmp, &found_value)) << "with string = " << string;
        EXPECT_EQ(static_cast<std::int64_t>(found_value), value) << "with string = " << string;

        if (options.mode == case_insensitive) {
            tmp[0] = 'V';
            EXPECT_TRUE(table::value.find_value(tmp, &found_value)) << "with string = " << tmp;
            EXPECT_EQ(static_cast<std::int64_t>(found_value), value) << "with string = " << tmp;
        } else {
            tmp[0] = 'V';
            EXPECT_FALSE(table::value.find_value(tmp, &found_value)) << "with string = " << tmp;
        }

        // Values between the listed ones should not be found.
        for (std::int64_t other = value + 1; other < value + step && other < value + 4; other++)
            EXPECT_FALSE(table::value.find_string(static_cast<std::uint64_t>(other), &found_string)) << "with value = " << other;

        // Same for strings that are almost right.
        tmp = std::string(string) + "_";
        EXPECT_FALSE(table::value.find_value(tmp, &found_value)) << "with string = " << tmp;
        tmp = std::string(string.substr(0, string.size() - 1)) + "_";
        EXPECT_FALSE(table::value.find_value(tmp, &found_value)) << "with string = " << tmp;
    }

    std::string_view found_string;
    std::int64_t last = first + static_cast<std::int64_t>(size - 1) * step;
    EXPECT_FALSE(table::value.find_string(static_cast<std::uint64_t>(first - 1), &found_string));
    EXPECT_FALSE(table::value.find_string(static_cast<std::uint64_t>(last + 1), &found_string));
    EXPECT_FALSE(table::value.find_string(static_cast<std::uint64_t>(last + step), &found_string));

    std::uint64_t found_value = 0;
    EXPECT_FALSE(table::value.find_value("", &found_value));
    EXPECT_FALSE(table::value.find_value("value_", &found_value));
    EXPECT_FALSE(table::value.find_value("no_such_value", &found_value));
}


//
// Table kind selection.
//

constexpr enum_table_options flat_options = sn::case_sensitive | sn::flat_enum_table;
constexpr enum_table_options hashed_options = sn::case_sensitive | sn::hashed_enum_table;
constexpr enum_table_options ci_options = sn::case_insensitive;

// Sequential values always go into a flat table.
static_assert(generated_table<1, 0, 1>::spec.flat);
static_assert(generated_table<10, 0, 1>::spec.flat);
static_assert(generated_table<1000, 0, 1>::spec.flat);
static_assert(generated_table<1000, -500, 1>::spec.flat);
static_assert(generated_table<1000, 1000000, 1>::spec.flat);

// Tables for ranges of up to 256 values are always flat.
static_assert(generated_table<2, 0, 255>::spec.flat);
static_assert(generated_table<2, 0, 255>::spec.to_string_slots == 256);
static_assert(!generated_table<2, 0, 256>::spec.flat);
static_assert(generated_table<2, -128, 255>::spec.flat);
static_assert(!generated_table<2, -128, 256>::spec.flat);

// Otherwise a table is flat if it has at most 4 slots per value.
static_assert(generated_table<100, 0, 4>::spec.flat);
static_assert(generated_table<100, 0, 4>::spec.to_string_slots == 397);
static_assert(!generated_table<100, 0, 5>::spec.flat);
static_assert(generated_table<100, 0, 5>::spec.to_string_slots == 128);

// And the kind can be forced.
static_assert(generated_table<2, 0, 1000, flat_options>::spec.flat);
static_assert(generated_table<2, 0, 1000, flat_options>::spec.to_string_slots == 1001);
static_assert(!generated_table<10, 0, 1, hashed_options>::spec.flat);
static_assert(generated_table<10, 0, 1, hashed_options>::spec.to_string_slots == 16);

// String-to-enum tables are always hashed.
static_assert(generated_table<10, 0, 1>::spec.from_string_slots == 16);
static_assert(generated_table<100, 0, 5>::spec.from_string_slots == 128);
static_assert(generated_table<1000, 0, 1>::spec.from_string_slots == 2048);
static_assert(!generated_table<10, 0, 1>::spec.fold_case);
static_assert(generated_table<10, 0, 1, ci_options>::spec.fold_case);


//
// Tests.
//

TEST(string_enum_table, sequential) {
    run_generated_table_test<1, 0, 1>();
    run_generated_table_test<2, 0, 1>();
    run_generated_table_test<10, 0, 1>();
    run_generated_table_test<255, 0, 1>();
    run_generated_table_test<256, 0, 1>();
    run_generated_table_test<1000, 0, 1>();
}

TEST(string_enum_table, sequential_with_offset) {
    run_generated_table_test<10, -5, 1>();
    run_generated_table_test<300, -150, 1>();
    run_generated_table_test<300, 1000000, 1>();
}

TEST(string_enum_table, gaps) {
    run_generated_table_test<2, 0, 255>();
    run_generated_table_test<2, -128, 255>();
    run_generated_table_test<100, 0, 4>();
    run_generated_table_test<100, -200, 4>();
}

TEST(string_enum_table, sparse) {
    run_generated_table_test<2, 0, 256>();
    run_generated_table_test<2, -128, 256>();
    run_generated_table_test<100, 0, 5>();
    run_generated_table_test<300, 0, 1000>();
    run_generated_table_test<300, -150000, 1000>();
    run_generated_table_test<1000, 0, 7919>();
    run_generated_table_test<16, 0, 0x100000000>();
}

TEST(string_enum_table, forced_kinds) {
    run_generated_table_test<2, 0, 1000, flat_options>();
    run_generated_table_test<10, 0, 1, hashed_options>();
    run_generated_table_test<300, 0, 1, hashed_options>();
    run_generated_table_test<300, -150, 1, hashed_options>();
}

TEST(string_enum_table, case_insensitive) {
    run_generated_table_test<10, 0, 1, ci_options>();
    run_generated_table_test<300, 0, 1000, ci_options>();
}

} // namespace sn::detail
