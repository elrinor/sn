#pragma once

#include <limits>

#include "test_suite.h"
#include "integer_test_suite.h"

namespace sn::detail {

template<class T, class S, class Stringifier>
test_suite<T, S> make_float_test_suite(Stringifier _) {
    test_suite<T, S> result;

    result.throwing_from = {
        _("+1"),
        _("+1.5"),
        _(" 1.5"),
        _("1.5 "),
        _(" 1"),
        _("1 "),
        _("\t10.0000"),
        _("10.0000\t"),
    };

    result.valid_from = {
        {_("0.0"), 0.0},
        {_("-0.0"), -0.0},
        {_(".5"), 0.5},
        {_(prepend_zeros(100, "0.0")), 0.0},
        {_("1.0e2"), 100.0},
        {_("1.e2"), 100.0},
        {_(".1e2"), 10.0},
        {_(".1e+2"), 10.0},
        {_("5.0e-1"), 0.5},
        {_(".1e+2"), 10.0},
        {_("5.0e-1"), 0.5},
        {_(".1e+" + prepend_zeros(100, "2")), 10.0},
        {_("5.0e-" + prepend_zeros(100, "1")), 0.5},
        {_("INF"), std::numeric_limits<T>::infinity()},
        {_("Inf"), std::numeric_limits<T>::infinity()},
        {_("iNf"), std::numeric_limits<T>::infinity()},
        {_("inF"), std::numeric_limits<T>::infinity()},
        {_("-INF"), -std::numeric_limits<T>::infinity()},
        {_("-Inf"), -std::numeric_limits<T>::infinity()},
        {_("-iNf"), -std::numeric_limits<T>::infinity()},
        {_("-inF"), -std::numeric_limits<T>::infinity()},
    };

    result.valid_fromto = {
        {_("0"), 0.0},
        {_("-0"), -0.0},
        {_("1"), 1.0},
        {_("-1"), -1.0},
        {_("1.5"), 1.5},
        {_("-1.5"), -1.5},
        {_("0.5"), 0.5},
        {_("inf"), std::numeric_limits<T>::infinity()},
        {_("-inf"), -std::numeric_limits<T>::infinity()},
    };

    // TODO(elric): test NANs.

    return result;
}

} // namespace sn::detail
