#pragma once

#include <string_view>
#include <type_traits> // For std::type_identity.

/**
 * Overrides the name that `sn::type_name` returns for `TYPE`. This is the name that shows up in error messages.
 *
 * The following function will be generated:
 * ```
 * [[nodiscard]] constexpr std::string_view type_name(std::type_identity<TYPE>) noexcept;
 * ```
 *
 * It's found via ADL, so this macro must be invoked in the namespace of `TYPE`. It must also come before anything that
 * uses `sn::type_name<TYPE>()` at compile time, e.g. before `SN_DEFINE_ENUM_STRING_FUNCTIONS` for `TYPE`.
 *
 * @param TYPE                          Type to override the name for.
 * @param NAME                          Name to use, a string literal.
 */
#define SN_DEFINE_TYPE_NAME(TYPE, NAME)                                                                                 \
    [[nodiscard]] constexpr std::string_view type_name(std::type_identity<TYPE>) noexcept {                             \
        return NAME;                                                                                                    \
    }
