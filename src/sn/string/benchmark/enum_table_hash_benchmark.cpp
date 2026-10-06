#include <cstddef>
#include <cstdint>
#include <random>
#include <string>
#include <vector>

#include <benchmark/benchmark.h> // NOLINT: Not a C system header.

#include "sn/detail/enum_table/enum_table_hash.h"

//
// Benchmarks for the two implementations of enum table hash functions. The 64-bit one is used on 64-bit platforms,
// and the 32-bit one on 32-bit platforms, and these benchmarks are what backs this choice.
//

constexpr std::size_t benchmark_input_size = 4096;

template<class Hash>
static void benchmark_enum_table_hash_mix(benchmark::State &state) { // NOLINT
    std::mt19937_64 rng(12345); // NOLINT: fixed seed is intended.
    std::vector<std::uint64_t> keys;
    for (std::size_t i = 0; i < benchmark_input_size; i++)
        keys.push_back(rng());

    std::uint64_t result = 0;
    std::size_t i = 0;
    for (auto _ : state) {
        result += Hash::mix(keys[i], 1);
        i = (i + 1) % benchmark_input_size;
    }
    benchmark::DoNotOptimize(result);
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

    std::uint64_t result = 0;
    std::size_t i = 0;
    for (auto _ : state) {
        result += Hash::hash_string(strings[i].data(), strings[i].size(), false, 0);
        i = (i + 1) % benchmark_input_size;
    }
    benchmark::DoNotOptimize(result);
}

BENCHMARK_TEMPLATE(benchmark_enum_table_hash_mix, sn::detail::enum_table_hash_32);
BENCHMARK_TEMPLATE(benchmark_enum_table_hash_mix, sn::detail::enum_table_hash_64);

BENCHMARK_TEMPLATE(benchmark_enum_table_hash_string, sn::detail::enum_table_hash_32, 6);
BENCHMARK_TEMPLATE(benchmark_enum_table_hash_string, sn::detail::enum_table_hash_64, 6);
BENCHMARK_TEMPLATE(benchmark_enum_table_hash_string, sn::detail::enum_table_hash_32, 12);
BENCHMARK_TEMPLATE(benchmark_enum_table_hash_string, sn::detail::enum_table_hash_64, 12);
BENCHMARK_TEMPLATE(benchmark_enum_table_hash_string, sn::detail::enum_table_hash_32, 24);
BENCHMARK_TEMPLATE(benchmark_enum_table_hash_string, sn::detail::enum_table_hash_64, 24);
BENCHMARK_TEMPLATE(benchmark_enum_table_hash_string, sn::detail::enum_table_hash_32, 48);
BENCHMARK_TEMPLATE(benchmark_enum_table_hash_string, sn::detail::enum_table_hash_64, 48);
