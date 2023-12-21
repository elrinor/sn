#pragma once

#include <cassert>
#include <array>
#include <utility> // For std::forward, std::pair.
#include <string>
#include <string_view>

#include "sn/core/type_name.h"
#include "sn/string/string.h"

#include "ascii_functions.h"
#include "small_buffer.h"
#include "enum_exceptions.h"
#include "enum_fwd.h"

namespace sn::detail {

template<class T>
constexpr bool is_signed_ex_v = std::is_signed_v<typename std::conditional_t<std::is_enum_v<T>, std::underlying_type<T>, std::type_identity<T>>::type>;

template<class Base>
struct universal_enum_table {
public:
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

    // We're not following our own API conventions here mainly for the sake of a better codegen.
    // Both x86_64 and arm64 ABIs return structs up to 16 bytes in registers, and it makes a lot of sense to use this here.

    struct try_from_string_result {
        std::uint64_t value = 0;
        bool ok = false;
    };

    void to_string(std::uint64_t src, std::string *dst) const {
        auto pos = _base.to_string_map.find(src);
        if (pos != _base.to_string_map.end()) {
            dst->assign(pos->second.data(), pos->second.size());
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
    [[nodiscard]] std::uint64_t from_string(std::string_view src) const {
        assert(_mode == mode);

        auto run = [&] (std::string_view src) {
            auto pos = _base.from_string_map.find(src);
            if (pos == _base.from_string_map.end())
                throw_enum_from_string_error(_type_name(), src);
            return pos->second;
        };

        if constexpr (mode == case_insensitive) {
            small_buffer<char, SN_MAX_SMALL_BUFFER_SIZE> buffer(src.size());
            return run(sn::detail::to_lower_ascii(src, buffer.data()));
        } else {
            return run(src);
        }
    }

    [[nodiscard]] bool try_to_string(std::uint64_t src, std::string *dst) const noexcept {
        auto pos = _base.to_string_map.find(src);
        if (pos != _base.to_string_map.end()) {
            dst->assign(pos->second.data(), pos->second.size());
            return true;
        } else {
            return false;
        }
    }

    template<case_sensitivity mode>
    [[nodiscard]] try_from_string_result try_from_string(std::string_view src) const noexcept {
        assert(_mode == mode);

        auto run = [&] (std::string_view src) -> try_from_string_result {
            auto pos = _base.from_string_map.find(src);
            if (pos != _base.from_string_map.end()) {
                return {pos->second, true};
            } else {
                return {0, false};
            }
        };

        if constexpr (mode == case_insensitive) {
            small_buffer<char, SN_MAX_SMALL_BUFFER_SIZE> buffer(src.size());
            return run(sn::detail::to_lower_ascii(src, buffer.data()));
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

template<class T, case_sensitivity mode, class Base>
class enum_table {
public:
    template<class... Args>
    explicit constexpr enum_table(Args &&... args) : _table(mode, &sn::type_name<T>, is_signed_ex_v<T>, std::forward<Args>(args)...) {}

    void to_string(T src, std::string *dst) const {
        _table.to_string(static_cast<std::uint64_t>(src), dst);
    }

    void from_string(std::string_view src, T *dst) const {
        *dst = static_cast<T>(_table.template from_string<mode>(src));
    }

    [[nodiscard]] bool try_to_string(T src, std::string *dst) const noexcept {
        return _table.try_to_string(static_cast<std::uint64_t>(src), dst);
    }

    [[nodiscard]] bool try_from_string(std::string_view src, T *dst) const noexcept {
        auto result = _table.template try_from_string<mode>(src);
        if (result.ok)
            *dst = static_cast<T>(result.value);
        return result.ok;
    }

private:
    universal_enum_table<Base> _table;
};

template<class T, std::size_t size>
consteval auto type_erase_enum_reflection(const std::array<std::pair<T, std::string_view>, size> &pairs) {
    std::array<std::pair<std::uint64_t, std::string_view>, size> result = {{}};
    for (std::size_t i = 0; i < pairs.size(); i++)
        result[i] = {static_cast<std::uint64_t>(pairs[i].first), pairs[i].second};
    return result;
}

} // namespace sn::detail

