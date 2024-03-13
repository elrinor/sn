#include <ranges>

#include <fmt/format.h> // NOLINT: not a C system header.

#include "sn/string/string.h"

int main(int argc, char **argv) {
    if (argc != 2)
        return 1;

    int count = sn::from_string<int>(argv[1]);
    auto forward_range = std::views::iota(1, count + 1);
    auto reverse_range = forward_range | std::views::reverse;

    fmt::println(stdout, "#define SN_PP_TUPLE_SIZE(TUPLE) _SN_PP_TUPLE_SIZE_I TUPLE");
    fmt::println(stdout, "#define _SN_PP_TUPLE_SIZE_I(...) _SN_PP_TUPLE_SIZE_II(__VA_ARGS__, {}, )",
                 fmt::join(reverse_range, ", "));
    fmt::println(stdout, "#define _SN_PP_TUPLE_SIZE_II(v{}, size, ...) size",
                 fmt::join(forward_range, ", v"));


    fmt::println(stdout, "");
    fmt::println(stdout, "#define SN_PP_TUPLE_FOR_EACH(MACRO, TUPLE) _SN_PP_TUPLE_FOR_EACH_I(MACRO, SN_PP_TUPLE_SIZE(TUPLE), SN_PP_REMOVE_PARENS(TUPLE))");
    fmt::println(stdout, "#define _SN_PP_TUPLE_FOR_EACH_I(MACRO, SIZE, ...) SN_PP_CAT(_SN_PP_TUPLE_FOR_EACH_I_, SIZE)(MACRO, __VA_ARGS__)");
    fmt::println(stdout, "#define _SN_PP_TUPLE_FOR_EACH_I_1(MACRO, v1) MACRO(v1)");

    for (int i = 2; i <= count; i++) {
        auto range = std::views::iota(1, i + 1);
        auto subrange = std::views::iota(2, i + 1);
        fmt::println(stdout, "#define _SN_PP_TUPLE_FOR_EACH_I_{}(MACRO, v{}) _SN_PP_TUPLE_FOR_EACH_I_1(MACRO, v1) _SN_PP_TUPLE_FOR_EACH_I_{}(MACRO, v{})",
                     i, fmt::join(range, ", v"),  i - 1, fmt::join(subrange, ", v"));
    }

    return 0;
}
