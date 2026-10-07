#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include <utility> // For std::pair.

#include "sn/detail/enum_table/enum_string_maps.h"
#include "sn/detail/enum_table/enum_table.h"

namespace sn::detail {

template<enum_table_spec spec, class Hash = enum_table_hash, class T, std::size_t size>
[[nodiscard]] consteval enum_to_string_map<spec, Hash> make_enum_to_string_map(const std::array<std::pair<T, std::string_view>, size> &reflection) {
    return enum_to_string_map<spec, Hash>(collect_to_string_pairs<spec.to_string_count>(reflection), spec.min_value);
}

template<enum_table_spec spec, class Hash = enum_table_hash, class T, std::size_t size>
[[nodiscard]] consteval string_to_enum_map<spec, Hash> make_string_to_enum_map(const std::array<std::pair<T, std::string_view>, size> &reflection) {
    return string_to_enum_map<spec, Hash>(collect_from_string_pairs<spec.from_string_count>(reflection), spec.min_value);
}

/**
 * Base table for `std::string` enum tables, to be used with `universal_enum_table`.
 *
 * Enum-to-string conversions use either a flat table or a perfect hash table, depending on the enum. String-to-enum
 * conversions always use a perfect hash table. An empty string, if it's listed in the reflection, is not stored in
 * the tables and is handled here.
 *
 * @tparam spec                         Table spec, as returned by `make_enum_table_spec`.
 * @tparam Hash                         Hash functions to use.
 */
template<enum_table_spec spec, class Hash = enum_table_hash>
class string_enum_table_base {
public:
    constexpr string_enum_table_base(const enum_to_string_map<spec, Hash> &to_string_map, const string_to_enum_map<spec, Hash> &from_string_map) :
        _to_string_map(to_string_map),
        _from_string_map(from_string_map)
    {}

    [[nodiscard]] constexpr bool find_string(std::uint64_t value, std::string_view *result) const noexcept {
        if constexpr (spec.empty_string_is_primary) {
            if (value == spec.empty_string_value) {
                *result = std::string_view();
                return true;
            }
        }

        return _to_string_map.find(value, result);
    }

    [[nodiscard]] constexpr bool find_value(std::string_view string, std::uint64_t *result) const noexcept {
        if (string.empty()) {
            if constexpr (spec.has_empty_string) {
                *result = spec.empty_string_value;
                return true;
            } else {
                return false;
            }
        }

        return _from_string_map.find(string, result);
    }

private:
    enum_to_string_map<spec, Hash> _to_string_map;
    string_to_enum_map<spec, Hash> _from_string_map;
};

} // namespace sn::detail

// The two maps below are built as separate constexpr variables, and not right in the constructor of the table, because
// compilers limit the number of steps in a single constant evaluation. This way each of the maps gets its own budget,
// which matters for enums with hundreds of values. With default limits an enum of about 1400 values still builds on
// clang, and one of about 4000 values on GCC. Past that the compiler reports that its limit was hit.

#define _SN_DEFINE_ENUM_STRING_TABLE(TABLE_NAME, ENUM, OPTIONS, REFLECTION_ARG)                                         \
    static constexpr auto table_spec = sn::detail::make_enum_table_spec(REFLECTION_ARG, OPTIONS);                       \
    static constexpr auto to_string_map = sn::detail::make_enum_to_string_map<table_spec>(REFLECTION_ARG);              \
    static constexpr auto from_string_map = sn::detail::make_string_to_enum_map<table_spec>(REFLECTION_ARG);            \
    static constexpr auto TABLE_NAME =                                                                                  \
        sn::detail::enum_table<ENUM, sn::detail::string_enum_table_base<table_spec>>(to_string_map, from_string_map);
