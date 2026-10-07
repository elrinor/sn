#pragma once

#include <optional>
#include <string>
#include <tuple> // For std::apply.
#include <utility> // For std::move.

#include <nlohmann/json.hpp>

#include "sn/core/exception.h"
#include "sn/core/preprocessor.h"
#include "sn/reflection/class_reflection.h"
#include "sn/detail/codegen/tuple_types.h"
#include "sn/njson/detail/njson_exceptions.h"

#include "njson.h"
#include "njson_fwd.h"

namespace sn::detail {

template<class T>
constexpr bool is_optional_v = false;

template<class T>
constexpr bool is_optional_v<std::optional<T>> = true;

template<class Field>
using field_type_t = typename decltype(Field::type)::type;

template<class T, class Field>
[[nodiscard]] bool try_field_to_njson(const T &src, nlohmann::json::object_t *dst, const Field &field) noexcept {
    nlohmann::json *value = &(*dst)[std::string(field.name)];
    return std::apply([&](auto... tags) {
        return sn::try_to_njson(field.getter(src), value, tags...);
    }, field.tags);
}

template<class T, class Field>
void field_to_njson(const T &src, nlohmann::json::object_t *dst, const Field &field) {
    nlohmann::json *value = &(*dst)[std::string(field.name)];
    try {
        std::apply([&](auto... tags) {
            sn::to_njson(field.getter(src), value, tags...);
        }, field.tags);
    } catch (const sn::exception &e) {
        sn::detail::throw_field_to_njson_error<T>(field.name, e.what());
    }
}

template<class T, class Field>
[[nodiscard]] bool try_field_from_njson(const nlohmann::json::object_t &src, T *dst, const Field &field) noexcept {
    using value_type = field_type_t<Field>;

    auto pos = src.find(field.name);
    if (pos == src.end()) {
        if constexpr (is_optional_v<value_type>) {
            field.setter(*dst, value_type());
            return true;
        } else {
            return false;
        }
    }

    value_type value = value_type();
    bool success = std::apply([&](auto... tags) {
        return sn::try_from_njson(pos->second, &value, tags...);
    }, field.tags);
    if (!success)
        return false;

    field.setter(*dst, std::move(value));
    return true;
}

template<class T, class Field>
void field_from_njson(const nlohmann::json::object_t &src, T *dst, const Field &field) {
    using value_type = field_type_t<Field>;

    auto pos = src.find(field.name);
    if (pos == src.end()) {
        if constexpr (is_optional_v<value_type>) {
            field.setter(*dst, value_type());
            return;
        } else {
            sn::detail::throw_missing_field_from_njson_error<T>(field.name);
        }
    }

    value_type value = value_type();
    try {
        std::apply([&](auto... tags) {
            sn::from_njson(pos->second, &value, tags...);
        }, field.tags);
    } catch (const sn::exception &e) {
        sn::detail::throw_field_from_njson_error<T>(field.name, e.what());
    }

    field.setter(*dst, std::move(value));
}

template<class T, class Reflection>
[[nodiscard]] bool try_class_to_njson(const T &src, nlohmann::json *dst, const Reflection &reflection) noexcept {
    *dst = nlohmann::json::object();
    nlohmann::json::object_t *object = dst->get_ptr<nlohmann::json::object_t *>();
    return std::apply([&](const auto &... fields) {
        return (try_field_to_njson(src, object, fields) && ...);
    }, reflection.fields);
}

template<class T, class Reflection>
void class_to_njson(const T &src, nlohmann::json *dst, const Reflection &reflection) {
    *dst = nlohmann::json::object();
    nlohmann::json::object_t *object = dst->get_ptr<nlohmann::json::object_t *>();
    std::apply([&](const auto &... fields) {
        (field_to_njson(src, object, fields), ...);
    }, reflection.fields);
}

template<class T, class Reflection>
[[nodiscard]] bool try_class_from_njson(const nlohmann::json &src, T *dst, const Reflection &reflection) noexcept {
    const nlohmann::json::object_t *object = src.get_ptr<const nlohmann::json::object_t *>();
    if (!object)
        return false;

    return std::apply([&](const auto &... fields) {
        return (try_field_from_njson(*object, dst, fields) && ...);
    }, reflection.fields);
}

template<class T, class Reflection>
void class_from_njson(const nlohmann::json &src, T *dst, const Reflection &reflection) {
    const nlohmann::json::object_t *object = src.get_ptr<const nlohmann::json::object_t *>();
    if (!object)
        sn::detail::throw_from_njson_error<T>(src);

    std::apply([&](const auto &... fields) {
        (field_from_njson(*object, dst, fields), ...);
    }, reflection.fields);
}

} // namespace sn::detail

#define _SN_DEFINE_CLASS_NJSON_FUNCTIONS_I(TYPE, ATTRIBUTES, ... /* TAGS */)                                            \
    [[nodiscard]] ATTRIBUTES bool try_to_njson(const TYPE &src, nlohmann::json *dst __VA_OPT__(,) __VA_ARGS__) noexcept { \
        return sn::detail::try_class_to_njson(src, dst, sn::reflect_class<TYPE> _SN_PP_TUPLE_TYPES_TO_DEFALT_CTORS((__VA_ARGS__))); \
    }                                                                                                                   \
                                                                                                                        \
    ATTRIBUTES void to_njson(const TYPE &src, nlohmann::json *dst __VA_OPT__(,) __VA_ARGS__) {                          \
        sn::detail::class_to_njson(src, dst, sn::reflect_class<TYPE> _SN_PP_TUPLE_TYPES_TO_DEFALT_CTORS((__VA_ARGS__))); \
    }                                                                                                                   \
                                                                                                                        \
    [[nodiscard]] ATTRIBUTES bool try_from_njson(const nlohmann::json &src, TYPE *dst __VA_OPT__(,) __VA_ARGS__) noexcept { \
        return sn::detail::try_class_from_njson(src, dst, sn::reflect_class<TYPE> _SN_PP_TUPLE_TYPES_TO_DEFALT_CTORS((__VA_ARGS__))); \
    }                                                                                                                   \
                                                                                                                        \
    ATTRIBUTES void from_njson(const nlohmann::json &src, TYPE *dst __VA_OPT__(,) __VA_ARGS__) {                        \
        sn::detail::class_from_njson(src, dst, sn::reflect_class<TYPE> _SN_PP_TUPLE_TYPES_TO_DEFALT_CTORS((__VA_ARGS__))); \
    }

/**
 * Generates `nlohmann::json` function definitions for `TYPE` with tag types passed in varargs. The functions are the
 * ones that `SN_DECLARE_NJSON_FUNCTIONS` declares. `TYPE` is serialized as a json object, with a member for each
 * reflected field.
 *
 * Fields are taken from the reflection of `TYPE`, which should be defined with `SN_DEFINE_CLASS_REFLECTION` for the
 * same tag types. Each field is converted with `sn::to_njson` / `sn::from_njson`, with the tags of that field.
 *
 * `from_njson` fails if the json object doesn't have a member for some field, except for `std::optional` fields, which
 * are set to `std::nullopt`. Members that don't match any field are ignored.
 *
 * @param TYPE                          Class to generate `nlohmann::json` function definitions for.
 * @param ...                           Tags, if any.
 * @see SN_DECLARE_NJSON_FUNCTIONS
 * @see SN_DEFINE_CLASS_REFLECTION
 */
#define SN_DEFINE_CLASS_NJSON_FUNCTIONS(TYPE, ... /* TAGS */)                                                           \
    _SN_DEFINE_CLASS_NJSON_FUNCTIONS_I(TYPE, [[]] __VA_OPT__(,) __VA_ARGS__)

/**
 * Same as `SN_DEFINE_CLASS_NJSON_FUNCTIONS`, but defines inline functions. To be used in a header file.
 *
 * @see SN_DEFINE_CLASS_NJSON_FUNCTIONS
 */
#define SN_DEFINE_INLINE_CLASS_NJSON_FUNCTIONS(TYPE, ... /* TAGS */)                                                    \
    _SN_DEFINE_CLASS_NJSON_FUNCTIONS_I(TYPE, inline __VA_OPT__(,) __VA_ARGS__)

/**
 * Same as `SN_DEFINE_CLASS_NJSON_FUNCTIONS`, but defines static functions.
 *
 * @see SN_DEFINE_CLASS_NJSON_FUNCTIONS
 */
#define SN_DEFINE_STATIC_CLASS_NJSON_FUNCTIONS(TYPE, ... /* TAGS */)                                                    \
    _SN_DEFINE_CLASS_NJSON_FUNCTIONS_I(TYPE, static __VA_OPT__(,) __VA_ARGS__)
