#pragma once

#include <array>
#include <cstddef> // For std::size_t.
#include <map>
#include <optional>
#include <string>
#include <unordered_map>
#include <utility> // For std::move.
#include <vector>

#include <nlohmann/json.hpp>

#include "sn/core/exception.h"
#include "sn/string/string.h"

#include "njson_exceptions.h"
#include "njson_shortcuts.h"

//
// Builtins in this file are for templates, and they need to (de)serialize the template arguments. They can't call
// sn::to_njson, which is declared after the builtins, so they go through sn::detail::njson_dispatcher.
//

namespace sn::detail {

//
// Arrays.
//

template<class T, class Range, class... Tags>
[[nodiscard]] bool try_range_to_njson(const Range &src, nlohmann::json *dst, Tags... tags) noexcept {
    *dst = nlohmann::json::array();
    nlohmann::json::array_t &array = *dst->get_ptr<nlohmann::json::array_t *>();
    array.reserve(src.size());
    for (const T &element : src)
        if (!njson_dispatcher<T, Tags...>::try_to_njson(element, &array.emplace_back(), tags...))
            return false;
    return true;
}

template<class T, class Range, class... Tags>
void range_to_njson(const Range &src, nlohmann::json *dst, Tags... tags) {
    *dst = nlohmann::json::array();
    nlohmann::json::array_t &array = *dst->get_ptr<nlohmann::json::array_t *>();
    array.reserve(src.size());
    for (const T &element : src) {
        try {
            njson_dispatcher<T, Tags...>::to_njson(element, &array.emplace_back(), tags...);
        } catch (const sn::exception &e) {
            throw_element_to_njson_error(array.size() - 1, e.what());
        }
    }
}

template<class T, class Allocator, class... Tags>
[[nodiscard]] bool try_vector_from_njson(const nlohmann::json &src, std::vector<T, Allocator> *dst, Tags... tags) noexcept {
    const nlohmann::json::array_t *array = src.get_ptr<const nlohmann::json::array_t *>();
    if (!array)
        return false;

    // Note that we can't deserialize into dst->back() because of std::vector<bool>.
    dst->clear();
    dst->reserve(array->size());
    for (const nlohmann::json &element : *array) {
        T value = T();
        if (!njson_dispatcher<T, Tags...>::try_from_njson(element, &value, tags...))
            return false;
        dst->push_back(std::move(value));
    }
    return true;
}

template<class T, class Allocator, class... Tags>
void vector_from_njson(const nlohmann::json &src, std::vector<T, Allocator> *dst, Tags... tags) {
    const nlohmann::json::array_t *array = src.get_ptr<const nlohmann::json::array_t *>();
    if (!array)
        throw_from_njson_error<std::vector<T, Allocator>>(src);

    dst->clear();
    dst->reserve(array->size());
    for (const nlohmann::json &element : *array) {
        T value = T();
        try {
            njson_dispatcher<T, Tags...>::from_njson(element, &value, tags...);
        } catch (const sn::exception &e) {
            throw_element_from_njson_error(dst->size(), e.what());
        }
        dst->push_back(std::move(value));
    }
}

template<class T, std::size_t N, class... Tags>
[[nodiscard]] bool try_array_from_njson(const nlohmann::json &src, std::array<T, N> *dst, Tags... tags) noexcept {
    const nlohmann::json::array_t *array = src.get_ptr<const nlohmann::json::array_t *>();
    if (!array || array->size() != N)
        return false;

    for (std::size_t i = 0; i < N; i++)
        if (!njson_dispatcher<T, Tags...>::try_from_njson((*array)[i], &(*dst)[i], tags...))
            return false;
    return true;
}

template<class T, std::size_t N, class... Tags>
void array_from_njson(const nlohmann::json &src, std::array<T, N> *dst, Tags... tags) {
    const nlohmann::json::array_t *array = src.get_ptr<const nlohmann::json::array_t *>();
    if (!array || array->size() != N)
        throw_from_njson_error<std::array<T, N>>(src);

    for (std::size_t i = 0; i < N; i++) {
        try {
            njson_dispatcher<T, Tags...>::from_njson((*array)[i], &(*dst)[i], tags...);
        } catch (const sn::exception &e) {
            throw_element_from_njson_error(i, e.what());
        }
    }
}


//
// Maps. Keys are converted with sn::to_string / sn::from_string, and tags are only applied to values.
//

template<class Map, class... Tags>
[[nodiscard]] bool try_map_to_njson(const Map &src, nlohmann::json *dst, Tags... tags) noexcept {
    using value_type = typename Map::mapped_type;

    *dst = nlohmann::json::object();
    nlohmann::json::object_t &object = *dst->get_ptr<nlohmann::json::object_t *>();
    std::string key;
    for (const auto &[src_key, src_value] : src) {
        if (!sn::try_to_string(src_key, &key))
            return false;
        if (!njson_dispatcher<value_type, Tags...>::try_to_njson(src_value, &object[key], tags...))
            return false;
    }
    return true;
}

template<class Map, class... Tags>
void map_to_njson(const Map &src, nlohmann::json *dst, Tags... tags) {
    using value_type = typename Map::mapped_type;

    *dst = nlohmann::json::object();
    nlohmann::json::object_t &object = *dst->get_ptr<nlohmann::json::object_t *>();
    std::string key;
    for (const auto &[src_key, src_value] : src) {
        sn::to_string(src_key, &key);
        try {
            njson_dispatcher<value_type, Tags...>::to_njson(src_value, &object[key], tags...);
        } catch (const sn::exception &e) {
            throw_member_to_njson_error(key, e.what());
        }
    }
}

template<class Map, class... Tags>
[[nodiscard]] bool try_map_from_njson(const nlohmann::json &src, Map *dst, Tags... tags) noexcept {
    using key_type = typename Map::key_type;
    using value_type = typename Map::mapped_type;

    const nlohmann::json::object_t *object = src.get_ptr<const nlohmann::json::object_t *>();
    if (!object)
        return false;

    dst->clear();
    for (const auto &[src_key, src_value] : *object) {
        key_type key = key_type();
        value_type value = value_type();
        if (!sn::try_from_string(src_key, &key) || !njson_dispatcher<value_type, Tags...>::try_from_njson(src_value, &value, tags...))
            return false;
        if (!dst->emplace(std::move(key), std::move(value)).second)
            return false; // Different strings that map to the same key, e.g. "1" and "01" for an int key.
    }
    return true;
}

template<class Map, class... Tags>
void map_from_njson(const nlohmann::json &src, Map *dst, Tags... tags) {
    using key_type = typename Map::key_type;
    using value_type = typename Map::mapped_type;

    const nlohmann::json::object_t *object = src.get_ptr<const nlohmann::json::object_t *>();
    if (!object)
        throw_from_njson_error<Map>(src);

    dst->clear();
    for (const auto &[src_key, src_value] : *object) {
        key_type key = key_type();
        value_type value = value_type();
        try {
            sn::from_string(src_key, &key);
            njson_dispatcher<value_type, Tags...>::from_njson(src_value, &value, tags...);
        } catch (const sn::exception &e) {
            throw_member_from_njson_error(src_key, e.what());
        }
        if (!dst->emplace(std::move(key), std::move(value)).second)
            throw_member_from_njson_error(src_key, "another member maps to the same key");
    }
}

} // namespace sn::detail

namespace sn::detail::builtins {

//
// Support for std::vector.
//

template<class T, class Allocator, class... Tags>
    requires(sn::detail::njson_dispatcher<T, Tags...>::is_try_to_njsonable())
[[nodiscard]] bool try_to_njson(const std::vector<T, Allocator> &src, nlohmann::json *dst, Tags... tags) noexcept {
    return sn::detail::try_range_to_njson<T>(src, dst, tags...);
}

template<class T, class Allocator, class... Tags>
    requires(sn::detail::njson_dispatcher<T, Tags...>::is_to_njsonable())
void to_njson(const std::vector<T, Allocator> &src, nlohmann::json *dst, Tags... tags) {
    sn::detail::range_to_njson<T>(src, dst, tags...);
}

template<class T, class Allocator, class... Tags>
    requires(sn::detail::njson_dispatcher<T, Tags...>::is_try_from_njsonable())
[[nodiscard]] bool try_from_njson(const nlohmann::json &src, std::vector<T, Allocator> *dst, Tags... tags) noexcept {
    return sn::detail::try_vector_from_njson(src, dst, tags...);
}

template<class T, class Allocator, class... Tags>
    requires(sn::detail::njson_dispatcher<T, Tags...>::is_from_njsonable())
void from_njson(const nlohmann::json &src, std::vector<T, Allocator> *dst, Tags... tags) {
    sn::detail::vector_from_njson(src, dst, tags...);
}


//
// Support for std::array.
//

template<class T, std::size_t N, class... Tags>
    requires(sn::detail::njson_dispatcher<T, Tags...>::is_try_to_njsonable())
[[nodiscard]] bool try_to_njson(const std::array<T, N> &src, nlohmann::json *dst, Tags... tags) noexcept {
    return sn::detail::try_range_to_njson<T>(src, dst, tags...);
}

template<class T, std::size_t N, class... Tags>
    requires(sn::detail::njson_dispatcher<T, Tags...>::is_to_njsonable())
void to_njson(const std::array<T, N> &src, nlohmann::json *dst, Tags... tags) {
    sn::detail::range_to_njson<T>(src, dst, tags...);
}

template<class T, std::size_t N, class... Tags>
    requires(sn::detail::njson_dispatcher<T, Tags...>::is_try_from_njsonable())
[[nodiscard]] bool try_from_njson(const nlohmann::json &src, std::array<T, N> *dst, Tags... tags) noexcept {
    return sn::detail::try_array_from_njson(src, dst, tags...);
}

template<class T, std::size_t N, class... Tags>
    requires(sn::detail::njson_dispatcher<T, Tags...>::is_from_njsonable())
void from_njson(const nlohmann::json &src, std::array<T, N> *dst, Tags... tags) {
    sn::detail::array_from_njson(src, dst, tags...);
}


//
// Support for std::map.
//

template<class Key, class T, class Compare, class Allocator, class... Tags>
    requires(sn::concepts::try_to_stringable<Key> && sn::detail::njson_dispatcher<T, Tags...>::is_try_to_njsonable())
[[nodiscard]] bool try_to_njson(const std::map<Key, T, Compare, Allocator> &src, nlohmann::json *dst, Tags... tags) noexcept {
    return sn::detail::try_map_to_njson(src, dst, tags...);
}

template<class Key, class T, class Compare, class Allocator, class... Tags>
    requires(sn::concepts::to_stringable<Key> && sn::detail::njson_dispatcher<T, Tags...>::is_to_njsonable())
void to_njson(const std::map<Key, T, Compare, Allocator> &src, nlohmann::json *dst, Tags... tags) {
    sn::detail::map_to_njson(src, dst, tags...);
}

template<class Key, class T, class Compare, class Allocator, class... Tags>
    requires(sn::concepts::try_from_stringable<Key> && sn::detail::njson_dispatcher<T, Tags...>::is_try_from_njsonable())
[[nodiscard]] bool try_from_njson(const nlohmann::json &src, std::map<Key, T, Compare, Allocator> *dst, Tags... tags) noexcept {
    return sn::detail::try_map_from_njson(src, dst, tags...);
}

template<class Key, class T, class Compare, class Allocator, class... Tags>
    requires(sn::concepts::from_stringable<Key> && sn::detail::njson_dispatcher<T, Tags...>::is_from_njsonable())
void from_njson(const nlohmann::json &src, std::map<Key, T, Compare, Allocator> *dst, Tags... tags) {
    sn::detail::map_from_njson(src, dst, tags...);
}


//
// Support for std::unordered_map.
//

template<class Key, class T, class Hash, class KeyEqual, class Allocator, class... Tags>
    requires(sn::concepts::try_to_stringable<Key> && sn::detail::njson_dispatcher<T, Tags...>::is_try_to_njsonable())
[[nodiscard]] bool try_to_njson(const std::unordered_map<Key, T, Hash, KeyEqual, Allocator> &src, nlohmann::json *dst, Tags... tags) noexcept {
    return sn::detail::try_map_to_njson(src, dst, tags...);
}

template<class Key, class T, class Hash, class KeyEqual, class Allocator, class... Tags>
    requires(sn::concepts::to_stringable<Key> && sn::detail::njson_dispatcher<T, Tags...>::is_to_njsonable())
void to_njson(const std::unordered_map<Key, T, Hash, KeyEqual, Allocator> &src, nlohmann::json *dst, Tags... tags) {
    sn::detail::map_to_njson(src, dst, tags...);
}

template<class Key, class T, class Hash, class KeyEqual, class Allocator, class... Tags>
    requires(sn::concepts::try_from_stringable<Key> && sn::detail::njson_dispatcher<T, Tags...>::is_try_from_njsonable())
[[nodiscard]] bool try_from_njson(const nlohmann::json &src, std::unordered_map<Key, T, Hash, KeyEqual, Allocator> *dst, Tags... tags) noexcept {
    return sn::detail::try_map_from_njson(src, dst, tags...);
}

template<class Key, class T, class Hash, class KeyEqual, class Allocator, class... Tags>
    requires(sn::concepts::from_stringable<Key> && sn::detail::njson_dispatcher<T, Tags...>::is_from_njsonable())
void from_njson(const nlohmann::json &src, std::unordered_map<Key, T, Hash, KeyEqual, Allocator> *dst, Tags... tags) {
    sn::detail::map_from_njson(src, dst, tags...);
}


//
// Support for std::optional. std::nullopt is null in json.
//

template<class T, class... Tags>
    requires(sn::detail::njson_dispatcher<T, Tags...>::is_try_to_njsonable())
[[nodiscard]] bool try_to_njson(const std::optional<T> &src, nlohmann::json *dst, Tags... tags) noexcept {
    if (!src) {
        *dst = nullptr;
        return true;
    }
    return sn::detail::njson_dispatcher<T, Tags...>::try_to_njson(*src, dst, tags...);
}

template<class T, class... Tags>
    requires(sn::detail::njson_dispatcher<T, Tags...>::is_to_njsonable())
void to_njson(const std::optional<T> &src, nlohmann::json *dst, Tags... tags) {
    if (!src) {
        *dst = nullptr;
        return;
    }
    sn::detail::njson_dispatcher<T, Tags...>::to_njson(*src, dst, tags...);
}

template<class T, class... Tags>
    requires(sn::detail::njson_dispatcher<T, Tags...>::is_try_from_njsonable())
[[nodiscard]] bool try_from_njson(const nlohmann::json &src, std::optional<T> *dst, Tags... tags) noexcept {
    if (src.is_null()) {
        dst->reset();
        return true;
    }
    return sn::detail::njson_dispatcher<T, Tags...>::try_from_njson(src, &dst->emplace(), tags...);
}

template<class T, class... Tags>
    requires(sn::detail::njson_dispatcher<T, Tags...>::is_from_njsonable())
void from_njson(const nlohmann::json &src, std::optional<T> *dst, Tags... tags) {
    if (src.is_null()) {
        dst->reset();
        return;
    }
    sn::detail::njson_dispatcher<T, Tags...>::from_njson(src, &dst->emplace(), tags...);
}

} // namespace sn::detail::builtins
