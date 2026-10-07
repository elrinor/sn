#include <string>

#include <benchmark/benchmark.h> // NOLINT: Not a C system header.

#include "sn/string/string.h"

static void benchmark_double_to_string(benchmark::State &state) {
    std::string tmp;
    std::size_t result = 0;
    for (auto _ : state) {
        result += sn::to_string(0.5, &tmp, nullptr);
        result += sn::to_string(3.14159, &tmp, nullptr);
        result += sn::to_string(123.456, &tmp, nullptr);
        result += sn::to_string(1e10, &tmp, nullptr);
        result += sn::to_string(-0.001, &tmp, nullptr);
    }
    benchmark::DoNotOptimize(tmp);
    benchmark::DoNotOptimize(result);
}

static void benchmark_double_to_new_string(benchmark::State &state) {
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
