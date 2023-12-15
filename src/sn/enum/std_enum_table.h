#pragma once

#include <cstdint>
#include <array>
#include <utility> // For std::pair.
#include <functional> // For std::equal_to.
#include <string>
#include <string_view>
#include <unordered_map>

#include "sn/core/type_name.h"

#include "enum_fwd.h"

namespace sn::detail {

class universal_std_enum_table {
public:
    template<class T, std::size_t size>
    universal_std_enum_table(case_sensitivity mode, const std::array<std::pair<T, std::string_view>, size> &pairs) : _mode(mode) {
        if constexpr (std::is_enum_v<T>) {
            _is_signed = std::is_signed_v<std::underlying_type_t<T>>;
        } else {
            static_assert(std::is_integral_v<T>);
            _is_signed = std::is_signed_v<T>;
        }

        _type_name = sn::type_name<T>();

        // static_cast below sign-extends negative signed values, this is implementation-defined behavior, and that's OK.
        for (const auto &[value, name] : pairs)
            insert(static_cast<std::uint64_t>(value), name);
    }

    // We're not following our own API conventions here mainly for the sake of a better codegen.
    // Both x86_64 and arm64 ABIs return structs up to 16 bytes in registers, and it makes a lot of sense to use this here.

    struct try_from_string_result {
        std::uint64_t value = 0;
        bool ok = false;
    };

    void to_string(std::uint64_t src, std::string *dst) const;
    [[nodiscard]] std::uint64_t from_string(std::string_view src) const;
    [[nodiscard]] std::uint64_t from_string_ci(std::string_view src) const;
    [[nodiscard]] bool try_to_string(std::uint64_t src, std::string *dst) const noexcept;
    [[nodiscard]] try_from_string_result try_from_string(std::string_view src) const noexcept;
    [[nodiscard]] try_from_string_result try_from_string_ci(std::string_view src) const noexcept;

private:
    void insert(std::uint64_t value, std::string_view name);

    template<case_sensitivity>
    [[nodiscard]] inline std::uint64_t do_from_string(std::string_view src) const;
    template<case_sensitivity>
    [[nodiscard]] inline try_from_string_result do_try_from_string(std::string_view src) const noexcept;

    template<class T>
    struct heterogeneous_hash : std::hash<T> {
        using is_transparent = void;
    };

private:
    case_sensitivity _mode;
    bool _is_signed;
    std::string_view _type_name;
    std::unordered_map<std::uint64_t, std::string> _to_string_map;
    std::unordered_map<std::string, std::uint64_t, heterogeneous_hash<std::string_view>, std::equal_to<>> _from_string_map;
};

template<class T, case_sensitivity mode>
class std_enum_table {
public:
    std_enum_table() = default;

    template<size_t size>
    explicit std_enum_table(const std::array<std::pair<T, std::string_view>, size> &pairs) : _table(mode, pairs) {}

    void to_string(T src, std::string *dst) const {
        _table.to_string(static_cast<std::uint64_t>(src), dst);
    }

    void from_string(std::string_view src, T *dst) const {
        *dst = static_cast<T>(mode == case_sensitive ? _table.from_string(src) : _table.from_string_ci(src));
    }

    [[nodiscard]] bool try_to_string(T src, std::string *dst) const noexcept {
        return _table.try_to_string(static_cast<std::uint64_t>(src), dst);
    }

    [[nodiscard]] bool try_from_string(std::string_view src, T *dst) const noexcept {
        universal_std_enum_table::try_from_string_result result = mode == case_sensitive ? _table.try_from_string(src) : _table.try_from_string_ci(src);
        if (result.ok)
            *dst = static_cast<T>(result.value);
        return result.ok;
    }

private:
    sn::detail::universal_std_enum_table _table;
};

} // namespace sn::detail

