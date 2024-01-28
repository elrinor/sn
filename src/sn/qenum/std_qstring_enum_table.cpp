#include "std_qstring_enum_table.h"

#include <cassert>

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
            QString lower_name = qname.toLower();
            assert(!from_string_map.contains(lower_name));
            from_string_map.emplace(std::move(lower_name), value);
        }
    }
}

} // namespace sn::detail

