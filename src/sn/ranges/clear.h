#pragma once

namespace sn::detail {
template<class Container>
void do_clear(Container &container) { // NOLINT: intentional lvalue ref.
    clear(container);
}
} // namespace sn::detail

namespace sn::niebloids {
struct clear {
    template<class Container>
    void operator()(Container &container) const { // NOLINT: intentional lvalue ref.
        if constexpr (requires { container.clear(); }) {
            container.clear();
        } else {
            sn::detail::do_clear(container);
        }
    }
};
} // namespace sn::niebloids

namespace sn::ranges {
constexpr sn::niebloids::clear clear;
} // namespace sn::ranges
