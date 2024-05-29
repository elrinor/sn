#pragma once

#include <cassert>

#include "tag.h"

namespace sn {

class dynamic_base_tag : public tag {
public:
    constexpr explicit dynamic_base_tag(int value) : _value(value) {
        assert(value >= 2 && value <= 36);
    }

    [[nodiscard]] constexpr int value() const {
        return _value;
    }

private:
    int _value = 0;
};

template<int value>
struct base_tag : public tag {
    // We only support sane defaults for the static base_tag.
    static_assert(value == 2 || value == 8 || value == 10 || value == 16);
};

} // namespace sn

namespace tn {

[[nodiscard]] constexpr sn::dynamic_base_tag dynamic_base(int value) {
    return sn::dynamic_base_tag(value);
}

template<int value>
constexpr sn::base_tag<value> base;
template<int value> requires(value != 2 && value != 8 && value != 10 && value != 16)
constexpr sn::dynamic_base_tag base<value> = sn::dynamic_base_tag(value);

constexpr sn::base_tag<2> bin;
constexpr sn::base_tag<8> oct;
constexpr sn::base_tag<10> dec;
constexpr sn::base_tag<16> hex;

} // namespace tn
