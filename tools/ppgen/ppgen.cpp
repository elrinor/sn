#include <format>
#include <print>
#include <string>
#include <ranges>
#include <tuple>

#include "sn/string/string.h"

template<class... Ranges, std::size_t... indices>
std::string join(std::string_view format, std::string_view sep, std::index_sequence<indices...>, const Ranges &... ranges) {
    std::string result;

    // Poor man's zip_view.
    std::tuple iters(ranges.begin()...);
    std::tuple ends(ranges.end()...);
    while (((std::get<indices>(iters) != std::get<indices>(ends)) && ...)) {
        std::tuple values(*std::get<indices>(iters)...); // std::make_format_args needs lvalues.
        result += std::vformat(format, std::make_format_args(std::get<indices>(values)...));
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

    int count = sn::from_string<int>(argv[1]).value();
    auto forward_range = std::views::iota(0, count);
    auto reverse_range = std::views::iota(0, count + 1) | std::views::reverse;

    std::println(stdout, "#define SN_PP_TUPLE_SIZE(TUPLE) _SN_PP_TUPLE_SIZE_I TUPLE");
    std::println(stdout, "#define _SN_PP_TUPLE_SIZE_I(...) _SN_PP_TUPLE_SIZE_II(__VA_ARGS__ __VA_OPT__(,) {})",
                 join("{}", ", ", reverse_range));
    std::println(stdout, "#define _SN_PP_TUPLE_SIZE_II({}, size, ...) size",
                 join("v{}", ", ", forward_range));

    std::println(stdout, "");
    std::println(stdout, "#define SN_PP_TUPLE_ELEM(I, TUPLE) SN_PP_CAT(_SN_PP_TUPLE_ELEM_O_, I) TUPLE");
    for (int i = 1; i <= count; i++) {
        auto range = std::views::iota(0, i);
        std::println(stdout, "#define _SN_PP_TUPLE_ELEM_O_{}({}, ...) v{}",
                     i - 1, join("v{}", ", ", range), i - 1);
    }

    std::println(stdout, "");
    std::println(stdout, "#define SN_PP_TUPLE_FOR_EACH_DI(MACRO, DATA, TUPLE) _SN_PP_TUPLE_FOR_EACH_DI_I(MACRO, DATA, SN_PP_TUPLE_SIZE(TUPLE), SN_PP_TUPLE_ENUM(TUPLE))");
    std::println(stdout, "#define _SN_PP_TUPLE_FOR_EACH_DI_I(MACRO, DATA, SIZE, ...) SN_PP_CAT(_SN_PP_TUPLE_FOR_EACH_DI_O_, SIZE)(MACRO, DATA, __VA_ARGS__)");
    std::println(stdout, "#define _SN_PP_TUPLE_FOR_EACH_DI_O_0(M, D, DUMMY)");
    for (int i = 1; i <= count; i++) {
        auto range = std::views::iota(0, i);
        std::println(stdout, "#define _SN_PP_TUPLE_FOR_EACH_DI_O_{}(M, D, {}) {}",
                     i, join("v{}", ", ", range),  join("M(D, {}, v{})", " ", range, range));
    }

    std::println(stdout, "");
    std::println(stdout, "#define SN_PP_BOOL(X) SN_PP_CAT(_SN_PP_BOOL_I_, X)");
    for (int i = 0; i <= count; i++)
        std::println(stdout, "#define _SN_PP_BOOL_I_{} {}", i, i ? 1 : 0);

    return 0;
}
