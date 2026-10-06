#pragma once

#include "sn/core/globals.h"

namespace sn::detail {

enum class enum_table_kind_option {
    automatic,
    flat,
    hashed
};

/**
 * Options for an enum table, this is what the second argument of `SN_DEFINE_ENUM_STRING_FUNCTIONS` is converted to.
 */
struct enum_table_options {
    case_sensitivity mode = case_sensitive;
    enum_table_kind_option kind = enum_table_kind_option::automatic;
};

[[nodiscard]] constexpr enum_table_options to_enum_table_options(case_sensitivity mode) noexcept {
    return {mode, enum_table_kind_option::automatic};
}

[[nodiscard]] constexpr enum_table_options to_enum_table_options(enum_table_options options) noexcept {
    return options;
}

} // namespace sn::detail

namespace sn {

[[nodiscard]] constexpr sn::detail::enum_table_options operator|(case_sensitivity mode, enum_table_kind kind) noexcept {
    return {mode, kind == flat_enum_table ? sn::detail::enum_table_kind_option::flat : sn::detail::enum_table_kind_option::hashed};
}

[[nodiscard]] constexpr sn::detail::enum_table_options operator|(enum_table_kind kind, case_sensitivity mode) noexcept {
    return mode | kind;
}

} // namespace sn
