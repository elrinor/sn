#pragma once

namespace sn {

enum class case_sensitivity {
    case_sensitive,
    case_insensitive
};
using enum case_sensitivity;

/**
 * Kind of the lookup table to use for enum-to-string conversions. A suitable one is picked automatically, so there is
 * normally no need to pass these in. If you do need to, combine it with `case_sensitivity`, e.g.
 * `sn::case_sensitive | sn::flat_enum_table`.
 */
enum class enum_table_kind {
    /** Pick a table kind automatically. This is the default. */
    auto_enum_table,

    /** Array of strings indexed by enum value. The fastest one, but its size depends on the range of enum values. */
    flat_enum_table,

    /** Perfect hash table. */
    hashed_enum_table
};
using enum enum_table_kind;

} // namespace sn
