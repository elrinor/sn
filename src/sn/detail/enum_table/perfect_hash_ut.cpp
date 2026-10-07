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

template<class Hash, class Key, std::size_t key_count>
static void expect_perfect(const std::array<Key, key_count> &keys, const char *description) {
    constexpr std::size_t size = perfect_hash_size(key_count);
    perfect_hash<size, Key, Hash> hash = make_perfect_hash<size, Hash>(keys);

    std::vector<bool> used(size);
    for (Key key : keys) {
        std::size_t slot = hash.slot(key);
        ASSERT_LT(slot, size) << "with key_count = " << key_count << " for " << description;
        EXPECT_FALSE(used[slot]) << "with key_count = " << key_count << " for " << description;
        used[slot] = true;
    }
}

template<class Hash, class Key, std::size_t key_count>
static void run_perfect_hash_tests() {
    std::array<Key, key_count> keys = {{}};

    for (std::size_t i = 0; i < key_count; i++)
        keys[i] = static_cast<Key>(i);
    expect_perfect<Hash>(keys, "sequential keys");

    if constexpr (sizeof(Key) == 8) {
        for (std::size_t i = 0; i < key_count; i++)
            keys[i] = static_cast<Key>(i) << 32;
        expect_perfect<Hash>(keys, "keys that only differ in high bits");
    }

    for (std::size_t i = 0; i < key_count; i++)
        keys[i] = static_cast<Key>(static_cast<Key>(i) * 0x10000u + 0xFFFFu);
    expect_perfect<Hash>(keys, "keys with the same low bits");

    for (std::size_t i = 0; i < key_count; i++)
        keys[i] = static_cast<Key>(static_cast<Key>(0) - static_cast<Key>(i));
    expect_perfect<Hash>(keys, "negative keys");

    for (std::uint64_t seed = 1; seed <= 10; seed++) {
        std::uint64_t state = seed;
        for (std::size_t i = 0; i < key_count; i++)
            keys[i] = static_cast<Key>(next_random(&state));
        expect_perfect<Hash>(keys, "random keys");
    }
}

// Keys that fit into a machine word and keys that don't are hashed differently, so we check both for each of the
// hash implementations.
template<class Hash, std::size_t key_count>
static void run_perfect_hash_tests() {
    run_perfect_hash_tests<Hash, std::uint64_t, key_count>();
    run_perfect_hash_tests<Hash, std::uint32_t, key_count>();
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

    std::array<std::uint32_t, 32> narrow_keys = {{}};
    for (std::size_t i = 0; i < narrow_keys.size(); i++)
        narrow_keys[i] = static_cast<std::uint32_t>(1) << i;
    expect_perfect<TypeParam>(narrow_keys, "powers of two");
}

template<class Hash, class Key>
static void run_constexpr_hash_test() {
    static constexpr std::array<Key, 5> keys = {{1, 100, 10000, 1000000, 100000000}};
    static constexpr perfect_hash<8, Key, Hash> hash = make_perfect_hash<8, Hash>(keys);
    static_assert(hash.slot(1) != hash.slot(100));

    // Hash built at compile time should work at run time.
    std::vector<bool> used(8);
    for (Key key : keys) {
        volatile Key runtime_key = key; // Volatile so that the call below is not constant-evaluated.
        std::size_t slot = hash.slot(runtime_key);
        ASSERT_LT(slot, 8u);
        EXPECT_FALSE(used[slot]);
        used[slot] = true;
    }
}

TYPED_TEST(perfect_hash_test, constexpr_hash) {
    run_constexpr_hash_test<TypeParam, std::uint64_t>();
    run_constexpr_hash_test<TypeParam, std::uint32_t>();
}

// Hash that puts all keys into one bucket with first-level seed 1, and works normally with other seeds.
struct overflowing_hash : enum_table_hash_64 {
    template<class Key>
    [[nodiscard]] static constexpr hash_type mix(Key key, hash_type seed) noexcept {
        return seed == 1 ? 0 : enum_table_hash_64::mix(key, seed);
    }
};

// Hash that maps every key to zero with every seed, so that no perfect hash can be built.
struct constant_hash : enum_table_hash_64 {
    template<class Key>
    [[nodiscard]] static constexpr hash_type mix(Key, hash_type) noexcept {
        return 0;
    }
};

TEST(perfect_hash, first_seed_retry) {
    // A few keys fit into one bucket, and are then placed with the second-level seeds.
    std::array<std::uint64_t, 4> small_keys = {{}};
    for (std::size_t i = 0; i < small_keys.size(); i++)
        small_keys[i] = i * 10;
    perfect_hash<64, std::uint64_t, overflowing_hash> small_hash = make_perfect_hash<64, overflowing_hash>(small_keys);
    EXPECT_EQ(small_hash.first_seed, 1u);
    expect_perfect<overflowing_hash>(small_keys, "keys in one bucket");

    // More keys than that don't, and the first-level seed is retried.
    std::array<std::uint64_t, max_perfect_hash_bucket_size + 1> big_keys = {{}};
    for (std::size_t i = 0; i < big_keys.size(); i++)
        big_keys[i] = i * 10;
    perfect_hash<64, std::uint64_t, overflowing_hash> big_hash = make_perfect_hash<64, overflowing_hash>(big_keys);
    EXPECT_EQ(big_hash.first_seed, 2u);
    expect_perfect<overflowing_hash>(big_keys, "keys that overflow one bucket");
}

TEST(perfect_hash, unbuildable) {
    std::array<std::uint64_t, 3> keys = {{1, 2, 3}};
    EXPECT_THROW((void) (make_perfect_hash<4, constant_hash>(keys)), std::logic_error);
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
