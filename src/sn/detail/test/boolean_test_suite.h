#pragma once

#include "tester.h"

namespace sn::detail {

template<class Ops>
inline void run_boolean_test_suite(const Ops &ops) {
    tester<bool, Ops> t(ops);

    t.expect_throwing_from({
        "",
        "da"
    });

    t.expect_valid_from({
        {"0", false},
        {"1", true}
    });

    t.expect_valid_fromto({
        {"true", true},
        {"false", false}
    });
}

} // namespace sn::detail
