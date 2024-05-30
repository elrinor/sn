#pragma once

#include <limits>
#include <string>
#include <string_view>

#include "enum_test_suite.h"
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

template<class T, class Ops>
inline void run_integer_test_suite(const Ops &ops) {
    tester<T, Ops> t(ops);

    t.expect_throwing_from({
        "",
        " 1",
        "1 ",
        " 111",
        "111 ",
        "\t111",
        "111\t",
        "+1",
        "--1",
        "0x1",
        "0b1",
        "0-0",
        "0-1",
    });

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
}

} // namespace sn::detail
