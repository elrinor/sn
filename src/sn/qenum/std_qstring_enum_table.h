#pragma once

#include <cstdint>
#include <utility> // For std::pair.
#include <functional> // For std::equal_to.
#include <span>
#include <unordered_map>
#include <string>

#include <QtCore/QString> // NOLINT
#include <QtCore/QStringView> // NOLINT
#include <QtCore/QHashFunctions> // NOLINT

#include "sn/detail/enum_table/enum_table.h"

#include "ascii_qstring_functions.h"

namespace sn::detail {

struct std_qstring_enum_table_traits {
    using string_type = QString;
    using string_view_type = QStringView;

    static void assign(const QString &src, QString *dst) {
        *dst = src;
    }

    static std::size_t to_lower_size(QStringView s) {
        return s.size();
    }

    static QStringView to_lower(QStringView s, QChar *buffer) {
        return to_lower_ascii(s, buffer);
    }

    static std::string to_std(QStringView s) {
        return s.toUtf8().toStdString();
    }
};

struct std_qstring_enum_table_base {
    template<class T>
    struct heterogeneous_hash : std::hash<T> {
        using is_transparent = void;
    };

    std_qstring_enum_table_base(case_sensitivity mode, std::span<const std::pair<std::uint64_t, std::string_view>> pairs);

    std::unordered_map<std::uint64_t, QString> to_string_map;
    std::unordered_map<QString, std::uint64_t, heterogeneous_hash<QStringView>, std::equal_to<>> from_string_map;
};

} // namespace sn::detail

#define _SN_DEFINE_ENUM_QSTRING_TABLE(TABLE_NAME, ENUM, CASE_SENSITIVITY, REFLECTION_ARG)                               \
    static inline const auto TABLE_NAME = sn::detail::enum_table<ENUM, CASE_SENSITIVITY, sn::detail::std_qstring_enum_table_traits, sn::detail::std_qstring_enum_table_base>( \
        CASE_SENSITIVITY, sn::detail::type_erase_enum_reflection(REFLECTION_ARG));
