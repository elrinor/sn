#pragma once

#include <expected>
#include <functional> // For std::invoke.
#include <string>
#include <type_traits>
#include <utility> // For std::move, std::forward, std::forward_like.

#include "error.h"

namespace sn {

/**
 * Exception thrown by `sn::expected::value` and `sn::expected::or_throw`.
 *
 * Derives from `std::bad_expected_access<sn::error>`, so code that catches that keeps working. Unlike the standard
 * exception, `what()` returns the actual error message instead of a generic string.
 */
class bad_expected_access : public std::bad_expected_access<sn::error> {
public:
    explicit bad_expected_access(sn::error error) :
        std::bad_expected_access<sn::error>(std::move(error)),
        _what(this->error().what())
    {}

    [[nodiscard]] const char *what() const noexcept override {
        return _what.c_str();
    }

private:
    std::string _what;
};

namespace detail {
template<class T>
constexpr bool is_unexpected_v = false;

template<class E>
constexpr bool is_unexpected_v<std::unexpected<E>> = true;

template<class T>
concept expected_like = requires { typename T::value_type; typename T::error_type; typename T::unexpected_type; };
} // namespace detail

/**
 * `std::expected` with `sn::error` as the error type, returned from `sn` conversion functions.
 *
 * Differences from `std::expected`:
 * - `value()` throws `sn::bad_expected_access`, whose `what()` returns the error message.
 * - `or_throw()` is the same as `value()`, but reads better at call sites, e.g. `sn::from_string<int>(s).or_throw()`.
 * - Monadic operations return `sn::expected`, and `and_then` / `or_else` accept functions that return either
 *   `sn::expected` or `std::expected`.
 *
 * Note that accessing an `sn::expected` through a reference to its `std::expected` base, or copying it into an
 * `std::expected`, brings back the standard `value()` that throws `std::bad_expected_access` with a generic message.
 *
 * @tparam T                            Value type.
 */
template<class T>
class expected : public std::expected<T, sn::error> {
    using base_type = std::expected<T, sn::error>;

public:
    using base_type::base_type;

    constexpr expected() = default;

    constexpr expected(const base_type &other) : base_type(other) {} // NOLINT: implicit on purpose.

    constexpr expected(base_type &&other) : base_type(std::move(other)) {} // NOLINT: implicit on purpose.

    template<class Self>
    constexpr decltype(auto) value(this Self &&self) {
        if (!self.has_value())
            throw sn::bad_expected_access(std::forward<Self>(self).error());
        if constexpr (!std::is_void_v<T>)
            return *std::forward<Self>(self);
    }

    /**
     * Same as `value()`, but reads better at call sites.
     */
    template<class Self>
    constexpr decltype(auto) or_throw(this Self &&self) {
        return std::forward<Self>(self).value();
    }

    // std::expected::and_then requires f to return a std::expected specialization, which sn::expected is not. So we
    // can't forward to the base class here.
    template<class Self, class F>
    constexpr auto and_then(this Self &&self, F &&f) {
        if constexpr (std::is_void_v<T>) {
            using U = std::remove_cvref_t<std::invoke_result_t<F>>;
            static_assert(std::is_same_v<typename U::error_type, sn::error>, "f must return an expected with sn::error as the error type");
            using R = sn::expected<typename U::value_type>;
            if (self.has_value())
                return R(std::invoke(std::forward<F>(f)));
            return R(std::unexpect, std::forward<Self>(self).error());
        } else {
            using U = std::remove_cvref_t<std::invoke_result_t<F, decltype(*std::forward<Self>(self))>>;
            static_assert(std::is_same_v<typename U::error_type, sn::error>, "f must return an expected with sn::error as the error type");
            using R = sn::expected<typename U::value_type>;
            if (self.has_value())
                return R(std::invoke(std::forward<F>(f), *std::forward<Self>(self)));
            return R(std::unexpect, std::forward<Self>(self).error());
        }
    }

    // Same as and_then, can't forward to the base class.
    template<class Self, class F>
    constexpr auto or_else(this Self &&self, F &&f) {
        using U = std::remove_cvref_t<std::invoke_result_t<F, decltype(std::forward<Self>(self).error())>>;
        static_assert(std::is_same_v<typename U::value_type, T>, "f must return an expected with the same value type");
        using R = std::conditional_t<std::is_same_v<typename U::error_type, sn::error>, sn::expected<T>, U>;
        if (self.has_value()) {
            if constexpr (std::is_void_v<T>) {
                return R();
            } else {
                return R(std::in_place, *std::forward<Self>(self));
            }
        }
        return R(std::invoke(std::forward<F>(f), std::forward<Self>(self).error()));
    }

    template<class Self, class F>
    constexpr auto transform(this Self &&self, F &&f) { // NOLINT: not std::transform.
        auto result = as_base(std::forward<Self>(self)).transform(std::forward<F>(f)); // NOLINT: not std::transform.
        return sn::expected<typename decltype(result)::value_type>(std::move(result));
    }

    template<class Self, class F>
    constexpr auto transform_error(this Self &&self, F &&f) {
        auto result = as_base(std::forward<Self>(self)).transform_error(std::forward<F>(f));
        if constexpr (std::is_same_v<typename decltype(result)::error_type, sn::error>) {
            return sn::expected<T>(std::move(result));
        } else {
            return result;
        }
    }

    // We need our own comparison operators because the ones inherited from std::expected are ambiguous for a derived
    // type under the C++20 reversed operator rules.

    template<class U> requires(!std::is_void_v<T> && !detail::expected_like<U> && !detail::is_unexpected_v<U>)
    friend constexpr bool operator==(const expected &l, const U &r) {
        return l.has_value() && static_cast<bool>(*l == r);
    }

    template<class E>
    friend constexpr bool operator==(const expected &l, const std::unexpected<E> &r) {
        return !l.has_value() && static_cast<bool>(l.error() == r.error());
    }

    template<class U>
    friend constexpr bool operator==(const expected &l, const expected<U> &r) {
        return static_cast<const base_type &>(l) == static_cast<const std::expected<U, sn::error> &>(r);
    }

    template<class U, class E>
    friend constexpr bool operator==(const expected &l, const std::expected<U, E> &r) {
        return static_cast<const base_type &>(l) == r;
    }

    // With std::expected on the left, std::expected's operator==(const expected &, const T2 &) takes sn::expected as a
    // plain value, and is an exact match. Only a non-template beats it, so the base type gets its own overload. Other
    // std::expected types on the left still go to std::expected's operator, see docs/error_handling.md.
    friend constexpr bool operator==(const base_type &l, const expected &r) {
        return l == static_cast<const base_type &>(r);
    }

private:
    template<class Self>
    static constexpr auto &&as_base(Self &&self) {
        using base_ref = std::conditional_t<std::is_const_v<std::remove_reference_t<Self>>, const base_type &, base_type &>;
        return std::forward_like<Self>(static_cast<base_ref>(self));
    }
};

} // namespace sn
