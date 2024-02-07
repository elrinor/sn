#include <benchmark/benchmark.h>

#include "enum_string.h"

#if defined(__GNUC__)
#   define BENCHMARK_NOINLINE __attribute__((noinline))
#elif defined(_MSC_VER)
#   define BENCHMARK_NOINLINE __declspec(noinline)
#else
#   define BENCHMARK_NOINLINE
#endif

enum class BenchEnum {
    VALUE_1,
    VALUE_2,
    VALUE_3,
    VALUE_4,
    VALUE_5,
};
using enum BenchEnum;

SN_DEFINE_ENUM_REFLECTION(BenchEnum, ({
    {VALUE_1, "aaaaa"},
    {VALUE_2, "bbbbb"},
    {VALUE_3, "ccccc"},
    {VALUE_4, "ddddd"},
    {VALUE_5, "eeeee"},
}))
SN_DEFINE_ENUM_STRING_FUNCTIONS(BenchEnum, sn::case_insensitive)

BENCHMARK_NOINLINE static bool do_try_from_string(std::string_view src, BenchEnum *dst) {
    return sn::try_from_string(src, dst);
}

static void benchmark_string_to_enum(benchmark::State &state) { // NOLINT: Google linter complains about the API in Google's own benchmark lib. Doh.
    std::size_t result = 0;
    for (auto _ : state) {
        BenchEnum value;
        result += do_try_from_string("dDdDd", &value);
        result += do_try_from_string("aaaaa", &value);
        result += do_try_from_string("ccccC", &value);
        result += do_try_from_string("eeeee", &value);
        result += do_try_from_string("aAAaa", &value);
        result += do_try_from_string("bBbbB", &value);
        result += do_try_from_string("aAAAA", &value);
        result += do_try_from_string("aaAaa", &value);
        result += do_try_from_string("bbbBB", &value);
        result += do_try_from_string("ddddd", &value);
    }
    benchmark::DoNotOptimize(result);
}

BENCHMARK(benchmark_string_to_enum);
