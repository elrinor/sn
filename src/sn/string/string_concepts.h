#pragma once

#include <concepts>

#include "sn/string/detail/string_builtins.h"

namespace sn::builtins::poison {

/**
 * @internal
 *
 * This is a long story.
 *
 * There are several mechanisms in C++ that we can use to make a library statically extensible. The main ones are:
 * - Specializations. This is how `std::hash` works.
 * - Overloads. This is how `std::ranges::swap` works - you need to define the `swap` overload for your type, and ADL
 *   will pick it up when calling `std::ranges::swap`.
 *
 * In my experience, offering overload-based extension points results in cleaner and more conscise code, so this is what
 * we do in `sn`. However, the problem with overloads is that during overload resolution all kinds of nastiness can
 * happen due to how C++ type conversions work.
 *
 * E.g. calling `f('c')` will silently call `f(int)` if `f(char)` is not found, and there is no way to detect this at
 * the call site. This is a real issue - in the first version of the library `sn::to_string((void *) nullptr)` was
 * calling `sn::to_string(bool)`. Several ways around this issue were considered:
 * 1. If you could take an address of a function found via ADL, then this would have been easy. Just test that it's
 *    castable to the signature you need, and that's it. Unfortunately, C++ doesn't allow that.
 * 2. Change extention point signatures to accept two pointers - `to_string(const T *, std::string *)`. This still
 *    leaves the derived-to-base conversions working, which can result in unintended slicing.
 * 3. Make the `SN_DECLARE_STRING_FUNCTIONS` family of macros introduce an intermediate layer of functions that
 *    are constrained with `std::same_as`. This can work, but this also means that `SN_DECLARE_STRING_FUNCTIONS` cannot
 *    be invoked twice for the same type, as it will be generating function definitions.
 * 4. Forget about using overloads for extension points, and use specializations instead. This will work, but it's
 *    ugly and verbose.
 * 5. Introduce another function that would essentially mark a type as "supported", then check that this function
 *    exists from inside the string functions in `sn` namespace. Something like
 *    `is_string_supported_type(std::type_identity<T>)`. This will work, but can be surprising for the end user.
 * 6. Implement an `only<T>` template that would essentially forbid all conversions, and change extension point
 *    signatures accordingly - `to_string(only<const T &>, std::string *)`. This is ugly, but will work.
 * 7. Implement another kind of `only<T>` wrapper that is convertible to the target type and nothing else,
 *    then use it at the call site (in `sn::detail::do_to_string`). This doesn't require changing the extension point
 *    signatures, but unfortunately this doesn't work for the example with `char` above as an implicit conversion chain
 *    in C++ can include a built-in conversion following a user-defined conversion. One might try to just `= delete`
 *    all other conversions, but this also won't work - the compiler doesn't know that the conversion operators are
 *    `= delete`d during overload resolution, so all calls will simply be ambiguous.
 * 8. Just poison the overload set with a function that will match everything for which no perfect match was found via
 *    ADL.
 *
 * #8 is what we're doing here.
 *
 * Drawback of this approach is that the user can still ADL-invoke string functions (using `to_string` instead of
 * `sn::to_string`) to bypass all the checks. Can't do anything about it w/o changing the extension point signatures.
 */
struct string_overload_not_found {};

template<class T, class... Tags>
string_overload_not_found try_to_string(const T &, std::string *, Tags...) noexcept = delete;
template<class T, class... Tags>
string_overload_not_found to_string(const T &src, std::string *, Tags...) = delete;
template<class T, class... Tags>
string_overload_not_found try_from_string(std::string_view src, T *dst, Tags...) noexcept = delete;
template<class T, class... Tags>
string_overload_not_found from_string(std::string_view src, T *dst, Tags...) = delete;

} // namespace sn::builtins::poison

namespace sn::detail::concepts {

using namespace sn::builtins; // NOLINT
using namespace sn::builtins::poison; // NOLINT

template<class T, class... Tags>
concept has_try_to_string =
    requires(const T &src, std::string *dst, Tags... tags) { {try_to_string(src, dst, tags...)} -> std::same_as<bool>; }; // NOLINT

template<class T, class... Tags>
concept has_to_string =
    requires(const T &src, std::string *dst, Tags... tags) { {to_string(src, dst, tags...)} -> std::same_as<void>; }; // NOLINT

template<class T, class... Tags>
concept has_try_from_string =
    requires(std::string_view src, T *dst, Tags... tags) { {try_from_string(src, dst, tags...)} -> std::same_as<bool>; }; // NOLINT

template<class T, class... Tags>
concept has_from_string =
    requires(std::string_view src, T *dst, Tags... tags) { {from_string(src, dst, tags...)} -> std::same_as<void>; }; // NOLINT

} // namespace sn::detail::concepts

namespace sn {

using sn::detail::concepts::has_try_to_string;
using sn::detail::concepts::has_to_string;
using sn::detail::concepts::has_try_from_string;
using sn::detail::concepts::has_from_string;

} // namespace sn
