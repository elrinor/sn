#include <array>
#include <cstddef>
#include <random>
#include <string>
#include <string_view>
#include <type_traits> // For std::type_identity.
#include <utility> // For std::pair.
#include <vector>

#include <benchmark/benchmark.h> // NOLINT: Not a C system header.

#include "sn/core/tag.h"
#include "sn/string/enum_string.h"

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


//
// Benchmarks for tables of different sizes and kinds. It's easier to generate reflections for these than to write
// them down, so we're using tagged reflections for int here.
//

template<std::size_t size>
struct generated_strings {
    std::array<std::array<char, 16>, size> data = {{}};
    std::array<std::size_t, size> sizes = {{}};
};

/**
 * @return                              Strings "value_0", "value_1", etc.
 */
template<std::size_t size>
constexpr generated_strings<size> generate_strings() {
    generated_strings<size> result;
    for (std::size_t i = 0; i < size; i++) {
        std::string_view prefix = "value_";
        std::size_t pos = 0;
        for (char c : prefix)
            result.data[i][pos++] = c;

        std::size_t digits = 1;
        for (std::size_t tmp = i; tmp >= 10; tmp /= 10)
            digits++;
        std::size_t tmp = i;
        for (std::size_t j = 0; j < digits; j++) {
            result.data[i][pos + digits - 1 - j] = static_cast<char>('0' + tmp % 10);
            tmp /= 10;
        }
        result.sizes[i] = pos + digits;
    }
    return result;
}

/**
 * Reflection for `size` values, where i-th value is `i * step`, and its string is "value_<i>".
 */
template<std::size_t size, int step>
struct generated_reflection {
    static constexpr generated_strings<size> strings = generate_strings<size>();

    static constexpr std::array<std::pair<int, std::string_view>, size> value = [] {
        std::array<std::pair<int, std::string_view>, size> result = {{}};
        for (std::size_t i = 0; i < size; i++)
            result[i] = {static_cast<int>(i) * step, std::string_view(strings.data[i].data(), strings.sizes[i])};
        return result;
    }();
};

// Values that are next to each other use a flat table, and values that are far apart use a hash table.
template<std::size_t size, int step>
struct generated_tag : sn::tags::tag {};

template<std::size_t size, int step>
[[nodiscard]] constexpr const auto &reflect_enum(std::type_identity<int>, generated_tag<size, step>) noexcept {
    return generated_reflection<size, step>::value;
}

using dense_8_tag = generated_tag<8, 1>;
using sparse_8_tag = generated_tag<8, 1009>;
using dense_64_tag = generated_tag<64, 1>;
using sparse_64_tag = generated_tag<64, 1009>;
using dense_800_tag = generated_tag<800, 1>;
using sparse_800_tag = generated_tag<800, 1009>;

SN_DEFINE_ENUM_STRING_FUNCTIONS(int, sn::case_sensitive, dense_8_tag)
SN_DEFINE_ENUM_STRING_FUNCTIONS(int, sn::case_sensitive, sparse_8_tag)
SN_DEFINE_ENUM_STRING_FUNCTIONS(int, sn::case_sensitive, dense_64_tag)
SN_DEFINE_ENUM_STRING_FUNCTIONS(int, sn::case_sensitive, sparse_64_tag)
SN_DEFINE_ENUM_STRING_FUNCTIONS(int, sn::case_sensitive, dense_800_tag)
SN_DEFINE_ENUM_STRING_FUNCTIONS(int, sn::case_sensitive, sparse_800_tag)

constexpr std::size_t benchmark_input_size = 4096;

/**
 * @return                              Indices into a reflection, either random, or all the same.
 */
static std::vector<std::size_t> make_benchmark_indices(std::size_t size, bool random) {
    std::mt19937 rng(12345); // NOLINT: fixed seed is intended.
    std::vector<std::size_t> result;
    for (std::size_t i = 0; i < benchmark_input_size; i++)
        result.push_back(random ? rng() % size : size / 2);
    return result;
}

template<class Tag, bool random>
static void benchmark_enum_to_string(benchmark::State &state) { // NOLINT
    const auto &reflection = sn::reflect_enum<int>(Tag());

    std::vector<int> values;
    for (std::size_t index : make_benchmark_indices(reflection.size(), random))
        values.push_back(reflection[index].first);

    std::string result;
    std::size_t i = 0;
    for (auto _ : state) {
        sn::to_string(values[i], &result, Tag());
        benchmark::DoNotOptimize(result);
        i = (i + 1) % benchmark_input_size;
    }
}

template<class Tag, bool random>
static void benchmark_enum_from_string(benchmark::State &state) { // NOLINT
    const auto &reflection = sn::reflect_enum<int>(Tag());

    // Strings are copied so that they don't point into the reflection.
    std::vector<std::string> strings;
    for (std::size_t index : make_benchmark_indices(reflection.size(), random))
        strings.emplace_back(reflection[index].second);

    int result = 0;
    std::size_t i = 0;
    for (auto _ : state) {
        sn::from_string(strings[i], &result, Tag());
        benchmark::DoNotOptimize(result);
        i = (i + 1) % benchmark_input_size;
    }
}

// Random values are the worst case, the same value over and over again is the best case. Real workloads are somewhere
// in between.

BENCHMARK_TEMPLATE(benchmark_enum_to_string, dense_8_tag, true);
BENCHMARK_TEMPLATE(benchmark_enum_to_string, dense_8_tag, false);
BENCHMARK_TEMPLATE(benchmark_enum_to_string, sparse_8_tag, true);
BENCHMARK_TEMPLATE(benchmark_enum_to_string, sparse_8_tag, false);
BENCHMARK_TEMPLATE(benchmark_enum_to_string, dense_64_tag, true);
BENCHMARK_TEMPLATE(benchmark_enum_to_string, dense_64_tag, false);
BENCHMARK_TEMPLATE(benchmark_enum_to_string, sparse_64_tag, true);
BENCHMARK_TEMPLATE(benchmark_enum_to_string, sparse_64_tag, false);
BENCHMARK_TEMPLATE(benchmark_enum_to_string, dense_800_tag, true);
BENCHMARK_TEMPLATE(benchmark_enum_to_string, dense_800_tag, false);
BENCHMARK_TEMPLATE(benchmark_enum_to_string, sparse_800_tag, true);
BENCHMARK_TEMPLATE(benchmark_enum_to_string, sparse_800_tag, false);

BENCHMARK_TEMPLATE(benchmark_enum_from_string, dense_8_tag, true);
BENCHMARK_TEMPLATE(benchmark_enum_from_string, dense_8_tag, false);
BENCHMARK_TEMPLATE(benchmark_enum_from_string, dense_64_tag, true);
BENCHMARK_TEMPLATE(benchmark_enum_from_string, dense_64_tag, false);
BENCHMARK_TEMPLATE(benchmark_enum_from_string, dense_800_tag, true);
BENCHMARK_TEMPLATE(benchmark_enum_from_string, dense_800_tag, false);
