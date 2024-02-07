#include "std_enum_table.h"

#include <cassert>
#include <string>
#include <utility> // For std::move.

#include "sn/detail/ascii/ascii_functions.h"

namespace sn::detail {

std_enum_table_base::std_enum_table_base(case_sensitivity mode, std::span<const std::pair<std::uint64_t, std::string_view>> pairs) {
    for (const auto &[value, name] : pairs) {
        if (!to_string_map.contains(value))
            to_string_map.emplace(value, name);

        if (mode == case_sensitive) {
            assert(!from_string_map.contains(name));
            from_string_map.emplace(name, value);
        } else {
            std::string lower_name(name.size(), '\0');
            sn::detail::to_lower_ascii(name, lower_name.data());
            assert(!from_string_map.contains(lower_name));
            from_string_map.emplace(std::move(lower_name), value);
        }
    }
}

} // namespace sn::detail
