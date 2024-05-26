#include "std_qstring_enum_table.h"

#include <cassert>
#include <utility>

namespace sn::detail {

std_qstring_enum_table_base::std_qstring_enum_table_base(case_sensitivity mode, std::span<const std::pair<std::uint64_t, std::string_view>> pairs) {
    for (const auto &[value, name] : pairs) {
        QString qname = QString::fromUtf8(name.data(), name.size());

        if (!to_string_map.contains(value))
            to_string_map.emplace(value, qname);

        if (mode == case_sensitive) {
            assert(!from_string_map.contains(qname));
            from_string_map.emplace(qname, value);
        } else {
            // Note that we're using the same code here to convert strings to lowercase as what's used in
            // universal_enum_table::from_string. Symmetry is important.
            lowercase_buffer<std_qstring_enum_table_traits> qname_lower(qname);

            assert(!from_string_map.contains(qname_lower.string_view()));
            from_string_map.emplace(qname_lower.string_view().toString(), value);
        }
    }
}

} // namespace sn::detail

