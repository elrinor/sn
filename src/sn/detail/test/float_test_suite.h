#pragma once

#include <limits>

#include "test_suite.h"
#include "integer_test_suite.h"

namespace sn::detail {

template<class T>
inline test_suite<T> make_float_test_suite() {
    test_suite<T> result;

    result.throwing_from = {
        "+1",
        "+1.5",
        " 1.5",
        "1.5 ",
        " 1",
        "1 ",
        "\t10.0000",
        "10.0000\t",
    };

    result.valid_from = {
        {"0.0", 0.0},
        {"-0.0", -0.0},
        {".5", 0.5},
        {prepend_zeros(100, "0.0"), 0.0},
        {"1.0e2", 100.0},
        {"1.e2", 100.0},
        {".1e2", 10.0},
        {".1e+2", 10.0},
        {"5.0e-1", 0.5},
        {".1e+2", 10.0},
        {"5.0e-1", 0.5},
        {".1e+" + prepend_zeros(100, "2"), 10.0},
        {"5.0e-" + prepend_zeros(100, "1"), 0.5},
        {"INF", std::numeric_limits<T>::infinity()},
        {"Inf", std::numeric_limits<T>::infinity()},
        {"iNf", std::numeric_limits<T>::infinity()},
        {"inF", std::numeric_limits<T>::infinity()},
        {"-INF", -std::numeric_limits<T>::infinity()},
        {"-Inf", -std::numeric_limits<T>::infinity()},
        {"-iNf", -std::numeric_limits<T>::infinity()},
        {"-inF", -std::numeric_limits<T>::infinity()},
    };

    result.valid_fromto = {
        {"0", 0.0},
        {"-0", -0.0},
        {"1", 1.0},
        {"-1", -1.0},
        {"1.5", 1.5},
        {"-1.5", -1.5},
        {"0.5", 0.5},
        {"inf", std::numeric_limits<T>::infinity()},
        {"-inf", -std::numeric_limits<T>::infinity()},
    };

    // TODO(elric): test NANs.

    return result;
}

} // namespace sn::detail
