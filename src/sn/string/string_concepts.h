#pragma once

#include "string_builtins.h"

namespace sn {

template<class T, class... Tags>
concept stringable =
    requires(Tags... tags) { sn::builtins::is_string_supported_type(std::type_identity<T>(), tags...); } ||
    requires(Tags... tags) { is_string_supported_type(std::type_identity<T>(), tags...); }; // NOLINT: that ; is needed.

} // namespace sn
