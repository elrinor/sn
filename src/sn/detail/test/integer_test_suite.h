#pragma once

#include <limits>
#include <string>
#include <string_view>

#include "sn/string/string.h"

#include "test_suite.h"

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

template<class T, class S, class Stringifier>
test_suite<T, S> make_integer_test_suite(Stringifier _) {
    test_suite<T, S> result;

    result.throwing_from = {
        _(""),
        _(" 1"),
        _("1 "),
        _(" 111"),
        _("111 "),
        _("\t111"),
        _("111\t"),
        _("+1"),
        _("--1"),
        _("0x1"),
        _("0b1"),
        _("0-0"),
        _("0-1"),
    };
    if constexpr (sizeof(T) < sizeof(long long)) {
        result.throwing_from.emplace_back(_(sn::to_string(static_cast<long long>(std::numeric_limits<T>::max()) + 1)));
        result.throwing_from.emplace_back(_(sn::to_string(static_cast<long long>(std::numeric_limits<T>::min()) - 1)));
    } else {
        static_assert(sizeof(T) == 8);

        if constexpr (std::is_unsigned_v<T>) {
            result.throwing_from.emplace_back(_("-1"));
            result.throwing_from.emplace_back(_("18446744073709551616")); // max unsigned long long +1
        } else {
            result.throwing_from.emplace_back(_("–9223372036854775809")); // min long long -1
            result.throwing_from.emplace_back(_("9223372036854775808")); // max long long +1
        }
    }

    result.valid_fromto = {
        {_("0"), 0},
        {_("1"), 1},
        {_("100"), 100}
    };
    if constexpr (std::is_signed_v<T>)
        result.valid_fromto.emplace_back(_("-1"), -1);

    result.valid_from = {
        {_(prepend_zeros(1, sn::to_string(std::numeric_limits<T>::max()))), std::numeric_limits<T>::max()},
        {_(prepend_zeros(1, sn::to_string(std::numeric_limits<T>::min()))), std::numeric_limits<T>::min()},
        {_(prepend_zeros(100, sn::to_string(std::numeric_limits<T>::max()))), std::numeric_limits<T>::max()},
        {_(prepend_zeros(100, sn::to_string(std::numeric_limits<T>::min()))), std::numeric_limits<T>::min()}
    };

    result.valid_roundtrip = {
        std::numeric_limits<T>::max(),
        std::numeric_limits<T>::min()
    };

    return result;
}

} // namespace sn::detail
