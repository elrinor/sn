#include "std_enum_table.h"

#include <cassert>
#include <array>
#include <memory>

#include "sn/workaround/make_unique_for_overwrite.h"
#include "sn/string/string.h"

#include "ascii_functions.h"
#include "small_buffer.h"
#include "enum_exceptions.h"

namespace sn::detail {

void universal_std_enum_table::to_string(std::uint64_t src, std::string *dst) const {
    if (try_to_string(src, dst))
        return;

    if (_is_signed) {
        // This static_cast relies on implementation-defined behavior, but it's symmetric to what we have in the
        // constructor, so that's OK.
        throw_enum_to_string_error(_type_name, sn::to_string(static_cast<std::int64_t>(src)));
    } else {
        throw_enum_to_string_error(_type_name, sn::to_string(src));
    }
}

template<case_sensitivity mode>
inline std::uint64_t universal_std_enum_table::do_from_string(std::string_view src) const {
    assert(_mode == mode);

    auto run = [&] (std::string_view src) {
        auto pos = _from_string_map.find(src);
        if (pos == _from_string_map.end())
            throw_enum_from_string_error(_type_name, src);
        return pos->second;
    };

    if constexpr (mode == case_insensitive) {
        small_buffer<char, SN_MAX_SMALL_BUFFER_SIZE> buffer(src.size());
        return run(sn::detail::to_lower_ascii(src, buffer.data()));
    } else {
        return run(src);
    }
}

std::uint64_t universal_std_enum_table::from_string(std::string_view src) const {
    return do_from_string<case_sensitive>(src);
}

std::uint64_t universal_std_enum_table::from_string_ci(std::string_view src) const {
    return do_from_string<case_insensitive>(src);
}

bool universal_std_enum_table::try_to_string(std::uint64_t src, std::string *dst) const noexcept {
    auto pos = _to_string_map.find(src);
    if (pos == _to_string_map.end())
        return false;
    *dst = pos->second;
    return true;
}

template<case_sensitivity mode>
inline universal_std_enum_table::try_from_string_result universal_std_enum_table::do_try_from_string(std::string_view src) const noexcept {
    assert(_mode == mode);

    auto run = [&] (std::string_view src) -> universal_std_enum_table::try_from_string_result {
        auto pos = _from_string_map.find(src);
        if (pos == _from_string_map.end())
            return {0, false};
        return {pos->second, true};
    };

    if constexpr (mode == case_insensitive) {
        small_buffer<char, SN_MAX_SMALL_BUFFER_SIZE> buffer(src.size());
        return run(sn::detail::to_lower_ascii(src, buffer.data()));
    } else {
        return run(src);
    }
}

universal_std_enum_table::try_from_string_result universal_std_enum_table::try_from_string(std::string_view src) const noexcept {
    return do_try_from_string<case_sensitive>(src);
}

universal_std_enum_table::try_from_string_result universal_std_enum_table::try_from_string_ci(std::string_view src) const noexcept {
    return do_try_from_string<case_insensitive>(src);
}

void universal_std_enum_table::insert(std::uint64_t value, std::string_view name) {
    if (!_to_string_map.contains(value))
        _to_string_map.emplace(value, name);

    if (_mode == case_sensitive) {
        assert(!_from_string_map.contains(name));
        _from_string_map.emplace(name, value);
    } else {
        std::string lower_name(name.size(), '\0');
        sn::detail::to_lower_ascii(name, lower_name.data());
        assert(!_from_string_map.contains(lower_name));
        _from_string_map.emplace(std::move(lower_name), value);
    }
}

} // namespace sn::detail
