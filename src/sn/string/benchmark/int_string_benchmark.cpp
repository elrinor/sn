#include <string>

#include <benchmark/benchmark.h> // NOLINT: Not a C system header.

#include "sn/string/enum_string.h"

template<class Tag>
static void benchmark_int_to_string(benchmark::State &state, Tag tag) { // NOLINT
    std::string tmp;
    std::size_t result = 0;
    for (auto _ : state) {
        result += sn::to_string(10, &tmp, nullptr, tag);
        result += sn::to_string(1000, &tmp, nullptr, tag);
        result += sn::to_string(100000, &tmp, nullptr, tag);
        result += sn::to_string(10000000, &tmp, nullptr, tag);
        result += sn::to_string(1000000000, &tmp, nullptr, tag);
    }
    benchmark::DoNotOptimize(tmp);
    benchmark::DoNotOptimize(result);
}

static void benchmark_int_to_string_base2(benchmark::State &state) { // NOLINT
    benchmark_int_to_string(state, tn::bin);
}

static void benchmark_int_to_string_base8(benchmark::State &state) { // NOLINT
    benchmark_int_to_string(state, tn::oct);
}

static void benchmark_int_to_string_base10(benchmark::State &state) { // NOLINT
    benchmark_int_to_string(state, tn::dec);
}

static void benchmark_int_to_string_base16(benchmark::State &state) { // NOLINT
    benchmark_int_to_string(state, tn::hex);
}

BENCHMARK(benchmark_int_to_string_base2);
BENCHMARK(benchmark_int_to_string_base8);
BENCHMARK(benchmark_int_to_string_base10);
BENCHMARK(benchmark_int_to_string_base16);
