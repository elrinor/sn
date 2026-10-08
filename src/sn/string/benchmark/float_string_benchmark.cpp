#include <string>

#include <benchmark/benchmark.h> // NOLINT: Not a C system header.

#include "sn/string/string.h"

static void benchmark_double_to_string(benchmark::State &state) { // NOLINT
    std::string tmp;
    for (auto _ : state) {
        sn::to_string(0.5, &tmp);
        sn::to_string(3.14159, &tmp);
        sn::to_string(123.456, &tmp);
        sn::to_string(1e10, &tmp);
        sn::to_string(-0.001, &tmp);
    }
    benchmark::DoNotOptimize(tmp);
}

static void benchmark_double_to_new_string(benchmark::State &state) { // NOLINT
    // Unlike the benchmark above, this one creates a new string every time, so it also measures allocations.
    for (auto _ : state) {
        benchmark::DoNotOptimize(sn::to_string(0.5));
        benchmark::DoNotOptimize(sn::to_string(3.14159));
        benchmark::DoNotOptimize(sn::to_string(123.456));
        benchmark::DoNotOptimize(sn::to_string(1e10));
        benchmark::DoNotOptimize(sn::to_string(-0.001));
    }
}

BENCHMARK(benchmark_double_to_string);
BENCHMARK(benchmark_double_to_new_string);
