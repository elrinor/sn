#pragma once

#include <string>
#include <utility> // For std::to_underlying.

#include <nlohmann/json.hpp>

#include "sn/core/preprocessor.h"
#include "sn/reflection/enum_reflection.h"
#include "sn/detail/codegen/tuple_types.h"
#include "sn/njson/detail/njson_exceptions.h"
#include "sn/string/enum_string.h" // For _SN_DEFINE_ENUM_STRING_TABLE.
#include "sn/string/string.h"

#include "njson.h"
#include "njson_fwd.h"

namespace sn::detail {

template<class T, class Table>
[[nodiscard]] bool try_enum_to_njson(const Table &table, T src, nlohmann::json *dst) noexcept {
    *dst = nlohmann::json::string_t();
    return table.try_to_string(src, dst->get_ptr<nlohmann::json::string_t *>());
}

template<class T, class Table>
void enum_to_njson(const Table &table, T src, nlohmann::json *dst) {
    // Not using table.to_string here because it would report that we can't serialize to string.
    if (!try_enum_to_njson(table, src, dst))
        sn::detail::throw_to_njson_error<T>(sn::to_string(+std::to_underlying(src)));
}

template<class T, class Table>
[[nodiscard]] bool try_enum_from_njson(const Table &table, const nlohmann::json &src, T *dst) noexcept {
    const nlohmann::json::string_t *string = src.get_ptr<const nlohmann::json::string_t *>();
    return string && table.try_from_string(*string, dst);
}

template<class T, class Table>
void enum_from_njson(const Table &table, const nlohmann::json &src, T *dst) {
    if (!try_enum_from_njson(table, src, dst))
        sn::detail::throw_from_njson_error<T>(src);
}

} // namespace sn::detail

#define _SN_DEFINE_ENUM_NJSON_FUNCTIONS_I(ENUM, CASE_SENSITIVITY, ATTRIBUTES, ... /* TAGS */)                           \
    template<class...>                                                                                                  \
    struct _enum_njson_table_container;                                                                                 \
                                                                                                                        \
    template<>                                                                                                          \
    struct _enum_njson_table_container<ENUM __VA_OPT__(,) __VA_ARGS__> {                                                \
        static constexpr auto reflection = sn::reflect_enum<ENUM> _SN_PP_TUPLE_TYPES_TO_DEFALT_CTORS((__VA_ARGS__));    \
        _SN_DEFINE_ENUM_STRING_TABLE(value, ENUM, CASE_SENSITIVITY, reflection)                                         \
    };                                                                                                                  \
                                                                                                                        \
    [[nodiscard]] ATTRIBUTES bool try_to_njson(const ENUM &src, nlohmann::json *dst __VA_OPT__(,) __VA_ARGS__) noexcept { \
        return sn::detail::try_enum_to_njson(_enum_njson_table_container<ENUM __VA_OPT__(,) __VA_ARGS__>::value, src, dst); \
    }                                                                                                                   \
                                                                                                                        \
    ATTRIBUTES void to_njson(const ENUM &src, nlohmann::json *dst __VA_OPT__(,) __VA_ARGS__) {                          \
        sn::detail::enum_to_njson(_enum_njson_table_container<ENUM __VA_OPT__(,) __VA_ARGS__>::value, src, dst);        \
    }                                                                                                                   \
                                                                                                                        \
    [[nodiscard]] ATTRIBUTES bool try_from_njson(const nlohmann::json &src, ENUM *dst __VA_OPT__(,) __VA_ARGS__) noexcept { \
        return sn::detail::try_enum_from_njson(_enum_njson_table_container<ENUM __VA_OPT__(,) __VA_ARGS__>::value, src, dst); \
    }                                                                                                                   \
                                                                                                                        \
    ATTRIBUTES void from_njson(const nlohmann::json &src, ENUM *dst __VA_OPT__(,) __VA_ARGS__) {                        \
        sn::detail::enum_from_njson(_enum_njson_table_container<ENUM __VA_OPT__(,) __VA_ARGS__>::value, src, dst);      \
    }

/**
 * Generates `nlohmann::json` function definitions for `ENUM` with tag types passed in varargs. The functions are the
 * ones that `SN_DECLARE_NJSON_FUNCTIONS` declares. Enum values are serialized as json strings.
 *
 * Conversions are done with the same lookup tables as in `SN_DEFINE_ENUM_STRING_FUNCTIONS`. The tables are built at
 * compile time from the reflection of `ENUM`, which should be defined with `SN_DEFINE_ENUM_REFLECTION` for the same tag
 * types. `std::string` functions for `ENUM` are not required.
 *
 * @param ENUM                          Enum to generate `nlohmann::json` function definitions for.
 * @param CASE_SENSITIVITY              Either `sn::case_sensitive` or `sn::case_insensitive`, this is how strings are
 *                                      matched in `from_njson`.
 * @param ...                           Tags, if any.
 * @see SN_DECLARE_NJSON_FUNCTIONS
 * @see SN_DEFINE_ENUM_REFLECTION
 */
#define SN_DEFINE_ENUM_NJSON_FUNCTIONS(ENUM, CASE_SENSITIVITY, ... /* TAGS */)                                          \
    _SN_DEFINE_ENUM_NJSON_FUNCTIONS_I(ENUM, CASE_SENSITIVITY, [[]] __VA_OPT__(,) __VA_ARGS__)

/**
 * Same as `SN_DEFINE_ENUM_NJSON_FUNCTIONS`, but defines inline functions. To be used in a header file.
 *
 * @see SN_DEFINE_ENUM_NJSON_FUNCTIONS
 */
#define SN_DEFINE_INLINE_ENUM_NJSON_FUNCTIONS(ENUM, CASE_SENSITIVITY, ... /* TAGS */)                                   \
    _SN_DEFINE_ENUM_NJSON_FUNCTIONS_I(ENUM, CASE_SENSITIVITY, inline __VA_OPT__(,) __VA_ARGS__)

/**
 * Same as `SN_DEFINE_ENUM_NJSON_FUNCTIONS`, but defines static functions.
 *
 * @see SN_DEFINE_ENUM_NJSON_FUNCTIONS
 */
#define SN_DEFINE_STATIC_ENUM_NJSON_FUNCTIONS(ENUM, CASE_SENSITIVITY, ... /* TAGS */)                                   \
    _SN_DEFINE_ENUM_NJSON_FUNCTIONS_I(ENUM, CASE_SENSITIVITY, static __VA_OPT__(,) __VA_ARGS__)
