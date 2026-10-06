#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include <utility> // For std::forward.

#include "sn/string/string.h"

#include "enum_table_exceptions.h"

namespace sn::detail {

/**
 * Type-erased enum table implementation. Uses `std::uint64_t` internally to store enum values.
 *
 * @tparam Base                         Base table. Needs to expose `find_string` and `find_value` functions.
 */
template<class Base>
struct universal_enum_table {
public:
    // sn::type_name is not constexpr b/c it has typeid() as one of its backends. Thus, we cannot use type name as an
    // argument to a constexpr constructor. We can, however, use a pointer to a function doing what we need. This
    // won't be a performance problem b/c it's on the cold (exception-throwing) path.
    using type_name_function = std::string_view (*)();

    template<class... Args>
    constexpr universal_enum_table(type_name_function type_name, bool is_signed, Args &&... args):
        _base(std::forward<Args>(args)...),
        _type_name(type_name),
        _is_signed(is_signed)
    {}

    // We're not following our own API conventions here mainly for the sake of better codegen.
    // Both x86_64 and arm64 ABIs return structs up to 16 bytes in registers, and it makes a lot of sense to use this here.

    struct try_from_string_result {
        std::uint64_t value = 0;
        bool ok = false;
    };

    void to_string(std::uint64_t src, std::string *dst) const {
        std::string_view string;
        if (_base.find_string(src, &string)) {
            dst->assign(string.data(), string.size());
        } else {
            if (_is_signed) {
                // This static_cast relies on implementation-defined behavior, but it's symmetric to the type erasure
                // that's done in enum_table, so it's OK.
                throw_enum_to_string_error(_type_name(), sn::to_string(static_cast<std::int64_t>(src)));
            } else {
                throw_enum_to_string_error(_type_name(), sn::to_string(src));
            }
        }
    }

    [[nodiscard]] std::uint64_t from_string(std::string_view src) const {
        std::uint64_t result = 0;
        if (!_base.find_value(src, &result))
            throw_enum_from_string_error(_type_name(), src);
        return result;
    }

    [[nodiscard]] bool try_to_string(std::uint64_t src, std::string *dst) const noexcept {
        std::string_view string;
        if (_base.find_string(src, &string)) {
            dst->assign(string.data(), string.size());
            return true;
        } else {
            return false;
        }
    }

    [[nodiscard]] try_from_string_result try_from_string(std::string_view src) const noexcept {
        try_from_string_result result;
        result.ok = _base.find_value(src, &result.value);
        return result;
    }

private:
    Base _base;
    type_name_function _type_name;
    bool _is_signed;
};

} // namespace sn::detail
