#pragma once

#include <algorithm> // For std::max.
#include <array>
#include <bit> // For std::bit_ceil, std::has_single_bit.
#include <cstddef>
#include <cstdint>
#include <stdexcept> // For std::logic_error.

#include "enum_table_hash.h"

namespace sn::detail {

/**
 * @param key_count                     Number of keys.
 * @return                              Number of slots that `perfect_hash` uses for this number of keys. Always a
 *                                      power of two, with at least 25% of the slots left empty.
 */
[[nodiscard]] constexpr std::size_t perfect_hash_size(std::size_t key_count) noexcept {
    return std::bit_ceil(key_count + key_count / 4 + 1);
}

/**
 * Keys grouped into buckets by hash. Indices of the keys that ended up in bucket `b` are `order[start[b]]` to
 * `order[start[b + 1] - 1]`, in ascending order.
 *
 * This is the first step in building a `perfect_hash`. It's also a cheap way to find equal keys, as they end up in
 * the same bucket. We could have used `std::sort` for that, but sorting is a lot more expensive at compile time.
 *
 * @tparam size                         Number of buckets. Must be a power of two.
 * @tparam Hash                         Hash functions to use.
 * @tparam Key                          Type of the keys, an unsigned integer.
 * @tparam key_count                    Number of keys.
 */
template<std::size_t size, class Hash, class Key, std::size_t key_count>
struct hash_buckets {
    std::array<std::size_t, size + 1> start = {{}};
    std::array<std::size_t, key_count> order = {{}};
    std::size_t max_bucket_size = 0;

    constexpr hash_buckets(const std::array<Key, key_count> &keys, std::uint32_t seed) {
        std::array<std::size_t, key_count> bucket_of = {{}};
        for (std::size_t i = 0; i < key_count; i++) {
            bucket_of[i] = static_cast<std::size_t>(Hash::mix(keys[i], seed) & (size - 1));
            start[bucket_of[i] + 1]++;
        }
        for (std::size_t b = 0; b < size; b++) {
            max_bucket_size = std::max(max_bucket_size, start[b + 1]);
            start[b + 1] += start[b];
        }
        std::array<std::size_t, size> filled = {{}};
        for (std::size_t i = 0; i < key_count; i++)
            order[start[bucket_of[i]] + filled[bucket_of[i]]++] = i;
    }

    [[nodiscard]] constexpr std::size_t bucket_size(std::size_t bucket) const noexcept {
        return start[bucket + 1] - start[bucket];
    }
};

/**
 * @param keys                          Keys to check.
 * @return                              For each key, whether there are no equal keys before it.
 */
template<class Key, std::size_t key_count>
[[nodiscard]] constexpr std::array<bool, key_count> find_first_occurrences(const std::array<Key, key_count> &keys) {
    constexpr std::size_t size = perfect_hash_size(key_count);
    hash_buckets<size, enum_table_hash, Key, key_count> buckets(keys, 1);

    std::array<bool, key_count> result = {{}};
    for (std::size_t b = 0; b < size; b++) {
        for (std::size_t i = buckets.start[b]; i < buckets.start[b + 1]; i++) {
            bool first = true;
            for (std::size_t j = buckets.start[b]; j < i && first; j++)
                first = keys[buckets.order[j]] != keys[buckets.order[i]];
            result[buckets.order[i]] = first;
        }
    }
    return result;
}

/**
 * @param keys                          Keys to check.
 * @param[out] first                    Index of the first of two equal keys, if there are any.
 * @param[out] second                   Index of the second of two equal keys, if there are any.
 * @return                              Whether some of the keys are equal.
 */
template<class Key, std::size_t key_count>
[[nodiscard]] constexpr bool find_duplicate_keys(const std::array<Key, key_count> &keys, std::size_t *first, std::size_t *second) {
    std::array<bool, key_count> is_first = find_first_occurrences(keys);
    for (std::size_t i = 0; i < key_count; i++) {
        if (is_first[i])
            continue;

        for (std::size_t j = 0; j < i; j++) {
            if (keys[j] == keys[i]) {
                *first = j;
                *second = i;
                return true;
            }
        }
    }
    return false;
}

template<class Key, std::size_t key_count>
[[nodiscard]] constexpr bool has_duplicate_keys(const std::array<Key, key_count> &keys) {
    std::size_t first = 0;
    std::size_t second = 0;
    return find_duplicate_keys(keys, &first, &second);
}

/**
 * Perfect hash function for a fixed set of integer keys, built at compile time. Maps every key from the set into its
 * own slot in `[0, size)`. Keys that are not in the set also get mapped into some slot, so the caller has to check
 * what's actually stored there.
 *
 * It's a two-level scheme. The first hash picks a bucket, and each bucket has a small seed for the second hash, picked
 * so that all keys of the bucket land in free slots. A lookup is then two hashes and one load, with no branches.
 *
 * Keys are either integers, or string hashes, see `basic_enum_table_hash`.
 *
 * @tparam size                         Number of slots, and also the number of buckets. Must be a power of two.
 * @tparam Key                          Type of the keys, an unsigned integer. Lookups are the fastest when it's
 *                                      `Hash::hash_type`, or smaller.
 * @tparam Hash                         Hash functions to use.
 */
template<std::size_t size, class Key, class Hash = enum_table_hash>
struct perfect_hash {
    std::uint32_t first_seed = 0;
    std::array<std::uint8_t, size> seeds = {{}};

    [[nodiscard]] constexpr std::size_t slot(Key key) const noexcept {
        std::size_t bucket = static_cast<std::size_t>(Hash::mix(key, first_seed) & (size - 1));
        return static_cast<std::size_t>(Hash::mix(key, seeds[bucket]) & (size - 1));
    }
};

inline constexpr std::size_t max_perfect_hash_bucket_size = 32;

/**
 * Tries to build a perfect hash using the provided first-level seed.
 *
 * @param keys                          Keys to build the perfect hash for.
 * @param first_seed                    Seed for the first-level hash.
 * @param[out] result                   Resulting perfect hash. In unspecified state if the function fails.
 * @return                              Whether the perfect hash was built.
 */
template<std::size_t size, class Key, class Hash, std::size_t key_count>
[[nodiscard]] constexpr bool try_make_perfect_hash(const std::array<Key, key_count> &keys, std::uint32_t first_seed, perfect_hash<size, Key, Hash> *result) {
    *result = perfect_hash<size, Key, Hash>();
    result->first_seed = first_seed;

    hash_buckets<size, Hash, Key, key_count> buckets(keys, first_seed);
    if (buckets.max_bucket_size > max_perfect_hash_bucket_size)
        return false;

    // Place buckets one by one, largest first, as they are the hardest ones to place.
    std::array<bool, size> used = {{}};
    for (std::size_t bucket_size = buckets.max_bucket_size; bucket_size >= 1; bucket_size--) {
        for (std::size_t b = 0; b < size; b++) {
            if (buckets.bucket_size(b) != bucket_size)
                continue;

            bool placed = false;
            for (std::uint32_t seed = 1; seed < 256 && !placed; seed++) {
                std::array<std::size_t, max_perfect_hash_bucket_size> slots = {{}};
                bool ok = true;
                for (std::size_t j = 0; j < bucket_size && ok; j++) {
                    slots[j] = static_cast<std::size_t>(Hash::mix(keys[buckets.order[buckets.start[b] + j]], seed) & (size - 1));
                    ok = !used[slots[j]];
                    for (std::size_t k = 0; k < j && ok; k++)
                        ok = slots[k] != slots[j];
                }
                if (!ok)
                    continue;

                for (std::size_t j = 0; j < bucket_size; j++)
                    used[slots[j]] = true;
                result->seeds[b] = static_cast<std::uint8_t>(seed);
                placed = true;
            }
            if (!placed)
                return false;
        }
    }
    return true;
}

/**
 * Tries to build a perfect hash for the provided keys.
 *
 * @param keys                          Keys to build the perfect hash for. All keys must be different, and this is
 *                                      not checked.
 * @param[out] result                   Resulting perfect hash. In unspecified state if the function fails.
 * @return                              Whether the perfect hash was built.
 */
template<std::size_t size, class Key, class Hash, std::size_t key_count>
[[nodiscard]] constexpr bool try_make_perfect_hash(const std::array<Key, key_count> &keys, perfect_hash<size, Key, Hash> *result) {
    static_assert(std::has_single_bit(size) && size > key_count);

    // Each first-level seed gives a completely different distribution of keys into buckets, so if one doesn't work
    // out then we just try the next one. In practice the very first seed works.
    for (std::uint32_t first_seed = 1; first_seed <= 64; first_seed++)
        if (try_make_perfect_hash(keys, first_seed, result))
            return true;
    return false;
}

/**
 * Builds a perfect hash for the provided keys.
 *
 * @tparam size                         Number of slots, normally `perfect_hash_size(key_count)`.
 * @tparam Hash                         Hash functions to use.
 * @param keys                          Keys to build the perfect hash for. All keys must be different.
 */
template<std::size_t size, class Hash = enum_table_hash, class Key = std::uint64_t, std::size_t key_count = 0>
[[nodiscard]] constexpr perfect_hash<size, Key, Hash> make_perfect_hash(const std::array<Key, key_count> &keys) {
    // Two equal keys can never be separated, so we have to check for them upfront.
    if (has_duplicate_keys(keys))
        throw std::logic_error("Duplicate keys passed to make_perfect_hash");

    perfect_hash<size, Key, Hash> result;
    if (!try_make_perfect_hash(keys, &result))
        throw std::logic_error("Failed to build a perfect hash");
    return result;
}

} // namespace sn::detail
