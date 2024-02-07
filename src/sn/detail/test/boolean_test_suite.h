#pragma once

#include "test_suite.h"

namespace sn::detail {

inline test_suite<bool> make_boolean_test_suite() {
    test_suite<bool> result;

    result.throwing_from = {
        "",
        "da"
    };

    result.valid_from = {
        {"0", false},
        {"1", true}
    };

    result.valid_fromto = {
        {"true", true},
        {"false", false}
    };

    return result;
}

} // namespace sn::detail
