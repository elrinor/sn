#pragma once

#include <limits>
#include <string>
#include <string_view>

#include "sn/string/string.h"

#include "tester.h"

namespace sn::detail {

inline std::string prepend_zeros(int zeros, std::string_view number_string) {
    std::string result;

    if (number_string.starts_with('-')) {
        result = "-";
        number_string = number_string.substr(1);
    }

    result += std::string(zeros, '0');
    result += number_string;
    return result;
}

template<class T>
inline void run_integer_test_suite() {
    tester<T> t;

    std::initializer_list<std::string_view> always_throwing = {
        "",
        " 1",
        "1 ",
        "\t1",
        "1\t",
        "+1",
        "--1",
        "0-0",
        "0-1",
    };
    t.expect_throwing_from(always_throwing);

    std::initializer_list<std::string_view> throwing_in_base10 = {
        " 111",
        "111 ",
        "\t111",
        "111\t",
        "0x1",
        "0b1",
    };
    t.expect_throwing_from(throwing_in_base10);

    if constexpr (sizeof(T) < sizeof(long long)) {
        t.expect_throwing_from({
            sn::to_string(static_cast<long long>(std::numeric_limits<T>::max()) + 1),
            sn::to_string(static_cast<long long>(std::numeric_limits<T>::min()) - 1)
        });
    } else {
        static_assert(sizeof(T) == 8);

        if constexpr (std::is_unsigned_v<T>) {
            t.expect_throwing_from({
                "-1",
                "18446744073709551616" // max unsigned long long +1
            });
        } else {
            t.expect_throwing_from({
                "-9223372036854775809", // min long long -1
                "9223372036854775808" // max long long +1
            });
        }
    }

    t.expect_valid_fromto({
        {"0", 0},
        {"1", 1},
        {"100", 100}
    });
    if constexpr (std::is_signed_v<T>)
        t.expect_valid_fromto({{"-1", -1}});

    t.expect_valid_from({
        {"010", 10}, // This is not an octal number.
        {prepend_zeros(1, sn::to_string(std::numeric_limits<T>::max())), std::numeric_limits<T>::max()},
        {prepend_zeros(1, sn::to_string(std::numeric_limits<T>::min())), std::numeric_limits<T>::min()},
        {prepend_zeros(100, sn::to_string(std::numeric_limits<T>::max())), std::numeric_limits<T>::max()},
        {prepend_zeros(100, sn::to_string(std::numeric_limits<T>::min())), std::numeric_limits<T>::min()}
    });

    t.expect_valid_roundtrip({
        std::numeric_limits<T>::max(),
        std::numeric_limits<T>::min()
    });

    static constexpr const char *base_strings_for_100[] = {
        nullptr,
        nullptr,
        "1100100",
        "10201",
        "1210",
        "400",
        "244",
        "202",
        "144",
        "121",
        "100",
        "91",
        "84",
        "79",
        "72",
        "6a",
        "64",
        "5f",
        "5a",
        "55",
        "50",
        "4g",
        "4c",
        "48",
        "44",
        "40",
        "3m",
        "3j",
        "3g",
        "3d",
        "3a",
        "37",
        "34",
        "31",
        "2w",
        "2u",
        "2s"
    };

    auto run_base_tests = [&](int base, auto tag) {
        std::string positive_100 = base_strings_for_100[base];
        t.expect_valid_fromto(positive_100, 100, tag);
        t.expect_valid_from(prepend_zeros(100, positive_100), 100, tag);

        t.expect_throwing_from(always_throwing, tag);
        if (base == 10)
            t.expect_throwing_from(throwing_in_base10, tag);

        if (base <= 11) {
            t.expect_throwing_from("0b1", tag);
        } else {
            t.expect_nonthrowing_from("0b1", tag);
        }

        if (base <= 33) {
            t.expect_throwing_from("0x1", tag);
        } else {
            t.expect_nonthrowing_from("0x1", tag);
        }

        if (std::is_signed_v<T>) {
            std::string negative_100 = "-" + positive_100;
            t.expect_valid_fromto(negative_100, -100, tag);
            t.expect_valid_from(prepend_zeros(100, negative_100), -100, tag);
        }
    };

    // Test tn::dynamic_base(N).
    for (std::size_t base = 2; base <= 36; base++)
        run_base_tests(base, tn::dynamic_base(base));

    // Test tn::base<N>.
    auto run_static_base_tests = [&]<int... bases>(std::integer_sequence<int, bases...>) {
        (run_base_tests(bases, tn::base<bases>), ...);
    }; // NOLINT: linter chokes here.
    run_static_base_tests(std::integer_sequence<int, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36>());

    // Test base shortcuts in tn::.
    run_base_tests(2, tn::bin);
    run_base_tests(8, tn::oct);
    run_base_tests(10, tn::dec);
    run_base_tests(16, tn::hex);
}

} // namespace sn::detail
