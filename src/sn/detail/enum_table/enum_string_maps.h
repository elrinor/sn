#pragma once

#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <stdexcept> // For std::logic_error.
#include <string>
#include <string_view>
#include <type_traits>
#include <utility> // For std::pair.

#include "sn/core/globals.h"
#include "sn/detail/ascii/ascii_functions.h"

#include "enum_table_hash.h"
#include "enum_type_traits.h"
#include "perfect_hash.h"

namespace sn::detail {

/**
 * Maximal number of slots in a flat enum table. Tables are built at compile time, and compilers limit the number of
 * steps in a single constant evaluation. With default limits a flat table of 2^16 slots still builds everywhere, and
 * one of 2^17 slots already doesn't on clang. In practice this only matters for `sn::flat_enum_table` being requested
 * explicitly for an enum with a huge range of values.
 */
inline constexpr std::uint64_t max_flat_enum_table_size = 1u << 16;

/**
 * Everything that's needed to pick types for the lookup tables of an enum. Computed from an enum reflection by
 * `make_enum_table_spec`, and then used as a template parameter.
 *
 * Enum values are type-erased into `std::uint64_t` here and in the tables.
 */
struct enum_table_spec {
    /** Whether string-to-enum conversions should ignore case of ascii letters. */
    bool fold_case = false;

    /** Whether the enum-to-string table is a flat one, otherwise it's a hash table. */
    bool flat = true;

    /** Whether an empty string is listed in the reflection. Empty strings are not stored in the tables. */
    bool has_empty_string = false;

    /** Whether the value that an empty string is listed for should be converted back into an empty string. */
    bool empty_string_is_primary = false;

    /** Value that an empty string is listed for. */
    std::uint64_t empty_string_value = 0;

    /** Smallest value in the reflection. Smallest in terms of the original type, i.e. `-1 < 0`. */
    std::uint64_t min_value = 0;

    /** Difference between the largest and the smallest values in the reflection. */
    std::uint64_t max_delta = 0;

    /** Number of strings in the enum-to-string table, this is the number of different values in the reflection. */
    std::size_t to_string_count = 0;
    std::size_t to_string_slots = 0;
    std::size_t to_string_data_size = 0;

    /** Number of strings in the string-to-enum table, this is the number of elements in the reflection. */
    std::size_t from_string_count = 0;
    std::size_t from_string_slots = 1;
    std::size_t from_string_data_size = 0;
};

/**
 * @param reflection                    Enum reflection.
 * @return                              For each element of the reflection, whether it's the first one that's listed
 *                                      for its value. Strings of such elements are the ones that enum-to-string
 *                                      conversions produce, other strings are only accepted as input.
 */
template<class T, std::size_t size>
[[nodiscard]] constexpr std::array<bool, size> find_primary_strings(const std::array<std::pair<T, std::string_view>, size> &reflection) {
    std::array<std::uint64_t, size> values = {{}};
    for (std::size_t i = 0; i < size; i++)
        values[i] = static_cast<std::uint64_t>(reflection[i].first);
    return find_first_occurrences(values);
}

/**
 * @param reflection                    Enum reflection, as returned from `sn::reflect_enum`.
 * @param options                       Enum table options.
 * @return                              Spec for the lookup tables for the provided reflection.
 */
template<class T, std::size_t size>
[[nodiscard]] constexpr enum_table_spec make_enum_table_spec(const std::array<std::pair<T, std::string_view>, size> &reflection, enum_table_options options) {
    enum_table_spec result;
    result.fold_case = options.mode == case_insensitive;
    if (size == 0)
        return result;

    // Min and max are computed in terms of the original type so that the range of values is as narrow as possible.
    // E.g. for values -1 and 1 this gives us a range of 3 values instead of 2^64 values.
    using value_type = underlying_type_ex_t<T>;
    value_type min = static_cast<value_type>(reflection[0].first);
    value_type max = min;

    std::array<bool, size> is_primary = find_primary_strings(reflection);
    for (std::size_t i = 0; i < size; i++) {
        value_type value = static_cast<value_type>(reflection[i].first);
        min = value < min ? value : min;
        max = value > max ? value : max;

        std::string_view string = reflection[i].second;
        if (string.empty()) {
            if (result.has_empty_string)
                throw std::logic_error("An empty string is listed more than once in an enum reflection");
            result.has_empty_string = true;
            result.empty_string_is_primary = is_primary[i];
            result.empty_string_value = static_cast<std::uint64_t>(reflection[i].first);
            continue;
        }

        result.from_string_count++;
        result.from_string_data_size += string.size();
        if (is_primary[i]) {
            result.to_string_count++;
            result.to_string_data_size += string.size();
        }
    }
    result.min_value = static_cast<std::uint64_t>(min);
    result.max_delta = static_cast<std::uint64_t>(max) - static_cast<std::uint64_t>(min);

    // A flat table is used if it's small in absolute terms, or if it's at most 4x larger than the number of strings
    // that it stores. A slot in a flat table is 1-2 bytes for most enums.
    bool can_be_flat = result.max_delta < max_flat_enum_table_size;
    if (options.kind == auto_enum_table) {
        result.flat = result.max_delta < 256 || (can_be_flat && result.max_delta < 4 * static_cast<std::uint64_t>(result.to_string_count));
    } else if (options.kind == flat_enum_table) {
        if (!can_be_flat)
            throw std::logic_error("Range of enum values is too large for sn::flat_enum_table");
        result.flat = true;
    } else {
        result.flat = false;
    }

    result.to_string_slots = result.flat ? static_cast<std::size_t>(result.max_delta + 1) : perfect_hash_size(result.to_string_count);
    result.from_string_slots = perfect_hash_size(result.from_string_count);
    return result;
}

/**
 * @tparam count                        Number of strings in the enum-to-string table, `enum_table_spec::to_string_count`.
 * @param reflection                    Enum reflection.
 * @return                              Type-erased elements of the enum-to-string table.
 */
template<std::size_t count, class T, std::size_t size>
[[nodiscard]] constexpr std::array<std::pair<std::uint64_t, std::string_view>, count>
collect_to_string_pairs(const std::array<std::pair<T, std::string_view>, size> &reflection) {
    std::array<std::pair<std::uint64_t, std::string_view>, count> result = {{}};
    std::array<bool, size> is_primary = find_primary_strings(reflection);
    std::size_t pos = 0;
    for (std::size_t i = 0; i < size; i++)
        if (is_primary[i] && !reflection[i].second.empty())
            result[pos++] = {static_cast<std::uint64_t>(reflection[i].first), reflection[i].second};
    return result;
}

/**
 * @tparam count                        Number of strings in the string-to-enum table, `enum_table_spec::from_string_count`.
 * @param reflection                    Enum reflection.
 * @return                              Type-erased elements of the string-to-enum table.
 */
template<std::size_t count, class T, std::size_t size>
[[nodiscard]] constexpr std::array<std::pair<std::string_view, std::uint64_t>, count>
collect_from_string_pairs(const std::array<std::pair<T, std::string_view>, size> &reflection) {
    std::array<std::pair<std::string_view, std::uint64_t>, count> result = {{}};
    std::size_t pos = 0;
    for (std::size_t i = 0; i < size; i++)
        if (!reflection[i].second.empty())
            result[pos++] = {reflection[i].second, static_cast<std::uint64_t>(reflection[i].first)};
    return result;
}

template<std::uint64_t max_value>
using smallest_uint_t =
    std::conditional_t<(max_value < 0x100u), std::uint8_t,
    std::conditional_t<(max_value < 0x10000u), std::uint16_t,
    std::conditional_t<(max_value <= 0xFFFFFFFFu), std::uint32_t, std::uint64_t>>>;

/**
 * Hides a string size from the optimizer.
 *
 * Offsets in `enum_table_strings` are narrow, and so a string size that's computed from them is known to be small.
 * When GCC for x86 knows that a `memcpy` is at most several KB in size, it expands it inline into a `rep movs`
 * instruction, and that one takes tens of cycles just to start. Copying a string out of an enum table then gets
 * several times slower than with a call to `memcpy`, and this is why we're not letting GCC know.
 *
 * @param size                          String size.
 * @return                              The same string size.
 */
[[nodiscard]] constexpr std::size_t hide_string_size(std::size_t size) noexcept {
#if defined(__GNUC__) && !defined(__clang__) && (defined(__i386__) || defined(__x86_64__))
    if !consteval {
        __asm__("" : "+r"(size));
    }
#endif
    return size;
}

/**
 * Strings of an enum table, concatenated in slot order. There is one offset per slot, and a string ends where the
 * string of the next slot begins, so an empty slot is the same thing as an empty string.
 *
 * @tparam slot_count                   Number of slots.
 * @tparam data_size                    Total size of all strings.
 */
template<std::size_t slot_count, std::size_t data_size>
class enum_table_strings {
public:
    constexpr enum_table_strings() = default;

    /**
     * @param strings                   Strings to store.
     * @param slots                     Slot for each of the strings.
     * @param to_lower                  Whether to convert the strings to lowercase.
     */
    template<std::size_t count>
    constexpr enum_table_strings(const std::array<std::string_view, count> &strings, const std::array<std::size_t, count> &slots, bool to_lower) {
        std::array<std::size_t, slot_count> sizes = {{}};
        for (std::size_t i = 0; i < count; i++)
            sizes[slots[i]] = strings[i].size();

        std::size_t offset = 0;
        for (std::size_t slot = 0; slot < slot_count; slot++) {
            _offsets[slot] = static_cast<offset_type>(offset);
            offset += sizes[slot];
        }
        _offsets[slot_count] = static_cast<offset_type>(offset);

        for (std::size_t i = 0; i < count; i++)
            for (std::size_t j = 0; j < strings[i].size(); j++)
                _data[_offsets[slots[i]] + j] = to_lower ? to_lower_ascii(strings[i][j]) : strings[i][j];
    }

    [[nodiscard]] constexpr std::string_view operator[](std::size_t slot) const noexcept {
        std::size_t offset = _offsets[slot];
        return {_data.data() + offset, hide_string_size(_offsets[slot + 1] - offset)};
    }

private:
    using offset_type = smallest_uint_t<data_size>;

    std::array<offset_type, slot_count + 1> _offsets = {{}};
    std::array<char, data_size> _data = {{}};
};

/**
 * Enum-to-string map that's an array of strings indexed by `value - min_value`.
 *
 * @tparam slot_count                   Number of slots, this is the size of the range of enum values.
 * @tparam data_size                    Total size of all strings.
 */
template<std::size_t slot_count, std::size_t data_size>
class flat_enum_string_map {
public:
    /**
     * @param pairs                     Values and their strings. All values must be different, and strings must not
     *                                  be empty.
     * @param min_value                 Smallest value.
     */
    template<std::size_t count>
    constexpr flat_enum_string_map(const std::array<std::pair<std::uint64_t, std::string_view>, count> &pairs,
                                   std::uint64_t min_value) : _min_value(min_value) {
        std::array<std::string_view, count> strings = {{}};
        std::array<std::size_t, count> slots = {{}};
        for (std::size_t i = 0; i < count; i++) {
            strings[i] = pairs[i].second;
            slots[i] = static_cast<std::size_t>(pairs[i].first - min_value);
        }
        _strings = enum_table_strings<slot_count, data_size>(strings, slots, false);
    }

    [[nodiscard]] constexpr bool find(std::uint64_t value, std::string_view *result) const noexcept {
        std::uint64_t slot = value - _min_value; // Wraps around for values below min, which then fail the check below.
        if (slot >= slot_count)
            return false;

        std::string_view string = _strings[static_cast<std::size_t>(slot)];
        if (string.empty())
            return false; // No such value.

        *result = string;
        return true;
    }

private:
    std::uint64_t _min_value;
    enum_table_strings<slot_count, data_size> _strings;
};

/**
 * Enum-to-string map that's a perfect hash table.
 *
 * @tparam slot_count                   Number of slots, `perfect_hash_size` of the number of values.
 * @tparam data_size                    Total size of all strings.
 * @tparam Key                          Type to store `value - min_value` in.
 * @tparam Hash                         Hash functions to use.
 */
template<std::size_t slot_count, std::size_t data_size, class Key, class Hash = enum_table_hash>
class hashed_enum_string_map {
public:
    /**
     * @param pairs                     Values and their strings. All values must be different, and strings must not
     *                                  be empty.
     * @param min_value                 Smallest value.
     */
    template<std::size_t count>
    constexpr hashed_enum_string_map(const std::array<std::pair<std::uint64_t, std::string_view>, count> &pairs,
                                     std::uint64_t min_value) : _min_value(min_value) {
        std::array<hash_key, count> keys = {{}};
        for (std::size_t i = 0; i < count; i++)
            keys[i] = static_cast<hash_key>(pairs[i].first);
        _hash = make_perfect_hash<slot_count, Hash>(keys);

        std::array<std::string_view, count> strings = {{}};
        std::array<std::size_t, count> slots = {{}};
        for (std::size_t i = 0; i < count; i++) {
            strings[i] = pairs[i].second;
            slots[i] = _hash.slot(keys[i]);
            _keys[slots[i]] = static_cast<Key>(pairs[i].first - min_value);
        }
        _strings = enum_table_strings<slot_count, data_size>(strings, slots, false);
    }

    [[nodiscard]] constexpr bool find(std::uint64_t value, std::string_view *result) const noexcept {
        std::size_t slot = _hash.slot(static_cast<hash_key>(value));
        std::string_view string = _strings[slot];

        // The stored key is widened for the comparison, and not the other way around. Values outside the range of
        // this table have a delta that doesn't fit into `Key`, and so can't compare equal to anything.
        if (string.empty() || static_cast<std::uint64_t>(_keys[slot]) != value - _min_value)
            return false; // Either the slot is empty, or it's for another value.

        *result = string;
        return true;
    }

private:
    // Hashing a machine word is cheaper than hashing a 64-bit value on a 32-bit platform, so we hash values as machine
    // words when we can. If `Key` fits into a machine word, then so does the difference between any two values of
    // this map. Truncating the values to a machine word then still leaves them all different, and this is all that
    // a perfect hash needs. Values that are not in the map are rejected in `find` in any case.
    using hash_key = std::conditional_t<(sizeof(Key) <= sizeof(typename Hash::hash_type)), typename Hash::hash_type, std::uint64_t>;

    std::uint64_t _min_value;
    perfect_hash<slot_count, hash_key, Hash> _hash;
    std::array<Key, slot_count> _keys = {{}};
    enum_table_strings<slot_count, data_size> _strings;
};

/**
 * String-to-enum map, a perfect hash table.
 *
 * @tparam slot_count                   Number of slots, `perfect_hash_size` of the number of strings.
 * @tparam data_size                    Total size of all strings.
 * @tparam Value                        Type to store `value - min_value` in.
 * @tparam fold_case                    Whether lookups should ignore case of ascii letters.
 * @tparam Hash                         Hash functions to use.
 */
template<std::size_t slot_count, std::size_t data_size, class Value, bool fold_case, class Hash = enum_table_hash>
class hashed_string_enum_map {
public:
    /**
     * @param pairs                     Strings and their values. All strings must be different, also after
     *                                  conversion to lowercase if `fold_case` is `true`, and must not be empty.
     * @param min_value                 Smallest value.
     */
    template<std::size_t count>
    constexpr hashed_string_enum_map(const std::array<std::pair<std::string_view, std::uint64_t>, count> &pairs,
                                     std::uint64_t min_value) : _min_value(min_value) {
        // Different strings can have the same hash. That's only a theoretical possibility for 64-bit hashes, but it
        // does happen for 32-bit ones if the enum is large. When it does, we just try another seed.
        std::array<typename Hash::hash_type, count> hashes = {{}};
        bool built = false;
        for (std::uint32_t seed = 0; seed < 16 && !built; seed++) {
            for (std::size_t i = 0; i < count; i++)
                hashes[i] = Hash::hash_string(pairs[i].first.data(), pairs[i].first.size(), fold_case, seed);
            if (has_hash_collisions(pairs, hashes))
                continue;

            _seed = seed;
            built = try_make_perfect_hash(hashes, &_hash);
        }
        if (!built)
            throw std::logic_error("Failed to build a perfect hash for enum strings");

        std::array<std::string_view, count> strings = {{}};
        std::array<std::size_t, count> slots = {{}};
        for (std::size_t i = 0; i < count; i++) {
            strings[i] = pairs[i].first;
            slots[i] = _hash.slot(hashes[i]);
            _values[slots[i]] = static_cast<Value>(pairs[i].second - min_value);
        }
        _strings = enum_table_strings<slot_count, data_size>(strings, slots, fold_case);
    }

    /**
     * @param string                    String to look up, must not be empty.
     * @param[out] result               Value for the string.
     * @return                          Whether the string was found.
     */
    [[nodiscard]] constexpr bool find(std::string_view string, std::uint64_t *result) const noexcept {
        assert(!string.empty()); // Would match an empty slot.

        std::size_t slot = _hash.slot(Hash::hash_string(string.data(), string.size(), fold_case, _seed));
        std::string_view stored = _strings[slot];
        if (stored.size() != string.size() || !equal_strings(string.data(), stored.data(), string.size(), fold_case))
            return false;

        *result = _min_value + _values[slot];
        return true;
    }

private:
    /**
     * @param pairs                     Strings and their values.
     * @param hashes                    Hashes of the strings.
     * @return                          Whether some of the hashes are equal. Throws if that's because the strings
     *                                  themselves are equal.
     */
    template<std::size_t count>
    [[nodiscard]] static constexpr bool has_hash_collisions(const std::array<std::pair<std::string_view, std::uint64_t>, count> &pairs,
                                                            const std::array<typename Hash::hash_type, count> &hashes) {
        std::size_t first = 0;
        std::size_t second = 0;
        if (!find_duplicate_keys(hashes, &first, &second))
            return false;

        std::string_view l = pairs[first].first;
        std::string_view r = pairs[second].first;
        bool equal = l.size() == r.size();
        for (std::size_t i = 0; i < l.size() && equal; i++)
            equal = fold_case ? to_lower_ascii(l[i]) == to_lower_ascii(r[i]) : l[i] == r[i];
        if (equal)
            throw std::logic_error("A string is listed more than once in an enum reflection, possibly in different case");
        return true;
    }

private:
    std::uint64_t _min_value;
    std::uint32_t _seed = 0;
    perfect_hash<slot_count, typename Hash::hash_type, Hash> _hash;
    std::array<Value, slot_count> _values = {{}};
    enum_table_strings<slot_count, data_size> _strings;
};

template<enum_table_spec spec, class Hash = enum_table_hash>
using enum_to_string_map = std::conditional_t<
    spec.flat,
    flat_enum_string_map<spec.to_string_slots, spec.to_string_data_size>,
    hashed_enum_string_map<spec.to_string_slots, spec.to_string_data_size, smallest_uint_t<spec.max_delta>, Hash>
>;

template<enum_table_spec spec, class Hash = enum_table_hash>
using string_to_enum_map = hashed_string_enum_map<spec.from_string_slots, spec.from_string_data_size, smallest_uint_t<spec.max_delta>, spec.fold_case, Hash>;

} // namespace sn::detail
