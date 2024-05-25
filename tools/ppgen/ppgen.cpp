#include <ranges>

#include <fmt/format.h> // NOLINT: not a C system header.

#include "sn/string/string.h"

int main(int argc, char **argv) {
    if (argc != 2)
        return 1;

    int count = sn::from_string<int>(argv[1]);
    auto forward_range = std::views::iota(0, count);
    auto reverse_range = std::views::iota(0, count + 1) | std::views::reverse;

    fmt::println(stdout, "#define SN_PP_TUPLE_SIZE(TUPLE) _SN_PP_TUPLE_SIZE_I TUPLE");
    fmt::println(stdout, "#define _SN_PP_TUPLE_SIZE_I(...) _SN_PP_TUPLE_SIZE_II(__VA_ARGS__ __VA_OPT__(,) {})",
                 fmt::join(reverse_range, ", "));
    fmt::println(stdout, "#define _SN_PP_TUPLE_SIZE_II(v{}, size, ...) size",
                 fmt::join(forward_range, ", v"));

    fmt::println(stdout, "");
    fmt::println(stdout, "#define SN_PP_TUPLE_FOR_EACH(MACRO, TUPLE) _SN_PP_TUPLE_FOR_EACH_I(MACRO, SN_PP_TUPLE_SIZE(TUPLE), SN_PP_TUPLE_ENUM(TUPLE))");
    fmt::println(stdout, "#define _SN_PP_TUPLE_FOR_EACH_I(MACRO, SIZE, ...) SN_PP_CAT(_SN_PP_TUPLE_FOR_EACH_I_, SIZE)(MACRO, __VA_ARGS__)");
    fmt::println(stdout, "#define _SN_PP_TUPLE_FOR_EACH_I_1(MACRO, v0) MACRO(v0)");
    for (int i = 2; i <= count; i++) {
        auto range = std::views::iota(0, i);
        auto subrange = std::views::iota(0, i - 1);
        fmt::println(stdout, "#define _SN_PP_TUPLE_FOR_EACH_I_{}(MACRO, v{}) _SN_PP_TUPLE_FOR_EACH_I_{}(MACRO, v{}) MACRO(v{})",
                     i, fmt::join(range, ", v"),  i - 1, fmt::join(subrange, ", v"), i - 1);
    }

    fmt::println(stdout, "");
    fmt::println(stdout, "#define SN_PP_TUPLE_FOR_EACH_I(MACRO, TUPLE) _SN_PP_TUPLE_FOR_EACH_I_I(MACRO, SN_PP_TUPLE_SIZE(TUPLE), SN_PP_TUPLE_ENUM(TUPLE))");
    fmt::println(stdout, "#define _SN_PP_TUPLE_FOR_EACH_I_I(MACRO, SIZE, ...) SN_PP_CAT(_SN_PP_TUPLE_FOR_EACH_I_I_, SIZE)(MACRO, __VA_ARGS__)");
    fmt::println(stdout, "#define _SN_PP_TUPLE_FOR_EACH_I_I_1(MACRO, v0) MACRO(0, v0)");
    for (int i = 2; i <= count; i++) {
        auto range = std::views::iota(0, i);
        auto subrange = std::views::iota(0, i - 1);
        fmt::println(stdout, "#define _SN_PP_TUPLE_FOR_EACH_I_I_{}(MACRO, v{}) _SN_PP_TUPLE_FOR_EACH_I_I_{}(MACRO, v{}) MACRO({}, v{})",
                     i, fmt::join(range, ", v"),  i - 1, fmt::join(subrange, ", v"), i - 1, i - 1);
    }

    fmt::println(stdout, "");
    fmt::println(stdout, "#define SN_PP_BOOL(X) SN_PP_CAT(_SN_PP_BOOL_I_, X)");
    for (int i = 0; i <= count; i++)
        fmt::println(stdout, "#define _SN_PP_BOOL_I_{} {}", i, i ? 1 : 0);

    return 0;
}
