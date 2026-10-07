#pragma once

#include <utility> // For std::forward.
#include <string>
#include <string_view>

#include "sn/core/error_fwd.h"
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
 * @tparam Base                         Base table to pass to `universal_enum_table`.
 */
template<class T, case_sensitivity mode, class Base>
class enum_table {
public:
    template<class... Args>
    explicit constexpr enum_table(Args &&... args) : _table(mode, &sn::type_name<T>, is_signed_ex_v<T>, std::forward<Args>(args)...) {}

    [[nodiscard]] bool to_string(T src, std::string *dst, sn::error *err) const {
        return _table.to_string(static_cast<std::uint64_t>(src), dst, err);
    }

    [[nodiscard]] bool from_string(std::string_view src, T *dst, sn::error *err) const {
        auto result = _table.template from_string<mode>(src, err);
        if (result.ok)
            *dst = static_cast<T>(result.value);
        return result.ok;
    }

private:
    universal_enum_table<Base> _table;
};

} // namespace sn::detail

