#pragma once

#include <array>
#include <cassert>
#include <cstdint>
#include <utility> // For std::pair.
#include <string>
#include <string_view>

#include "sn/core/error_fwd.h"
#include "sn/core/globals.h"

#include "enum_table_errors.h"
#include "lowercase_buffer.h"

namespace sn::detail {

/**
 * Type-erased enum table implementation. Uses `std::uint64_t` internally to store enum values.
 *
 * @tparam Base                         Base table. Needs to expose `to_string_map` and `from_string_map` fields.
 */
template<class Base>
struct universal_enum_table {
public:
    // sn::type_name is not constexpr b/c it has typeid() as one of its backends. Thus, we cannot use type name as an
    // argument to a constexpr constructor. We can, however, use a pointer to a function doing what we need. This
    // won't be a performance problem b/c it's on the cold (error-reporting) path.
    using type_name_function = std::string_view (*)();

    template<class... Args>
    constexpr universal_enum_table(case_sensitivity mode, type_name_function type_name, bool is_signed, Args &&... args):
        _mode(mode),
        _type_name(type_name),
        _is_signed(is_signed),
        _base(std::forward<Args>(args)...)
    {}

    // We're not following our own API conventions here mainly for the sake of better codegen.
    // Both x86_64 and arm64 ABIs return structs up to 16 bytes in registers, and it makes a lot of sense to use this here.

    struct from_string_result {
        std::uint64_t value = 0;
        bool ok = false;
    };

    [[nodiscard]] bool to_string(std::uint64_t src, std::string *dst, sn::error *err) const {
        auto pos = _base.to_string_map.find(src);
        if (pos == _base.to_string_map.end()) [[unlikely]] {
            report_enum_to_string_error(_type_name, src, _is_signed, err);
            return false;
        }

        dst->assign(pos->second.data(), pos->second.size());
        return true;
    }

    template<case_sensitivity mode>
    [[nodiscard]] from_string_result from_string(std::string_view src, sn::error *err) const {
        assert(_mode == mode);

        auto run = [&] (std::string_view key) -> from_string_result {
            auto pos = _base.from_string_map.find(key);
            if (pos != _base.from_string_map.end()) {
                return {pos->second, true};
            } else {
                return {0, false};
            }
        };

        from_string_result result;
        if constexpr (mode == case_insensitive) {
            result = run(lowercase_buffer(src));
        } else {
            result = run(src);
        }

        if (!result.ok) [[unlikely]]
            report_enum_from_string_error(_type_name, src, err); // Note that we're reporting the original string.
        return result;
    }

private:
    Base _base;
    case_sensitivity _mode;
    type_name_function _type_name;
    bool _is_signed;
};


/**
 * Type-erases an enum reflection array, producing an array that's suitable for usage with `universal_enum_table`.
 *
 * @param pairs                         Enum reflection array, as returned from `sn::reflect_enum`.
 * @return                              Type-erased enum reflection array, with enum values converted to `std::uint64_t`.
 */
template<class T, std::size_t size>
consteval auto type_erase_enum_reflection(const std::array<std::pair<T, std::string_view>, size> &pairs) {
    std::array<std::pair<std::uint64_t, std::string_view>, size> result = {{}};
    for (std::size_t i = 0; i < pairs.size(); i++)
        result[i] = {static_cast<std::uint64_t>(pairs[i].first), pairs[i].second};
    return result;
}

} // namespace sn::detail
