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

/**
 * Options for enum string functions, this is what the second argument of `SN_DEFINE_ENUM_STRING_FUNCTIONS` is. Either
 * a `case_sensitivity` alone, or a `case_sensitivity` combined with an `enum_table_kind` using `operator|`.
 */
struct enum_table_options {
    constexpr enum_table_options(case_sensitivity sensitivity = case_sensitive, // NOLINT: implicit conversion is intended.
                                 enum_table_kind table_kind = auto_enum_table) noexcept :
        mode(sensitivity),
        kind(table_kind)
    {}

    /** Table kind alone is not enough, case sensitivity always has to be specified. */
    enum_table_options(enum_table_kind) = delete; // NOLINT: deleted, so it's not converting anything.

    case_sensitivity mode;
    enum_table_kind kind;
};

[[nodiscard]] constexpr enum_table_options operator|(case_sensitivity mode, enum_table_kind kind) noexcept {
    return {mode, kind};
}

[[nodiscard]] constexpr enum_table_options operator|(enum_table_kind kind, case_sensitivity mode) noexcept {
    return {mode, kind};
}

} // namespace sn
