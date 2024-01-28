#pragma once

#include <cstdint>
#include <concepts>
#include <utility> // For std::pair.
#include <string_view>

#include "sn/core/globals.h"

#include "small_buffer.h"
#include "enum_table_exceptions.h"

namespace sn::detail {

template<class Traits>
concept enum_table_traits = requires(typename Traits::string_type s, typename Traits::string_view_type sv, typename Traits::string_type::value_type c) {
    // There should also be void Traits::assign(*, &s), but we can't express it easily.
    { Traits::to_lower_size(sv) } -> std::same_as<std::size_t>;
    { Traits::to_lower(sv, &c) } -> std::same_as<typename Traits::string_view_type>;
    { Traits::to_std(sv) } -> std::convertible_to<std::string_view>;
}; // NOLINT


/**
 * Type-erased enum table implementation. Uses `std::uint64_t` internally to store enum values.
 *
 * @tparam Traits                       Table traits.
 * @tparam Base                         Base table. Needs to expose `to_string_map` and `from_string_map` fields.
 */
template<enum_table_traits Traits, class Base>
struct universal_enum_table {
public:
    using string_type = typename Traits::string_type;
    using string_view_type = typename Traits::string_view_type;

    // sn::type_name is not constexpr b/c it has typeid() as one of its backends. Thus, we cannot use type name as an
    // argument to a constexpr constructor. We can, however, use a pointer to a function doing what we need. This
    // won't be a performance problem b/c it's on the cold (exception-throwing) path.
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

    struct try_from_string_result {
        std::uint64_t value = 0;
        bool ok = false;
    };

    void to_string(std::uint64_t src, string_type *dst) const {
        auto pos = _base.to_string_map.find(src);
        if (pos != _base.to_string_map.end()) {
            Traits::assign(pos->second, dst);
        } else {
            if (_is_signed) {
                // This static_cast relies on implementation-defined behavior, but it's symmetric to what we have in
                // type_erase_enum_reflection(), so it's OK.
                throw_enum_to_string_error(_type_name(), sn::to_string(static_cast<std::int64_t>(src)));
            } else {
                throw_enum_to_string_error(_type_name(), sn::to_string(src));
            }
        }
    }

    template<case_sensitivity mode>
    [[nodiscard]] std::uint64_t from_string(string_view_type src) const {
        assert(_mode == mode);

        auto run = [&] (string_view_type src) {
            auto pos = _base.from_string_map.find(src);
            if (pos == _base.from_string_map.end())
                throw_enum_from_string_error(_type_name(), Traits::to_std(src));
            return pos->second;
        };

        if constexpr (mode == case_insensitive) {
            small_buffer<typename string_type::value_type, SN_MAX_SMALL_BUFFER_SIZE> buffer(Traits::to_lower_size(src));
            return run(Traits::to_lower(src, buffer.data()));
        } else {
            return run(src);
        }
    }

    [[nodiscard]] bool try_to_string(std::uint64_t src, string_type *dst) const noexcept {
        auto pos = _base.to_string_map.find(src);
        if (pos != _base.to_string_map.end()) {
            Traits::assign(pos->second, dst);
            return true;
        } else {
            return false;
        }
    }

    template<case_sensitivity mode>
    [[nodiscard]] try_from_string_result try_from_string(string_view_type src) const noexcept {
        assert(_mode == mode);

        auto run = [&] (string_view_type src) -> try_from_string_result {
            auto pos = _base.from_string_map.find(src);
            if (pos != _base.from_string_map.end()) {
                return {pos->second, true};
            } else {
                return {0, false};
            }
        };

        if constexpr (mode == case_insensitive) {
            small_buffer<typename string_type::value_type, SN_MAX_SMALL_BUFFER_SIZE> buffer(Traits::to_lower_size(src));
            return run(Traits::to_lower(src, buffer.data()));
        } else {
            return run(src);
        }
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
