#include <cassert>
#include <algorithm>
#include <string>
#include <compare>
#include <charconv>
#include <limits>
#include <map>
#include <vector>
#include <type_traits>

#include <fmt/format.h> // NOLINT: not a C system header.
#include <fmt/ranges.h> // NOLINT: not a C system header. For fmt::join.

struct key {
    bool sign = false;
    int size = 0;
    int base = 0;

    key(bool sign, int size, int base) : sign(sign), size(size), base(base) {}

    friend auto operator<=>(const key &, const key &) = default;
};

std::map<key, int> max_lengths;

template<class T>
void fill_max_sizes() {
    char buffer[1024];

    for (int base = 2; base <= 36; base++) {
        std::to_chars_result result_min = std::to_chars(buffer, buffer + 1024, std::numeric_limits<T>::min(), base);
        std::to_chars_result result_max = std::to_chars(buffer, buffer + 1024, std::numeric_limits<T>::max(), base);

        int length = std::max(result_min.ptr - buffer, result_max.ptr - buffer);
        key k(std::is_signed_v<T>, sizeof(T), base);
        assert(!max_lengths.contains(k) || max_lengths.find(k)->second == length);
        max_lengths[k] = length;
    }
}

int main(int argc, char **argv) {
    if (argc != 1)
        return 1;

    fill_max_sizes<short>();
    fill_max_sizes<unsigned short>();
    fill_max_sizes<int>();
    fill_max_sizes<unsigned int>();
    fill_max_sizes<long>();
    fill_max_sizes<unsigned long>();
    fill_max_sizes<long long>();
    fill_max_sizes<unsigned long long>();

    fmt::println("template<bool is_signed, int size>");
    fmt::println("static constexpr std::nullptr_t max_integer_lengths = nullptr;");

    for (int size : {2, 4, 8}) {
        for (bool sign : {true, false}) {
            std::vector<int> values;
            for (int base = 2; base <= 36; base++) {
                key k(sign, size, base);
                assert(max_lengths.contains(k));
                values.push_back(max_lengths[k]);
            }
            fmt::println("template<>");
            fmt::println("constexpr std::array<std::uint8_t, 35> max_integer_lengths<{}, {}> = {{{}}};",
                         sign, size, fmt::join(values, ", "));
        }
    }

    return 0;
}
