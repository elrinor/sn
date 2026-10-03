#pragma once

#include <utility> // For std::forward.
#include <string>
#include <string_view>

#include "sn/core/type_name.h"
#include "sn/core/globals.h"

#include "universal_enum_table.h"

namespace sn::detail {

/**
 * Same as `std::is_signed_v`, but also works for enums.
 *
 * @tparam T                            Type to check.
 */
template<class T>
constexpr bool is_signed_ex_v = std::is_signed_v<typename std::conditional_t<std::is_enum_v<T>, std::underlying_type<T>, std::type_identity<T>>::type>;

/**
 * Enum table to be used by different enum table implementations. All the actual work is done in the type-erased
 * `universal_enum_table`, this class is just a wrapper.
 *
 * @tparam T                            Enum type.
 * @tparam mode                         Case sensitivity mode.
 * @tparam Traits                       Table traits.
 * @tparam Base                         Base table to pass to `universal_enum_table`.
 */
template<class T, case_sensitivity mode, class Traits, class Base>
class enum_table {
public:
    using string_type = typename Traits::string_type;
    using string_view_type = typename Traits::string_view_type;

    template<class... Args>
    explicit constexpr enum_table(Args &&... args) : _table(mode, &sn::type_name<T>, is_signed_ex_v<T>, std::forward<Args>(args)...) {}

    void to_string(T src, string_type *dst) const {
        _table.to_string(static_cast<std::uint64_t>(src), dst);
    }

    void from_string(string_view_type src, T *dst) const {
        *dst = static_cast<T>(_table.template from_string<mode>(src));
    }

    [[nodiscard]] bool try_to_string(T src, string_type *dst) const noexcept {
        return _table.try_to_string(static_cast<std::uint64_t>(src), dst);
    }

    [[nodiscard]] bool try_from_string(string_view_type src, T *dst) const noexcept {
        auto result = _table.template try_from_string<mode>(src);
        if (result.ok)
            *dst = static_cast<T>(result.value);
        return result.ok;
    }

private:
    universal_enum_table<Traits, Base> _table;
};

} // namespace sn::detail

