#pragma once

#include <cstdint>
#include <initializer_list>
#include <utility> // For std::pair.
#include <functional> // For std::equal_to.
#include <string>
#include <string_view>
#include <unordered_map>

#include "enum_fwd.h"

namespace sn::detail {

class universal_std_enum_table {
public:
    template<class T>
    universal_std_enum_table(case_sensitivity mode, std::initializer_list<std::pair<T, const char *>> pairs) : _mode(mode) {
        for (const auto &[value, name] : pairs)
            insert(static_cast<std::uint64_t>(value), name); // This static_cast sign-extends negative signed values, which is OK.
    }

    [[nodiscard]] bool try_to_string(std::uint64_t src, std::string *dst) const noexcept;
    [[nodiscard]] bool try_to_string_view(std::uint64_t src, std::string_view *dst) const noexcept;
    [[nodiscard]] bool try_from_string(std::string_view src, std::uint64_t *dst) const noexcept;
    [[nodiscard]] bool try_from_string_ci(std::string_view src, std::uint64_t *dst) const noexcept;

private:
    void insert(std::uint64_t value, std::string_view name);

    template<class T>
    struct heterogeneous_hash : std::hash<T> {
        using is_transparent = void;
    };

private:
    case_sensitivity _mode;
    std::unordered_map<std::uint64_t, std::string> _string_by_enum;
    std::unordered_map<std::string, std::uint64_t, heterogeneous_hash<std::string_view>, std::equal_to<>> _enum_by_string;
};

template<class T, case_sensitivity mode>
class std_enum_table {
public:
    std_enum_table() = default;

    explicit std_enum_table(std::initializer_list<std::pair<T, const char *>> pairs) : _table(mode, pairs) {}

    [[nodiscard]] bool try_to_string(T src, std::string *dst) const noexcept {
        return _table.try_to_string(static_cast<std::uint64_t>(src), dst);
    }

    [[nodiscard]] bool try_to_string_view(T src, std::string_view *dst) const noexcept {
        return _table.try_to_string_view(static_cast<std::uint64_t>(src), dst);
    }

    [[nodiscard]] bool try_from_string(std::string_view src, T *dst) const noexcept {
        std::uint64_t tmp;
        if (mode == case_sensitive ? !_table.try_from_string(src, &tmp) : !_table.try_from_string_ci(src, &tmp))
            return false;
        *dst = static_cast<T>(tmp);
        return true;
    }

private:
    sn::detail::universal_std_enum_table _table;
};

} // namespace sn::detail

