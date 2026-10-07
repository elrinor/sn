#pragma once

#include "sn/core/globals.h"

namespace sn::detail {

/**
 * Options for an enum table, this is what the second argument of `SN_DEFINE_ENUM_STRING_FUNCTIONS` is converted to.
 * The constructor is implicit so that `sn::case_sensitive` alone can be passed where options are expected.
 */
struct enum_table_options {
    constexpr enum_table_options(case_sensitivity mode = case_sensitive, enum_table_kind kind = auto_enum_table) noexcept : // NOLINT: implicit conversion is intended.
        mode(mode),
        kind(kind)
    {}

    case_sensitivity mode;
    enum_table_kind kind;
};

} // namespace sn::detail

namespace sn {

[[nodiscard]] constexpr sn::detail::enum_table_options operator|(case_sensitivity mode, enum_table_kind kind) noexcept {
    return {mode, kind};
}

[[nodiscard]] constexpr sn::detail::enum_table_options operator|(enum_table_kind kind, case_sensitivity mode) noexcept {
    return mode | kind;
}

} // namespace sn
