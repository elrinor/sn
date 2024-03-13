#pragma once

#include <type_traits>

namespace sn {

/**
 * Base class for all `sn` tags. Derive all your tags from `sn::tag`.
 */
struct tag {};

/**
 * Concept that recognizes `sn` tags.
 *
 * It's mainly used in user-facing functions in `sn` namespace to improve error reporting.
 *
 * For example, consider a call `sn::to_string(x, &s)`, where `s` is not `std::string`. This call will be matched to an
 * overload that returns an `std::string` and assumes that the 2nd arg is a tag, which is definitely not what we want.
 */
template<class T>
concept any_tag = std::is_base_of_v<sn::tag, T>;

} // namespace sn
