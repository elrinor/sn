#pragma once

#include <string>
#include <string_view>
#include <type_traits>

#include "sn/core/preprocessor.h" // For __VA_OPT__.

/**
 * @internal
 *
 * Enables `sn` string functions for `TYPE`.
 *
 * Some notes on why do we even need this macro in the first place. This is going to be a long comment.
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
 * call site. This is a real issue - in the first version of the library `sn::to_string((void *) nullptr)` was calling
 * `sn::to_string(bool)`. There are several ways around this issue:
 * 1. Change extention point signatures to accept two pointers - `to_string(const T *, std::string *)`. This still
 *    leaves the derived-to-base conversions working, which can be surprising for the users.
 * 2. Implement an `only<T>` template that would essentially forbid all conversions, and change extension point
 *    signatures accordingly - `to_string(only<const T &>, std::string *)`. This is ugly, but will work.
 * 3. Wrap the 1st arg into some wrapper at call site that is convertible to the target type, w/o changing the
 *    extension point signatures. This won't work for the example with `char` above as an implicit conversion chain in
 *    C++ can include a built-in conversion following a user-defined conversion.
 * 4. Make the `SN_DECLARE_STRING_FUNCTIONS` family of macros introduce an intermediate layer of functions that
 *    are constrained with `std::same_as`. This can work, but this also means that `SN_DECLARE_STRING_FUNCTIONS` cannot
 *    be invoked twice for the same type, as it will be generating function definitions.
 * 5. Forget about using overloads for extension points, and use specializations instead. This will work, but it's
 *    ugly and verbose.
 * 6. Introduce another function that would essentially mark a type as "supported", then check for it in string
 *    functions inside the `sn` namespace.
 *
 * #6 is what we're doing here. `_SN_ENABLE_STRING_FUNCTIONS` generates a function declaration that is ADL-picked by
 * `sn::stringable`, which in turn is used to restrict the set of types usable with `sn` string functions.
 *
 * Drawbacks of this approach are:
 * 1. User can still ADL-invoke string functions (using `to_string` instead of `sn::to_string`) to bypass all the
 *    checks. Can't do anything about it w/o changing the extension point signatures.
 * 2. If the user isn't careful and uses `_SN_ENABLE_STRING_FUNCTIONS` without declaring all the string functions,
 *    then stuff can blow up. This is why it's private API.
 *
 * @param TYPE                          Type to enable string functions for.
 * @param ATTRIBUTES                    Function attributes to use.
 * @param ...                           Additional tags.
 */
#define _SN_ENABLE_STRING_FUNCTIONS(TYPE, ATTRIBUTES, ... /* TAGS */)                                                   \
    ATTRIBUTES void is_string_supported_type(std::type_identity<TYPE> __VA_OPT__(,) __VA_ARGS__);

#define _SN_DECLARE_STRING_FUNCTIONS_I(TYPE, TYPE_ARG, NORMAL_ATTRIBUTES, NODISCARD_ATTRIBUTES, ... /* TAGS */)         \
    _SN_ENABLE_STRING_FUNCTIONS(TYPE, NORMAL_ATTRIBUTES __VA_OPT__(,) __VA_ARGS__);                                     \
    NODISCARD_ATTRIBUTES bool try_to_string(TYPE_ARG src, std::string *dst __VA_OPT__(,) __VA_ARGS__) noexcept;         \
    NORMAL_ATTRIBUTES void to_string(TYPE_ARG src, std::string *dst __VA_OPT__(,) __VA_ARGS__);                         \
    NODISCARD_ATTRIBUTES bool try_from_string(std::string_view src, TYPE *dst __VA_OPT__(,) __VA_ARGS__) noexcept;      \
    NORMAL_ATTRIBUTES void from_string(std::string_view src, TYPE *dst __VA_OPT__(,) __VA_ARGS__);

/**
 * @internal
 *
 * Same as `SN_DECLARE_STRING_FUNCTIONS`, but `to_string` and `try_to_string` take `TYPE` by value. This can result in
 * better codegen on most architectures as the 1st arg can now be passed in registers.
 */
#define _SN_DECLARE_STRING_FUNCTIONS_BY_VALUE(TYPE, ... /* TAGS */)                                                     \
    _SN_DECLARE_STRING_FUNCTIONS_I(TYPE, TYPE, [[]], [[nodiscard]] __VA_OPT__(,) __VA_ARGS__)

#define SN_DECLARE_STRING_FUNCTIONS(TYPE, ... /* TAGS */)                                                               \
    _SN_DECLARE_STRING_FUNCTIONS_I(TYPE, const TYPE &, [[]], [[nodiscard]] __VA_OPT__(,) __VA_ARGS__)

#define SN_DECLARE_FRIEND_STRING_FUNCTIONS(TYPE, ... /* TAGS */)                                                        \
    _SN_DECLARE_STRING_FUNCTIONS_I(TYPE, const TYPE &, friend, friend __VA_OPT__(,) __VA_ARGS__) // Can't have [[nodiscard]] on a friend function declaration...
