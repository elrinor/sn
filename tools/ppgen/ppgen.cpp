#include <string>
#include <ranges>

#include <fmt/format.h> // NOLINT: not a C system header.

#include "sn/string/string.h"

template<class... Ranges, std::size_t... indices>
std::string join(std::string_view format, std::string_view sep, std::index_sequence<indices...>, const Ranges &... ranges) {
    std::string result;

    // Poor man's zip_view.
    std::tuple iters(ranges.begin()...);
    std::tuple ends(ranges.end()...);
    while (((std::get<indices>(iters) != std::get<indices>(ends)) && ...)) {
        result += fmt::format(fmt::runtime(format), *std::get<indices>(iters)...);
        result += sep;

        (std::get<indices>(iters)++, ...);
    }

    if (!result.empty())
        result.erase(result.size() - sep.size(), sep.size());

    return result;
}

template<class... Ranges>
std::string join(std::string_view format, std::string_view sep, const Ranges &... ranges) {
    return join(format, sep, std::index_sequence_for<Ranges...>(), ranges...);
}

int main(int argc, char **argv) {
    if (argc != 2)
        return 1;

    int count = sn::from_string<int>(argv[1]);
    auto forward_range = std::views::iota(0, count);
    auto reverse_range = std::views::iota(0, count + 1) | std::views::reverse;

    fmt::println(stdout, "#define SN_PP_TUPLE_SIZE(TUPLE) _SN_PP_TUPLE_SIZE_I TUPLE");
    fmt::println(stdout, "#define _SN_PP_TUPLE_SIZE_I(...) _SN_PP_TUPLE_SIZE_II(__VA_ARGS__ __VA_OPT__(,) {})",
                 join("{}", ", ", reverse_range));
    fmt::println(stdout, "#define _SN_PP_TUPLE_SIZE_II({}, size, ...) size",
                 join("v{}", ", ", forward_range));

    fmt::println(stdout, "");
    fmt::println(stdout, "#define SN_PP_TUPLE_FOR_EACH(MACRO, TUPLE) _SN_PP_TUPLE_FOR_EACH_I(MACRO, SN_PP_TUPLE_SIZE(TUPLE), SN_PP_TUPLE_ENUM(TUPLE))");
    fmt::println(stdout, "#define _SN_PP_TUPLE_FOR_EACH_I(MACRO, SIZE, ...) SN_PP_CAT(_SN_PP_TUPLE_FOR_EACH_I_, SIZE)(MACRO, __VA_ARGS__)");
    fmt::println(stdout, "#define _SN_PP_TUPLE_FOR_EACH_I_0(M, DUMMY)");
    for (int i = 1; i <= count; i++) {
        auto range = std::views::iota(0, i);
        fmt::println(stdout, "#define _SN_PP_TUPLE_FOR_EACH_I_{}(M, {}) {}", i, join("v{}", ", ", range), join("M(v{})", " ", range));
    }

    fmt::println(stdout, "");
    fmt::println(stdout, "#define SN_PP_TUPLE_FOR_EACH_I(MACRO, TUPLE) _SN_PP_TUPLE_FOR_EACH_I_I(MACRO, SN_PP_TUPLE_SIZE(TUPLE), SN_PP_TUPLE_ENUM(TUPLE))");
    fmt::println(stdout, "#define _SN_PP_TUPLE_FOR_EACH_I_I(MACRO, SIZE, ...) SN_PP_CAT(_SN_PP_TUPLE_FOR_EACH_I_I_, SIZE)(MACRO, __VA_ARGS__)");
    fmt::println(stdout, "#define _SN_PP_TUPLE_FOR_EACH_I_I_0(M, DUMMY)");
    for (int i = 1; i <= count; i++) {
        auto range = std::views::iota(0, i);
        fmt::println(stdout, "#define _SN_PP_TUPLE_FOR_EACH_I_I_{}(M, {}) {}",
                     i, join("v{}", ", ", range),  join("M({}, v{})", " ", range, range));
    }

    fmt::println(stdout, "");
    fmt::println(stdout, "#define SN_PP_TUPLE_TRANSFORM(MACRO, TUPLE) _SN_PP_TUPLE_TRANSFORM_I(MACRO, SN_PP_TUPLE_SIZE(TUPLE), SN_PP_TUPLE_ENUM(TUPLE))");
    fmt::println(stdout, "#define _SN_PP_TUPLE_TRANSFORM_I(MACRO, SIZE, ...) SN_PP_CAT(_SN_PP_TUPLE_TRANSFORM_I_, SIZE)(MACRO, __VA_ARGS__)");
    fmt::println(stdout, "#define _SN_PP_TUPLE_TRANSFORM_I_0(M, DUMMY) ()");
    for (int i = 1; i <= count; i++) {
        auto range = std::views::iota(0, i);
        fmt::println(stdout, "#define _SN_PP_TUPLE_TRANSFORM_I_{}(M, {}) ({})", i, join("v{}", ", ", range), join("M(v{})", ", ", range));
    }

    fmt::println(stdout, "");
    fmt::println(stdout, "#define SN_PP_BOOL(X) SN_PP_CAT(_SN_PP_BOOL_I_, X)");
    for (int i = 0; i <= count; i++)
        fmt::println(stdout, "#define _SN_PP_BOOL_I_{} {}", i, i ? 1 : 0);

    return 0;
}
