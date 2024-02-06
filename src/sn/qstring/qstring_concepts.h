#pragma once

#include "qstring_builtins.h"

namespace sn {

template<class T, class... Tags>
concept qstringable =
    requires(Tags... tags) { sn::builtins::is_qstring_supported_type(std::type_identity<T>(), tags...); } ||
    requires(Tags... tags) { is_qstring_supported_type(std::type_identity<T>(), tags...); }; // NOLINT: that ; is needed.

} // namespace sn
