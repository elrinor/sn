#pragma once

#include <bit> // For std::endian.
#include <cstddef>
#include <cstdint>
#include <cstring> // For std::memcpy.
#include <type_traits> // For std::conditional_t.

#if defined(_MSC_VER) && !defined(__SIZEOF_INT128__) && (defined(_M_X64) || defined(_M_ARM64))
#   include <intrin.h> // For _umul128 and __umulh.
#endif

#include "sn/detail/ascii/ascii_functions.h"

namespace sn::detail {

// Strings are hashed and compared a word at a time. Words are read in native byte order at run time, and are assembled
// as little-endian at compile time, so the two only agree on little-endian platforms.
static_assert(std::endian::native == std::endian::little, "sn enum tables only support little-endian platforms");

/**
 * Portable implementation of `multiply_fold`. Usable at compile time with compilers that don't have a 128-bit integer
 * type.
 */
[[nodiscard]] constexpr std::uint64_t multiply_fold_portable(std::uint64_t a, std::uint64_t b) noexcept {
    std::uint64_t a_lo = a & 0xFFFFFFFFu;
    std::uint64_t a_hi = a >> 32;
    std::uint64_t b_lo = b & 0xFFFFFFFFu;
    std::uint64_t b_hi = b >> 32;

    std::uint64_t lo_lo = a_lo * b_lo;
    std::uint64_t lo_hi = a_lo * b_hi;
    std::uint64_t hi_lo = a_hi * b_lo;
    std::uint64_t hi_hi = a_hi * b_hi;

    std::uint64_t mid = (lo_lo >> 32) + (lo_hi & 0xFFFFFFFFu) + (hi_lo & 0xFFFFFFFFu);
    std::uint64_t lo = (lo_lo & 0xFFFFFFFFu) | (mid << 32);
    std::uint64_t hi = hi_hi + (lo_hi >> 32) + (hi_lo >> 32) + (mid >> 32);
    return lo ^ hi;
}

/**
 * Multiplies two 64-bit numbers into a 128-bit product and xors its halves together. Unlike a regular 64-bit
 * multiplication, every bit of the result depends on every bit of the inputs, including the low bits of the result.
 *
 * This matters because hash tables reduce hashes by taking their low bits.
 */
[[nodiscard]] constexpr std::uint64_t multiply_fold(std::uint64_t a, std::uint64_t b) noexcept {
#if defined(__SIZEOF_INT128__)
    __uint128_t product = static_cast<__uint128_t>(a) * b;
    return static_cast<std::uint64_t>(product) ^ static_cast<std::uint64_t>(product >> 64);
#else
    if !consteval {
#   if defined(_MSC_VER) && defined(_M_X64)
        std::uint64_t hi = 0;
        std::uint64_t lo = _umul128(a, b, &hi);
        return lo ^ hi;
#   elif defined(_MSC_VER) && defined(_M_ARM64)
        return (a * b) ^ __umulh(a, b);
#   endif
    }
    return multiply_fold_portable(a, b);
#endif
}

/**
 * Same as `multiply_fold`, but for 32-bit numbers.
 */
[[nodiscard]] constexpr std::uint32_t multiply_fold(std::uint32_t a, std::uint32_t b) noexcept {
    std::uint64_t product = static_cast<std::uint64_t>(a) * b;
    return static_cast<std::uint32_t>(product) ^ static_cast<std::uint32_t>(product >> 32);
}

[[nodiscard]] constexpr std::uint64_t load_u64(const char *p) noexcept {
    if consteval {
        std::uint64_t result = 0;
        for (std::size_t i = 0; i < 8; i++)
            result |= static_cast<std::uint64_t>(static_cast<unsigned char>(p[i])) << (8 * i);
        return result;
    } else {
        std::uint64_t result;
        std::memcpy(&result, p, sizeof(result));
        return result;
    }
}

[[nodiscard]] constexpr std::uint32_t load_u32(const char *p) noexcept {
    if consteval {
        std::uint32_t result = 0;
        for (std::size_t i = 0; i < 4; i++)
            result |= static_cast<std::uint32_t>(static_cast<unsigned char>(p[i])) << (8 * i);
        return result;
    } else {
        std::uint32_t result;
        std::memcpy(&result, p, sizeof(result));
        return result;
    }
}

/**
 * Packs a string of 1 to 3 chars into a word. The packing is injective for a given size.
 */
[[nodiscard]] constexpr std::uint32_t load_u24(const char *p, std::size_t size) noexcept {
    return static_cast<std::uint32_t>(static_cast<unsigned char>(p[0])) |
           (static_cast<std::uint32_t>(static_cast<unsigned char>(p[size >> 1])) << 8) |
           (static_cast<std::uint32_t>(static_cast<unsigned char>(p[size - 1])) << 16);
}

/**
 * Converts 8 chars packed into a 64-bit word to lowercase, all at once. Only touches ascii uppercase letters.
 */
[[nodiscard]] constexpr std::uint64_t to_lower_ascii_word(std::uint64_t word) noexcept {
    std::uint64_t low7 = word & 0x7F7F7F7F7F7F7F7Fu;
    std::uint64_t ge_a = low7 + 0x3F3F3F3F3F3F3F3Fu; // High bit of a byte is set if the byte is >= 'A'.
    std::uint64_t gt_z = low7 + 0x2525252525252525u; // High bit of a byte is set if the byte is > 'Z'.
    std::uint64_t is_upper = ge_a & ~gt_z & ~word & 0x8080808080808080u;
    return word | (is_upper >> 2); // Turns 0x80 into 0x20, which is the difference between 'a' and 'A'.
}

/**
 * Same as above, but for 4 chars packed into a 32-bit word.
 */
[[nodiscard]] constexpr std::uint32_t to_lower_ascii_word(std::uint32_t word) noexcept {
    std::uint32_t low7 = word & 0x7F7F7F7Fu;
    std::uint32_t ge_a = low7 + 0x3F3F3F3Fu;
    std::uint32_t gt_z = low7 + 0x25252525u;
    std::uint32_t is_upper = ge_a & ~gt_z & ~word & 0x80808080u;
    return word | (is_upper >> 2);
}

/**
 * Hash functions for enum tables. There are two implementations, for 64-bit and for 32-bit platforms, that only differ
 * in the size of the machine word that they use. Both work everywhere, the one that's natural for the platform is
 * just faster.
 *
 * `hash_string` hashes a string. It doesn't read outside the string, and it processes the string in words and not
 * char by char. The result is to be passed into `mix`.
 *
 * `mix` mixes a seed into a string hash, or into an integer key, producing the final hash. Different seeds produce
 * unrelated hashes, which is what `perfect_hash` relies on. This way a string is only traversed once even if it's
 * then hashed with several seeds.
 *
 * @tparam Word                         Machine word to use, `std::uint64_t` or `std::uint32_t`.
 */
template<class Word>
struct basic_enum_table_hash {
    using hash_type = Word;

    /**
     * @param key                       Integer key, or a hash returned by `hash_string`.
     * @param seed                      Seed.
     * @return                          Hash of the key.
     */
    [[nodiscard]] static constexpr hash_type mix(std::uint64_t key, std::uint64_t seed) noexcept {
        if constexpr (sizeof(Word) == 8) {
            return multiply_fold(key ^ (seed * 0x9E3779B97F4A7C15u), static_cast<std::uint64_t>(0xC2B2AE3D27D4EB4Fu));
        } else {
            std::uint32_t lo = static_cast<std::uint32_t>(key);
            std::uint32_t hi = static_cast<std::uint32_t>(key >> 32);
            std::uint32_t mixed_seed = static_cast<std::uint32_t>(seed) * 0x9E3779B9u;
            return multiply_fold(multiply_fold(lo ^ mixed_seed, static_cast<std::uint32_t>(0x85EBCA6Bu)) ^ hi ^ 0x27D4EB2Fu, static_cast<std::uint32_t>(0xC2B2AE35u));
        }
    }

    /**
     * @param p                         String data.
     * @param size                      String size.
     * @param fold_case                 Whether ascii uppercase letters should hash the same as lowercase ones.
     * @param seed                      Seed. Only needed to resolve hash collisions between different strings, which
     *                                  are realistic only for 32-bit hashes.
     * @return                          Hash of the string, to be passed into `mix`.
     */
    [[nodiscard]] static constexpr std::uint64_t hash_string(const char *p, std::size_t size, bool fold_case, std::uint64_t seed) noexcept {
        constexpr Word k0 = static_cast<Word>(0x9E3779B97F4A7C15u);
        constexpr Word k1 = static_cast<Word>(0xC2B2AE3D27D4EB4Fu);
        constexpr Word k2 = static_cast<Word>(0xA0761D6478BD642Fu);
        constexpr Word k3 = static_cast<Word>(0xE7037ED1A0B428DBu);
        constexpr Word k4 = static_cast<Word>(0x8EBC6AF09C88C6E3u);
        constexpr std::size_t word_size = sizeof(Word);

        // Strings that fit into two words are loaded with overlap, e.g. for a 64-bit word a 10-char string is loaded
        // as chars [0, 8) and [2, 10). That's OK because the size is hashed in too, and for a given size this is
        // injective. Longer strings are hashed two words at a time, and the last two words are again loaded with
        // overlap.
        Word a = 0;
        Word b = 0;
        Word state = k0 + static_cast<Word>(seed) * k4;
        if (size <= 2 * word_size) {
            if (size >= word_size) {
                a = load_word(p);
                b = load_word(p + size - word_size);
            } else if (size >= 4) { // Only possible for 64-bit words.
                a = static_cast<Word>(load_u32(p) | (static_cast<std::uint64_t>(load_u32(p + size - 4)) << 32));
            } else if (size > 0) {
                a = load_u24(p, size);
            }
        } else {
            for (std::size_t i = 0; i + 2 * word_size < size; i += 2 * word_size) {
                Word w0 = load_word(p + i);
                Word w1 = load_word(p + i + word_size);
                if (fold_case) {
                    w0 = to_lower_ascii_word(w0);
                    w1 = to_lower_ascii_word(w1);
                }
                state = multiply_fold(static_cast<Word>(w0 ^ k1), static_cast<Word>(w1 ^ state));
            }
            a = load_word(p + size - 2 * word_size);
            b = load_word(p + size - word_size);
        }
        if (fold_case) {
            a = to_lower_ascii_word(a);
            b = to_lower_ascii_word(b);
        }
        Word result = multiply_fold(static_cast<Word>(a ^ k1), static_cast<Word>(b ^ state));
        return multiply_fold(static_cast<Word>(result ^ k2), static_cast<Word>(static_cast<Word>(size) ^ k3));
    }

private:
    [[nodiscard]] static constexpr Word load_word(const char *p) noexcept {
        if constexpr (sizeof(Word) == 8) {
            return load_u64(p);
        } else {
            return load_u32(p);
        }
    }
};

using enum_table_hash_64 = basic_enum_table_hash<std::uint64_t>;
using enum_table_hash_32 = basic_enum_table_hash<std::uint32_t>;

/** Hash functions that enum tables use on this platform. */
using enum_table_hash = std::conditional_t<(sizeof(std::size_t) >= 8), enum_table_hash_64, enum_table_hash_32>;

/**
 * Compares two strings of the same size, 8 chars at a time. Never reads outside the strings.
 *
 * @param input                         String to compare.
 * @param stored                        String to compare to. Must be lowercase if `fold_case` is `true`.
 * @param size                          Size of both strings.
 * @param fold_case                     Whether to ignore case of ascii letters in `input`.
 */
[[nodiscard]] constexpr bool equal_strings(const char *input, const char *stored, std::size_t size, bool fold_case) noexcept {
    if (size >= 8) {
        for (std::size_t i = 0; i + 8 < size; i += 8) {
            std::uint64_t word = load_u64(input + i);
            if ((fold_case ? to_lower_ascii_word(word) : word) != load_u64(stored + i))
                return false;
        }
        std::uint64_t word = load_u64(input + size - 8);
        return (fold_case ? to_lower_ascii_word(word) : word) == load_u64(stored + size - 8);
    }
    if (size >= 4) {
        std::uint32_t word0 = load_u32(input);
        std::uint32_t word1 = load_u32(input + size - 4);
        if (fold_case) {
            word0 = to_lower_ascii_word(word0);
            word1 = to_lower_ascii_word(word1);
        }
        return word0 == load_u32(stored) && word1 == load_u32(stored + size - 4);
    }
    for (std::size_t i = 0; i < size; i++)
        if ((fold_case ? to_lower_ascii(input[i]) : input[i]) != stored[i])
            return false;
    return true;
}

} // namespace sn::detail
