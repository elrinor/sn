#pragma once

#include "test_suite.h"

namespace sn::detail {

template<class S, class Stringifier>
test_suite<bool, S> make_boolean_test_suite(Stringifier _) {
    test_suite<bool, S> result;

    result.throwing_from = {
        _(""),
        _("da")
    };

    result.valid_from = {
        {_("0"), false},
        {_("1"), true}
    };

    result.valid_fromto = {
        {_("true"), true},
        {_("false"), false}
    };

    return result;
}

} // namespace sn::detail
