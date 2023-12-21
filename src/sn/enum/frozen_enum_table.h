#pragma once

#include <cstdint>
#include <utility> // For std::pair.
#include <array>
#include <algorithm> // For std::sort.
#include <tuple>
#include <string>
#include <string_view>

#include <frozen/unordered_map.h> // NOLINT
#include <frozen/string.h> // NOLINT

#include "ascii_functions.h"
#include "enum_table.h"

namespace sn::detail {

template<std::size_t size>
using to_string_array = std::array<std::pair<std::uint64_t, std::string_view>, size>;

template<std::size_t size>
using from_string_array = std::array<std::pair<std::string_view, std::uint64_t>, size>;

template<std::size_t size>
consteval std::size_t count_unique_keys(const to_string_array<size> &pairs) {
    if (size <= 1)
        return size;

    auto tmp = pairs;
    std::sort(tmp.begin(), tmp.end());

    std::size_t result = 1;
    for (std::size_t i0 = 0, i1 = 1; i1 < tmp.size(); i0++, i1++)
        result += (tmp[i0].first != tmp[i1].first);
    return result;
}

template<std::size_t unique_size, std::size_t size>
consteval auto make_unique_to_string_array(const to_string_array<size> &pairs) {
    // std::stable_sort is not constexpr, so we have to jump through hoops.
    // Note that frozen brings in <tuple> anyway, so we are free to use it here.
    std::array<std::tuple<std::uint64_t, std::size_t, std::string_view>, size> tmp = {{}};
    for (std::size_t i = 0; i < pairs.size(); i++)
        tmp[i] = {pairs[i].first, i, pairs[i].second};
    std::sort(tmp.begin(), tmp.end());

    to_string_array<unique_size> result = {{}};
    if (size == 0 && unique_size == 0)
        return result;

    // There is no std::transform_unique_copy, and we'd rather not include <ranges> or create temporaries.
    // So we just do a loop.
    result[0] = pairs[0];
    std::size_t ix = 1;
    for (std::size_t i0 = 0, i1 = 1; i1 < tmp.size(); i0++, i1++)
        if (std::get<0>(tmp[i0]) != std::get<0>(tmp[i1]))
            result[ix++] = {std::get<0>(tmp[i1]), std::get<2>(tmp[i1])};

    if (ix != unique_size)
        throw std::runtime_error("Invalid unique_size passed to to_forward_mapping");

    return result;
}

template<std::size_t size>
consteval std::size_t count_total_string_size(const to_string_array<size> &pairs) {
    std::size_t result = 0;
    for (const auto &[_, value] : pairs)
        result += value.size();
    return result;
}

template<std::size_t size, std::size_t buffer_size>
class from_string_storage_ci {
public:
    consteval from_string_storage_ci(const to_string_array<size> &pairs) {
        char *pos = _buffer.data();
        char *end = _buffer.data() + _buffer.size();

        for (std::size_t i = 0; i < pairs.size(); i++) {
            std::size_t string_size = pairs[i].second.size();
            if (pos + string_size > end)
                throw std::runtime_error("Invalid buffer_size used for lowercase_storage");

            _sizes[i] = string_size;
            _keys[i] = pairs[i].first;

            sn::detail::to_lower_ascii(pairs[i].second, pos);
            pos += string_size;
        }
    }

    consteval auto value() const {
        from_string_array<size> result = {{}};

        const char *pos = _buffer.data();
        for (std::size_t i = 0; i < _sizes.size(); i++) {
            result[i] = {{pos, _sizes[i]}, _keys[i]};
            pos += _sizes[i];
        }

        return result;
    }

private:
    // Can't store std::string_view here because then the constructor won't be a constant expression - we can't store
    // pointers to non-static memory inside an object, and at constructor time the current object is not yet in
    // static storage.
    std::array<std::size_t, size> _sizes;
    std::array<std::uint64_t, size> _keys;
    std::array<char, buffer_size> _buffer;
};

template<std::size_t size>
class from_string_storage {
public:
    consteval from_string_storage(const to_string_array<size> &pairs) : _pairs(&pairs) {}

    consteval auto value() const {
        from_string_array<size> result = {{}};
        for (std::size_t i = 0; i < _pairs->size(); i++)
            result[i] = {(*_pairs)[i].second, (*_pairs)[i].first};
        return result;
    }

private:
    const to_string_array<size> *_pairs = nullptr;
};

template<case_sensitivity mode, std::size_t buffer_size, std::size_t size>
consteval auto make_from_string_storage(const to_string_array<size> &pairs) {
    if constexpr (mode == case_sensitive) {
        return from_string_storage<size>(pairs);
    } else {
        return from_string_storage_ci<size, buffer_size>(pairs);
    }
}

template<case_sensitivity mode, std::size_t to_string_size, std::size_t from_string_size>
struct frozen_enum_table_base {
    // TODO(elric): #cpp23 this one should be consteval, but requires P2564 to work, which is in C++23.
    constexpr frozen_enum_table_base(const to_string_array<to_string_size> &to_string_pairs, const from_string_array<from_string_size> &from_string_pairs) :
        to_string_map(to_string_pairs),
        from_string_map(from_string_pairs)
    {}

    frozen::unordered_map<std::uint64_t, frozen::string, to_string_size> to_string_map;
    frozen::unordered_map<frozen::string, std::uint64_t, from_string_size> from_string_map;
};

} // namespace sn::detail

#define _SN_DEFINE_ENUM_STRING_TABLE(TABLE_NAME, ENUM, CASE_SENSITIVITY, REFLECTION_ARG)                                \
    static constexpr auto type_erased_pairs = sn::detail::type_erase_enum_reflection(REFLECTION_ARG);                   \
    static constexpr auto total_key_count = type_erased_pairs.size();                                                   \
    static constexpr auto unique_key_count = sn::detail::count_unique_keys(type_erased_pairs);                          \
    static constexpr auto total_string_size = sn::detail::count_total_string_size(type_erased_pairs);                   \
    static constexpr auto from_string_storage = sn::detail::make_from_string_storage<CASE_SENSITIVITY, total_string_size>(type_erased_pairs); \
    static constexpr auto TABLE_NAME =                                                                                  \
        sn::detail::enum_table<ENUM, CASE_SENSITIVITY, sn::detail::frozen_enum_table_base<CASE_SENSITIVITY, unique_key_count, total_key_count>>( \
            sn::detail::make_unique_to_string_array<unique_key_count>(type_erased_pairs),                               \
            from_string_storage.value());
