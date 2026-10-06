#include <array>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <vector>

#include <gtest/gtest.h> // NOLINT: not a C system header.

#include "perfect_hash.h"

namespace sn::detail {

// Simple deterministic generator, we don't want to depend on <random> implementations here.
static std::uint64_t next_random(std::uint64_t *state) {
    *state += 0x9E3779B97F4A7C15u;
    std::uint64_t z = *state;
    z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9u;
    z = (z ^ (z >> 27)) * 0x94D049BB133111EBu;
    return z ^ (z >> 31);
}

// Both hash implementations should work on all platforms, so we test both.
template<class Hash>
class perfect_hash_test : public testing::Test {};

using hash_types = testing::Types<enum_table_hash_64, enum_table_hash_32>;
TYPED_TEST_SUITE(perfect_hash_test, hash_types);

template<class Hash, std::size_t key_count>
static void expect_perfect(const std::array<std::uint64_t, key_count> &keys, const char *description) {
    constexpr std::size_t size = perfect_hash_size(key_count);
    perfect_hash<size, Hash> hash = make_perfect_hash<size, Hash>(keys);

    std::vector<bool> used(size);
    for (std::uint64_t key : keys) {
        std::size_t slot = hash.slot(key);
        ASSERT_LT(slot, size) << "with key_count = " << key_count << " for " << description;
        EXPECT_FALSE(used[slot]) << "with key_count = " << key_count << " for " << description;
        used[slot] = true;
    }
}

template<class Hash, std::size_t key_count>
static void run_perfect_hash_tests() {
    std::array<std::uint64_t, key_count> keys = {{}};

    for (std::size_t i = 0; i < key_count; i++)
        keys[i] = i;
    expect_perfect<Hash>(keys, "sequential keys");

    for (std::size_t i = 0; i < key_count; i++)
        keys[i] = static_cast<std::uint64_t>(i) << 32;
    expect_perfect<Hash>(keys, "keys that only differ in high bits");

    for (std::size_t i = 0; i < key_count; i++)
        keys[i] = static_cast<std::uint64_t>(i) * 0x10000u + 0xFFFFu;
    expect_perfect<Hash>(keys, "keys with the same low bits");

    for (std::size_t i = 0; i < key_count; i++)
        keys[i] = static_cast<std::uint64_t>(0) - i;
    expect_perfect<Hash>(keys, "negative keys");

    for (std::uint64_t seed = 1; seed <= 10; seed++) {
        std::uint64_t state = seed;
        for (std::size_t i = 0; i < key_count; i++)
            keys[i] = next_random(&state);
        expect_perfect<Hash>(keys, "random keys");
    }
}

TEST(perfect_hash, size) {
    EXPECT_EQ(perfect_hash_size(0), 1u);
    EXPECT_EQ(perfect_hash_size(1), 2u);
    EXPECT_EQ(perfect_hash_size(3), 4u);
    EXPECT_EQ(perfect_hash_size(4), 8u);
    EXPECT_EQ(perfect_hash_size(16), 32u);
    EXPECT_EQ(perfect_hash_size(800), 1024u);
    EXPECT_EQ(perfect_hash_size(819), 1024u);
    EXPECT_EQ(perfect_hash_size(820), 2048u);
}

TYPED_TEST(perfect_hash_test, key_counts) {
    run_perfect_hash_tests<TypeParam, 0>();
    run_perfect_hash_tests<TypeParam, 1>();
    run_perfect_hash_tests<TypeParam, 2>();
    run_perfect_hash_tests<TypeParam, 3>();
    run_perfect_hash_tests<TypeParam, 4>();
    run_perfect_hash_tests<TypeParam, 5>();
    run_perfect_hash_tests<TypeParam, 7>();
    run_perfect_hash_tests<TypeParam, 8>();
    run_perfect_hash_tests<TypeParam, 16>();
    run_perfect_hash_tests<TypeParam, 17>();
    run_perfect_hash_tests<TypeParam, 31>();
    run_perfect_hash_tests<TypeParam, 64>();
    run_perfect_hash_tests<TypeParam, 100>();
    run_perfect_hash_tests<TypeParam, 255>();
    run_perfect_hash_tests<TypeParam, 256>();
    run_perfect_hash_tests<TypeParam, 819>(); // Largest key count for 1024 slots.
    run_perfect_hash_tests<TypeParam, 1000>();
    run_perfect_hash_tests<TypeParam, 5000>();
}

TYPED_TEST(perfect_hash_test, powers_of_two) {
    std::array<std::uint64_t, 64> keys = {{}};
    for (std::size_t i = 0; i < keys.size(); i++)
        keys[i] = static_cast<std::uint64_t>(1) << i;
    expect_perfect<TypeParam>(keys, "powers of two");
}

TYPED_TEST(perfect_hash_test, constexpr_hash) {
    static constexpr std::array<std::uint64_t, 5> keys = {{1, 100, 10000, 1000000, 100000000}};
    static constexpr perfect_hash<8, TypeParam> hash = make_perfect_hash<8, TypeParam>(keys);
    static_assert(hash.slot(1) != hash.slot(100));

    // Hash built at compile time should work at run time.
    std::vector<bool> used(8);
    for (std::uint64_t key : keys) {
        volatile std::uint64_t runtime_key = key; // Volatile so that the call below is not constant-evaluated.
        std::size_t slot = hash.slot(runtime_key);
        ASSERT_LT(slot, 8u);
        EXPECT_FALSE(used[slot]);
        used[slot] = true;
    }
}

TYPED_TEST(perfect_hash_test, duplicate_keys) {
    std::array<std::uint64_t, 3> keys = {{1, 2, 1}};
    EXPECT_TRUE(has_duplicate_keys(keys));
    EXPECT_THROW((void) (make_perfect_hash<4, TypeParam>(keys)), std::logic_error);

    keys[2] = 3;
    EXPECT_FALSE(has_duplicate_keys(keys));
    EXPECT_NO_THROW((void) (make_perfect_hash<4, TypeParam>(keys)));
}

} // namespace sn::detail
