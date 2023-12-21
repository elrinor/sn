#pragma once

#include <cstdint>
#include <utility> // For std::pair.
#include <functional> // For std::equal_to.
#include <string>
#include <string_view>
#include <span>
#include <unordered_map>

#include "enum_table.h"

namespace sn::detail {

struct std_enum_table_base {
    template<class T>
    struct heterogeneous_hash : std::hash<T> {
        using is_transparent = void;
    };

    std_enum_table_base(case_sensitivity mode, std::span<const std::pair<std::uint64_t, std::string_view>> pairs);

    std::unordered_map<std::uint64_t, std::string> to_string_map;
    std::unordered_map<std::string, std::uint64_t, heterogeneous_hash<std::string_view>, std::equal_to<>> from_string_map;
};

} // namespace sn::detail

#define _SN_DEFINE_ENUM_STRING_TABLE(TABLE_NAME, ENUM, CASE_SENSITIVITY, REFLECTION_ARG)                                \
    static inline const auto TABLE_NAME = sn::detail::enum_table<ENUM, CASE_SENSITIVITY, sn::detail::std_enum_table_base>( \
        CASE_SENSITIVITY, sn::detail::type_erase_enum_reflection(REFLECTION_ARG));
