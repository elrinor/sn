#include <array>
#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>

#include <gtest/gtest.h> // NOLINT: not a C system header.

#include "enum_table_hash.h"

namespace sn::detail {

// Simple deterministic generator, we don't want to depend on <random> implementations here.
static std::uint64_t next_random(std::uint64_t *state) {
    *state += 0x9E3779B97F4A7C15u;
    std::uint64_t z = *state;
    z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9u;
    z = (z ^ (z >> 27)) * 0x94D049BB133111EBu;
    return z ^ (z >> 31);
}

static std::string make_test_string(std::size_t size, std::uint64_t seed) {
    std::string result;
    for (std::size_t i = 0; i < size; i++)
        result.push_back(static_cast<char>('a' + next_random(&seed) % 26));
    return result;
}

// Both hash implementations should work on all platforms, so we test both.
template<class Hash>
class enum_table_hash_test : public testing::Test {};

using hash_types = testing::Types<enum_table_hash_64, enum_table_hash_32>;
TYPED_TEST_SUITE(enum_table_hash_test, hash_types);


TEST(enum_table_hash, multiply_fold_known_values) {
    constexpr std::uint64_t max64 = 0xFFFFFFFFFFFFFFFFu;
    constexpr std::uint32_t max32 = 0xFFFFFFFFu;

    EXPECT_EQ(multiply_fold(static_cast<std::uint64_t>(0), static_cast<std::uint64_t>(0)), 0u);
    EXPECT_EQ(multiply_fold(static_cast<std::uint64_t>(1), static_cast<std::uint64_t>(1)), 1u);
    EXPECT_EQ(multiply_fold(static_cast<std::uint64_t>(max32), static_cast<std::uint64_t>(max32)), 0xFFFFFFFE00000001u);

    // (2^64 - 1)^2 is 2^128 - 2^65 + 1, so the high half is 2^64 - 2, and the low half is 1.
    EXPECT_EQ(multiply_fold(max64, max64), (max64 - 1) ^ 1u);

    // 2^32 * 2^32 is 2^64, so the high half is 1, and the low half is 0.
    EXPECT_EQ(multiply_fold(static_cast<std::uint64_t>(0x100000000u), static_cast<std::uint64_t>(0x100000000u)), 1u);

    // Same for the 32-bit version.
    EXPECT_EQ(multiply_fold(static_cast<std::uint32_t>(0), static_cast<std::uint32_t>(0)), 0u);
    EXPECT_EQ(multiply_fold(static_cast<std::uint32_t>(1), static_cast<std::uint32_t>(1)), 1u);
    EXPECT_EQ(multiply_fold(max32, max32), (max32 - 1) ^ 1u);
    EXPECT_EQ(multiply_fold(static_cast<std::uint32_t>(0x10000u), static_cast<std::uint32_t>(0x10000u)), 1u);
}

TEST(enum_table_hash, multiply_fold_portable) {
    std::uint64_t state = 1;
    for (std::size_t i = 0; i < 10000; i++) {
        std::uint64_t a = next_random(&state);
        std::uint64_t b = next_random(&state);
        EXPECT_EQ(multiply_fold(a, b), multiply_fold_portable(a, b)) << "with a = " << a << " and b = " << b;
    }
}

TEST(enum_table_hash, multiply_fold_constexpr) {
    static constexpr std::uint64_t a = 0x0123456789ABCDEFu;
    static constexpr std::uint64_t b = 0xFEDCBA9876543210u;
    static constexpr std::uint64_t expected = multiply_fold(a, b);

    // Volatile so that the call below is not constant-evaluated.
    volatile std::uint64_t runtime_a = a;
    volatile std::uint64_t runtime_b = b;
    EXPECT_EQ(multiply_fold(static_cast<std::uint64_t>(runtime_a), static_cast<std::uint64_t>(runtime_b)), expected);
}

TEST(enum_table_hash, to_lower_ascii_word) {
    for (std::size_t pos = 0; pos < 8; pos++) {
        for (std::size_t c = 0; c < 256; c++) {
            std::uint64_t word = 0x4142434445464748u; // "HGFEDCBA".
            std::uint64_t expected = 0x6162636465666768u; // "hgfedcba".

            std::uint64_t mask = static_cast<std::uint64_t>(0xFFu) << (8 * pos);
            std::uint64_t lower = static_cast<unsigned char>(to_lower_ascii(static_cast<char>(c)));
            word = (word & ~mask) | (static_cast<std::uint64_t>(c) << (8 * pos));
            expected = (expected & ~mask) | (lower << (8 * pos));

            EXPECT_EQ(to_lower_ascii_word(word), expected) << "with c = " << c << " at pos = " << pos;
            EXPECT_EQ(to_lower_ascii_word(static_cast<std::uint32_t>(word)), static_cast<std::uint32_t>(expected)) << "with c = " << c << " at pos = " << pos;
        }
    }
}

template<class Hash, class Key>
static void run_mix_test() {
    constexpr Key high_bit = static_cast<Key>(static_cast<Key>(1) << (8 * sizeof(Key) - 1));

    // Changing the seed should change the hash, for any key.
    std::uint64_t state = 1;
    for (std::size_t i = 0; i < 1000; i++) {
        Key key = static_cast<Key>(next_random(&state));
        EXPECT_NE(Hash::mix(key, 1), Hash::mix(key, 2)) << "with key = " << key;
        EXPECT_NE(Hash::mix(key, 1), Hash::mix(static_cast<Key>(key + 1), 1)) << "with key = " << key;
        EXPECT_NE(Hash::mix(key, 1), Hash::mix(static_cast<Key>(key ^ high_bit), 1)) << "with key = " << key;
    }

    // Hashes computed at compile time should be the same as the ones computed at run time.
    static constexpr Key constexpr_key = static_cast<Key>(0x0123456789ABCDEFu);
    static constexpr auto expected = Hash::mix(constexpr_key, 123);
    volatile Key runtime_key = constexpr_key; // Volatile so that the call below is not constant-evaluated.
    EXPECT_EQ(Hash::mix(static_cast<Key>(runtime_key), 123), expected);
}

TYPED_TEST(enum_table_hash_test, mix) {
    // Keys that fit into a machine word and keys that don't are hashed differently, so we check both.
    run_mix_test<TypeParam, std::uint64_t>();
    run_mix_test<TypeParam, std::uint32_t>();
}

TYPED_TEST(enum_table_hash_test, mix_narrow_keys) {
    using hash = TypeParam;
    using hash_type = typename hash::hash_type;

    // Keys that are narrower than a machine word should hash the same as machine words.
    for (std::uint32_t key : {0u, 1u, 200u, 255u}) {
        EXPECT_EQ(hash::mix(static_cast<std::uint8_t>(key), 7), hash::mix(static_cast<hash_type>(key), 7)) << "with key = " << key;
        EXPECT_EQ(hash::mix(static_cast<std::uint16_t>(key), 7), hash::mix(static_cast<hash_type>(key), 7)) << "with key = " << key;
    }
}

TYPED_TEST(enum_table_hash_test, hash_string_constexpr) {
    using hash = TypeParam;

    static constexpr std::string_view string = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

    static constexpr auto expected = [] {
        std::array<std::array<typename hash::hash_type, 2>, 64> result = {{}};
        for (std::size_t size = 0; size < string.size(); size++) {
            auto seed = static_cast<typename hash::hash_type>(size);
            result[size][0] = hash::hash_string(string.data(), size, false, seed);
            result[size][1] = hash::hash_string(string.data(), size, true, seed);
        }
        return result;
    }();

    // Copy into a std::string so that hashing is done at run time, and so that reading past the end is detectable.
    for (std::size_t size = 0; size < string.size(); size++) {
        std::string tmp(string.substr(0, size));
        auto seed = static_cast<typename hash::hash_type>(size);
        EXPECT_EQ(hash::hash_string(tmp.data(), tmp.size(), false, seed), expected[size][0]) << "with size = " << size;
        EXPECT_EQ(hash::hash_string(tmp.data(), tmp.size(), true, seed), expected[size][1]) << "with size = " << size;
    }
}

TYPED_TEST(enum_table_hash_test, hash_string_sensitivity) {
    using hash = TypeParam;

    // Changing any char should change the hash, for any string size.
    for (std::size_t size = 1; size <= 70; size++) {
        std::string string = make_test_string(size, size);
        auto value = hash::hash_string(string.data(), string.size(), false, 0);

        for (std::size_t pos = 0; pos < size; pos++) {
            std::string changed = string;
            changed[pos] = '#';
            EXPECT_NE(hash::hash_string(changed.data(), changed.size(), false, 0), value) << "with size = " << size << " and pos = " << pos;
        }

        // Same for adding a char, dropping one, or changing the seed.
        std::string longer = string + "a";
        EXPECT_NE(hash::hash_string(longer.data(), longer.size(), false, 0), value) << "with size = " << size;
        EXPECT_NE(hash::hash_string(string.data(), string.size() - 1, false, 0), value) << "with size = " << size;
        EXPECT_NE(hash::hash_string(string.data(), string.size(), false, 1), value) << "with size = " << size;
    }
}

TYPED_TEST(enum_table_hash_test, hash_string_fold_case) {
    using hash = TypeParam;

    for (std::size_t size = 1; size <= 70; size++) {
        std::string lower = make_test_string(size, size);
        auto value = hash::hash_string(lower.data(), lower.size(), true, 0);

        for (std::size_t pos = 0; pos < size; pos++) {
            std::string mixed = lower;
            mixed[pos] = static_cast<char>(mixed[pos] - 'a' + 'A');
            EXPECT_EQ(hash::hash_string(mixed.data(), mixed.size(), true, 0), value) << "with size = " << size << " and pos = " << pos;
            EXPECT_NE(hash::hash_string(mixed.data(), mixed.size(), false, 0), hash::hash_string(lower.data(), lower.size(), false, 0));
        }
    }

    // Non-ascii chars should not be folded.
    std::string_view upper = "\xd0\x94\xd0\xbe\xd0\xbc"; // "Dom" (House) in Russian.
    std::string_view lower = "\xd0\xb4\xd0\xbe\xd0\xbc"; // "dom" (house) in Russian.
    EXPECT_NE(hash::hash_string(upper.data(), upper.size(), true, 0), hash::hash_string(lower.data(), lower.size(), true, 0));
}

TEST(enum_table_hash, equal_strings) {
    for (std::size_t size = 0; size <= 70; size++) {
        std::string string = make_test_string(size, size);
        std::string tmp = string;
        EXPECT_TRUE(equal_strings(tmp.data(), string.data(), size, false)) << "with size = " << size;
        EXPECT_TRUE(equal_strings(tmp.data(), string.data(), size, true)) << "with size = " << size;

        for (std::size_t pos = 0; pos < size; pos++) {
            std::string changed = string;
            changed[pos] = '#';
            EXPECT_FALSE(equal_strings(changed.data(), string.data(), size, false)) << "with size = " << size << " and pos = " << pos;
            EXPECT_FALSE(equal_strings(changed.data(), string.data(), size, true)) << "with size = " << size << " and pos = " << pos;

            std::string mixed = string;
            mixed[pos] = static_cast<char>(mixed[pos] - 'a' + 'A');
            EXPECT_FALSE(equal_strings(mixed.data(), string.data(), size, false)) << "with size = " << size << " and pos = " << pos;
            EXPECT_TRUE(equal_strings(mixed.data(), string.data(), size, true)) << "with size = " << size << " and pos = " << pos;
        }
    }
}

} // namespace sn::detail
