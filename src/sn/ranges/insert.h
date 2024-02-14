#pragma once

#include <utility> // For std::forward.

namespace sn::detail {
template<class Container, class T>
void do_insert(Container &container, T &&value) { // NOLINT: intentional lvalue ref.
    insert(container, value);
}
} // namespace sn::detail

namespace sn::niebloids {
struct insert {
    template<class Container, class T>
    void operator()(Container &container, T &&value) const { // NOLINT: intentional lvalue ref.
        if constexpr (requires { container.push_back(std::forward<T>(value)); }) {
            container.push_back(std::forward<T>(value));
        } else if constexpr (requires { container.insert(std::forward<T>(value)); }) {
            container.insert(std::forward<T>(value));
        } else {
            sn::detail::do_insert(container, std::forward<T>(value));
        }
    }
};
} // namespace sn::niebloids

namespace sn::ranges {
// TODO(elric): DOCS! calls (almost) the same methods as ranges::to, see cppref
constexpr  sn::niebloids::insert insert;
} // namespace sn::ranges
