#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include <utility> // For std::pair.

#include "sn/detail/enum_table/enum_string_maps.h"
#include "sn/detail/enum_table/enum_table.h"
#include "sn/detail/enum_table/enum_table_options.h"

namespace sn::detail {

struct string_enum_table_traits {
    using string_type = std::string;
    using string_view_type = std::string_view;

    static void assign(std::string_view src, std::string *dst) {
        dst->assign(src.data(), src.size());
    }

    static std::string_view to_std(std::string_view s) {
        return s;
    }
};

template<enum_table_spec spec, class T, std::size_t size>
[[nodiscard]] consteval enum_to_string_map<spec> make_enum_to_string_map(const std::array<std::pair<T, std::string_view>, size> &reflection) {
    return enum_to_string_map<spec>(collect_to_string_pairs<spec.to_string_count>(reflection), spec.min_value);
}

template<enum_table_spec spec, class T, std::size_t size>
[[nodiscard]] consteval string_to_enum_map<spec> make_string_to_enum_map(const std::array<std::pair<T, std::string_view>, size> &reflection) {
    return string_to_enum_map<spec>(collect_from_string_pairs<spec.from_string_count>(reflection), spec.min_value);
}

/**
 * Base table for `std::string` enum tables, to be used with `universal_enum_table`.
 *
 * Enum-to-string conversions use either a flat table or a perfect hash table, depending on the enum. String-to-enum
 * conversions always use a perfect hash table. An empty string, if it's listed in the reflection, is not stored in
 * the tables and is handled here.
 *
 * @tparam spec                         Table spec, as returned by `make_enum_table_spec`.
 */
template<enum_table_spec spec>
class string_enum_table_base {
public:
    constexpr string_enum_table_base(const enum_to_string_map<spec> &to_string_map, const string_to_enum_map<spec> &from_string_map) :
        _to_string_map(to_string_map),
        _from_string_map(from_string_map)
    {}

    [[nodiscard]] bool find_string(std::uint64_t value, std::string_view *result) const noexcept {
        if constexpr (spec.empty_string_is_primary) {
            if (value == spec.empty_string_value) {
                *result = std::string_view();
                return true;
            }
        }

        return _to_string_map.find(value, result);
    }

    [[nodiscard]] bool find_value(std::string_view string, std::uint64_t *result) const noexcept {
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
    enum_to_string_map<spec> _to_string_map;
    string_to_enum_map<spec> _from_string_map;
};

} // namespace sn::detail

// The two maps below are built as separate constexpr variables, and not right in the constructor of the table, because
// compilers limit the number of steps in a single constant evaluation. This way each of the maps gets its own budget,
// which matters for enums with hundreds of values.

#define _SN_DEFINE_ENUM_STRING_TABLE(TABLE_NAME, ENUM, OPTIONS, REFLECTION_ARG)                                         \
    static constexpr auto table_spec = sn::detail::make_enum_table_spec(REFLECTION_ARG, sn::detail::to_enum_table_options(OPTIONS)); \
    static constexpr auto to_string_map = sn::detail::make_enum_to_string_map<table_spec>(REFLECTION_ARG);              \
    static constexpr auto from_string_map = sn::detail::make_string_to_enum_map<table_spec>(REFLECTION_ARG);            \
    static constexpr auto TABLE_NAME =                                                                                  \
        sn::detail::enum_table<ENUM, sn::detail::string_enum_table_traits, sn::detail::string_enum_table_base<table_spec>>(to_string_map, from_string_map);
