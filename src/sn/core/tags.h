#pragma once

namespace tn::detail {

/**
 * Implementation tag for overloads that should ignore user-defined conversions. See how an overload for `bool` is
 * implemented.
 */
struct explicit_type_tag {};

constexpr explicit_type_tag explicit_type;

} // namespace tn::detail
