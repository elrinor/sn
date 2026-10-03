#pragma once

#include <cassert>

#include "sn/core/tag.h"

namespace sn::tags {

/**
 * Tag for converting integers to/from string using bases other than 10.
 *
 * Don't use this tag type in your code directly, use `tn::base<N>` instead. This tag type only supports a limited
 * number of base values, while `tn::base<N>` becomes a `sn::tags::dynamic_base_tag` for values that are not supported by
 * this tag type.
 *
 * @tparam value                        Base value.
 * @see tn::base
 */
template<int value>
struct base_tag : public tag {
    // We only support sane defaults for the static base_tag.
    static_assert(value == 2 || value == 8 || value == 10 || value == 16);
};

/**
 * Tag for converting integers to/from string using bases other than 10.
 *
 * Don't use this tag type in your code directly, call `tn::dynamic_base(N)`, or `tn::base<N>` if you know your base at
 * compile time.
 *
 * @see tn::dynamic_base
 * @see tn::base
 */
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

} // namespace sn::tags

namespace sn::detail {
consteval int base_value() {
    return 10;
}

template<int base>
consteval int base_value(sn::tags::base_tag<base>) {
    return base;
}

inline int base_value(sn::tags::dynamic_base_tag base) {
    return base.value();
}
} // namespace sn::detail

namespace tn {
/**
 * Creates a tag for converting integers to/from string using bases other than 10.
 *
 * If you know your base at compile time, use `sn::base<N>` instead, it's more efficient.
 *
 * Example usage
 * ```
 * std::string s = sn::to_string(64, sn::base<8>); // s = "100";
 * ```
 *
 * @param value                         Base value.
 * @return                              Tag to be used with `sn` functions.
 */
[[nodiscard]] constexpr sn::tags::dynamic_base_tag dynamic_base(int value) {
    return sn::tags::dynamic_base_tag(value);
}

/**
 * Tag for converting integers to/from string using bases other than 10. Use this tag if the base you need is known at
 * compile time.
 *
 * Consider using `tn::bin`, `tn::oct` or `tn::hex` for (de)serializing to/from binary, octal or hexadecimal
 * representations. They work the same but are shorter to type.
 *
 * @tparam value                        Base value.
 */
template<int value>
constexpr sn::tags::base_tag<value> base;
template<int value> requires(value != 2 && value != 8 && value != 10 && value != 16)
constexpr sn::tags::dynamic_base_tag base<value> = sn::tags::dynamic_base_tag(value);

constexpr sn::tags::base_tag<2> bin;
constexpr sn::tags::base_tag<8> oct;
constexpr sn::tags::base_tag<10> dec;
constexpr sn::tags::base_tag<16> hex;

} // namespace tn
