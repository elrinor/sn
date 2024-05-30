#include <string>

#include <benchmark/benchmark.h> // NOLINT: Not a C system header.

#include "sn/string/enum_string.h"

template<class Tag>
static void benchmark_int_to_string(benchmark::State &state, Tag tag) { // NOLINT
    std::string tmp;
    for (auto _ : state) {
        sn::to_string(10, &tmp, tag);
        sn::to_string(1000, &tmp, tag);
        sn::to_string(100000, &tmp, tag);
        sn::to_string(10000000, &tmp, tag);
        sn::to_string(1000000000, &tmp, tag);
    }
    benchmark::DoNotOptimize(tmp);
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
