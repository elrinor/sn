#pragma once

#include <limits>

#include "tester.h"
#include "integer_test_suite.h"

namespace sn::detail {

template<class T, class Ops>
inline void run_float_test_suite(const Ops &ops) {
    tester<T, Ops> t(ops);

    t.expect_throwing_from({
        "+1",
        "+1.5",
        " 1.5",
        "1.5 ",
        " 1",
        "1 ",
        "\t10.0000",
        "10.0000\t",
    });

    t.expect_valid_from({
        {"0.0", 0.0f},
        {"-0.0", -0.0f},
        {".5", 0.5f},
        {prepend_zeros(100, "0.0"), 0.0f},
        {"1.0e2", 100.0f},
        {"1.e2", 100.0f},
        {".1e2", 10.0f},
        {".1e+2", 10.0f},
        {"5.0e-1", 0.5f},
        {".1e+2", 10.0f},
        {"5.0e-1", 0.5f},
        {".1e+" + prepend_zeros(100, "2"), 10.0f},
        {"5.0e-" + prepend_zeros(100, "1"), 0.5f},
        {"INF", std::numeric_limits<T>::infinity()},
        {"Inf", std::numeric_limits<T>::infinity()},
        {"iNf", std::numeric_limits<T>::infinity()},
        {"inF", std::numeric_limits<T>::infinity()},
        {"-INF", -std::numeric_limits<T>::infinity()},
        {"-Inf", -std::numeric_limits<T>::infinity()},
        {"-iNf", -std::numeric_limits<T>::infinity()},
        {"-inF", -std::numeric_limits<T>::infinity()},
    });

    t.expect_valid_fromto({
        {"0", 0.0f},
        {"-0", -0.0f},
        {"1", 1.0f},
        {"-1", -1.0f},
        {"1.5", 1.5f},
        {"-1.5", -1.5f},
        {"0.5", 0.5f},
        {"inf", std::numeric_limits<T>::infinity()},
        {"-inf", -std::numeric_limits<T>::infinity()},
    });

    // TODO(elric): test NANs.
}

} // namespace sn::detail
