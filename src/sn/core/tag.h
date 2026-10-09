#pragma once

#include <type_traits>

namespace sn::tags {

/**
 * Base class for all `sn` tags. Derive all your tags from `sn::tag`.
 */
struct tag {};

} // namespace sn::tags

namespace sn::concepts {

/**
 * Concept that recognizes `sn` tags.
 *
 * It's used in user-facing functions and classes in `sn` namespace to improve error reporting.
 *
 * For example, consider a call `sn::to_string(x, &s)`, where the caller forgot the `sn::error *` argument. Without the
 * concept, this call would be matched to the overload that returns `sn::expected<std::string>`, treating `&s` as a tag,
 * which is definitely not what we want.
 */
template<class T>
concept tag = std::is_base_of_v<sn::tags::tag, T>;

} // namespace sn::concepts
