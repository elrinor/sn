#pragma once

#include <cassert>
#include <cstdint>
#include <utility> // For std::pair.
#include <string>
#include <string_view>

#include "sn/core/globals.h"
#include "sn/string/string.h"

#include "enum_table_exceptions.h"
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
    template<class... Args>
    constexpr universal_enum_table(case_sensitivity mode, std::string_view type_name, bool is_signed, Args &&... args):
        _mode(mode),
        _type_name(type_name),
        _is_signed(is_signed),
        _base(std::forward<Args>(args)...)
    {}

    // We're not following our own API conventions here mainly for the sake of better codegen.
    // Both x86_64 and arm64 ABIs return structs up to 16 bytes in registers, and it makes a lot of sense to use this here.

    struct try_from_string_result {
        std::uint64_t value = 0;
        bool ok = false;
    };

    void to_string(std::uint64_t src, std::string *dst) const {
        auto pos = _base.to_string_map.find(src);
        if (pos != _base.to_string_map.end()) {
            dst->assign(pos->second.data(), pos->second.size());
        } else {
            if (_is_signed) {
                // This static_cast relies on implementation-defined behavior, but it's symmetric to what we have in
                // type_erase_enum_reflection(), so it's OK.
                throw_enum_to_string_error(_type_name, sn::to_string(static_cast<std::int64_t>(src)));
            } else {
                throw_enum_to_string_error(_type_name, sn::to_string(src));
            }
        }
    }

    template<case_sensitivity mode>
    [[nodiscard]] std::uint64_t from_string(std::string_view src) const {
        assert(_mode == mode);

        auto run = [&] (std::string_view src) {
            auto pos = _base.from_string_map.find(src);
            if (pos == _base.from_string_map.end())
                throw_enum_from_string_error(_type_name, src);
            return pos->second;
        };

        if constexpr (mode == case_insensitive) {
            return run(lowercase_buffer(src));
        } else {
            return run(src);
        }
    }

    [[nodiscard]] bool try_to_string(std::uint64_t src, std::string *dst) const noexcept {
        auto pos = _base.to_string_map.find(src);
        if (pos != _base.to_string_map.end()) {
            dst->assign(pos->second.data(), pos->second.size());
            return true;
        } else {
            return false;
        }
    }

    template<case_sensitivity mode>
    [[nodiscard]] try_from_string_result try_from_string(std::string_view src) const noexcept {
        assert(_mode == mode);

        auto run = [&] (std::string_view src) -> try_from_string_result {
            auto pos = _base.from_string_map.find(src);
            if (pos != _base.from_string_map.end()) {
                return {pos->second, true};
            } else {
                return {0, false};
            }
        };

        if constexpr (mode == case_insensitive) {
            return run(lowercase_buffer(src));
        } else {
            return run(src);
        }
    }

private:
    Base _base;
    case_sensitivity _mode;
    std::string_view _type_name;
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
