#include <cstddef>
#include <cstdint>
#include <random>
#include <string>
#include <vector>

#include <benchmark/benchmark.h> // NOLINT: Not a C system header.

#include "sn/detail/enum_table/enum_table_hash.h"

//
// Benchmarks for the two implementations of enum table hash functions. The 64-bit one is used on 64-bit platforms,
// and the 32-bit one on 32-bit platforms, and these benchmarks are what backs this choice. The 32-bit one only wins on
// 32-bit platforms, so the numbers to look at are in the logs of 32-bit CI jobs.
//
// Each hash below depends on the previous one. In a table lookup the hash is on the critical path, so it's the latency
// of a hash that matters, and not how many of them can run in parallel.
//

constexpr std::size_t benchmark_input_size = 4096;

template<class Hash, class Key>
static void benchmark_enum_table_hash_mix(benchmark::State &state) { // NOLINT
    std::mt19937_64 rng(12345); // NOLINT: fixed seed is intended.
    std::vector<Key> keys;
    for (std::size_t i = 0; i < benchmark_input_size; i++)
        keys.push_back(static_cast<Key>(rng()));

    Key key = 0;
    std::size_t i = 0;
    for (auto _ : state) {
        key = static_cast<Key>(Hash::mix(key, 1) ^ keys[i]);
        i = (i + 1) % benchmark_input_size;
    }
    benchmark::DoNotOptimize(key);
}

template<class Hash, std::size_t size>
static void benchmark_enum_table_hash_string(benchmark::State &state) { // NOLINT
    std::mt19937 rng(12345); // NOLINT: fixed seed is intended.
    std::vector<std::string> strings;
    for (std::size_t i = 0; i < benchmark_input_size; i++) {
        std::string string;
        for (std::size_t j = 0; j < size; j++)
            string.push_back(static_cast<char>('a' + rng() % 26));
        strings.push_back(string);
    }

    typename Hash::hash_type seed = 0;
    std::size_t i = 0;
    for (auto _ : state) {
        seed = Hash::hash_string(strings[i].data(), strings[i].size(), false, seed);
        i = (i + 1) % benchmark_input_size;
    }
    benchmark::DoNotOptimize(seed);
}

// See enum_string_benchmark.cpp for why min time is set here.

// Tables hash machine words, and that's what the first two benchmarks are for. The only exception is an enum with
// values that are more than 2^32 apart on a 32-bit platform, and that's the third one.
BENCHMARK_TEMPLATE(benchmark_enum_table_hash_mix, sn::detail::enum_table_hash_32, std::uint32_t)->MinTime(0.1);
BENCHMARK_TEMPLATE(benchmark_enum_table_hash_mix, sn::detail::enum_table_hash_64, std::uint64_t)->MinTime(0.1);
BENCHMARK_TEMPLATE(benchmark_enum_table_hash_mix, sn::detail::enum_table_hash_32, std::uint64_t)->MinTime(0.1);

BENCHMARK_TEMPLATE(benchmark_enum_table_hash_string, sn::detail::enum_table_hash_32, 6)->MinTime(0.1);
BENCHMARK_TEMPLATE(benchmark_enum_table_hash_string, sn::detail::enum_table_hash_64, 6)->MinTime(0.1);
BENCHMARK_TEMPLATE(benchmark_enum_table_hash_string, sn::detail::enum_table_hash_32, 12)->MinTime(0.1);
BENCHMARK_TEMPLATE(benchmark_enum_table_hash_string, sn::detail::enum_table_hash_64, 12)->MinTime(0.1);
BENCHMARK_TEMPLATE(benchmark_enum_table_hash_string, sn::detail::enum_table_hash_32, 24)->MinTime(0.1);
BENCHMARK_TEMPLATE(benchmark_enum_table_hash_string, sn::detail::enum_table_hash_64, 24)->MinTime(0.1);
BENCHMARK_TEMPLATE(benchmark_enum_table_hash_string, sn::detail::enum_table_hash_32, 48)->MinTime(0.1);
BENCHMARK_TEMPLATE(benchmark_enum_table_hash_string, sn::detail::enum_table_hash_64, 48)->MinTime(0.1);
