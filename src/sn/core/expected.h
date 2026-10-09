#pragma once

#include <expected>

#include "error.h"

namespace sn {

/**
 * Result of `sn` conversion functions, a value or an `sn::error`.
 *
 * `value()` throws `sn::bad_expected_access` with the error message as `what()`, see the specialization of
 * `std::bad_expected_access` in `error.h`.
 *
 * @tparam T                            Value type.
 */
template<class T>
using expected = std::expected<T, sn::error>;

/**
 * Exception thrown by `sn::expected::value()`.
 */
using bad_expected_access = std::bad_expected_access<sn::error>;

} // namespace sn
