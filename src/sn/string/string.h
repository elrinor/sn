#pragma once

#include <string>
#include <string_view>

#include <expected> // For std::unexpected.
#include <utility> // For std::move.

#include "sn/core/error.h"
#include "sn/core/expected.h"
#include "sn/core/tag.h"
#include "sn/string/detail/string_dispatch.h"
#include "sn/string/detail/string_errors.h"

namespace sn {

/**
 * Serializes `src` into `*dst`.
 *
 * This is also the signature of the extension point that you implement for your types, so it's what you call when
 * serializing nested values.
 *
 * @param src                           Value to serialize.
 * @param dst                           Output. Unspecified on failure.
 * @param err                           Error output, can be `nullptr`. Written only on failure, so a single
 *                                      `sn::error` can be reused across calls.
 * @param tags                          Tags, if any.
 * @return                              Whether serialization succeeded.
 */
template<class T, sn::concepts::tag... Tags>
[[nodiscard]] bool to_string(const T &src, std::string *dst, sn::error *err, Tags... tags) {
    return sn::detail::do_to_string(src, dst, err, tags...);
}

/**
 * Serializes `src`.
 *
 * @param src                           Value to serialize.
 * @param tags                          Tags, if any.
 * @return                              Serialized value, or an error.
 */
template<class T, sn::concepts::tag... Tags>
[[nodiscard]] sn::expected<std::string> to_string(const T &src, Tags... tags) {
    std::string result;
    sn::error error;
    if (!sn::detail::do_to_string(src, &result, &error, tags...))
        return std::unexpected(std::move(error));
    return result;
}

/**
 * Deserializes `src` into `*dst`.
 *
 * This is also the signature of the extension point that you implement for your types, so it's what you call when
 * deserializing nested values. Pass `nullptr` as `err` for speculative parsing, it never allocates.
 *
 * @param src                           Value to deserialize.
 * @param dst                           Output. Unspecified on failure.
 * @param err                           Error output, can be `nullptr`. Written only on failure, so a single
 *                                      `sn::error` can be reused across calls.
 * @param tags                          Tags, if any.
 * @return                              Whether deserialization succeeded.
 */
template<class T, sn::concepts::tag... Tags>
[[nodiscard]] bool from_string(std::string_view src, T *dst, sn::error *err, Tags... tags) {
    return sn::detail::do_from_string(src, dst, err, tags...);
}

/**
 * Deserializes `src` as `T`.
 *
 * @param src                           Value to deserialize.
 * @param tags                          Tags, if any.
 * @return                              Deserialized value, or an error.
 */
template<class T, sn::concepts::tag... Tags>
[[nodiscard]] sn::expected<T> from_string(std::string_view src, Tags... tags) {
    T result;
    sn::error error;
    if (!sn::detail::do_from_string(src, &result, &error, tags...))
        return std::unexpected(std::move(error));
    return result;
}

/**
 * Reports that `src` couldn't be deserialized as `T`. To be used in `from_string` extension points, takes the same
 * arguments.
 *
 * Writes "Cannot deserialize '<src>' as '<T>': <reason>" into `*err`, or just "Cannot deserialize '<src>' as '<T>'" if
 * `reason` is empty. Does nothing if `err` is `nullptr`.
 *
 * @param src                           Value that couldn't be deserialized.
 * @param dst                           Output of the extension point, only used to deduce `T`.
 * @param err                           Error output, can be `nullptr`.
 * @param reason                        Why it couldn't be deserialized, can be empty.
 */
template<class T>
void report_from_string_error(std::string_view src, T *dst, sn::error *err, std::string_view reason = {}) {
    sn::detail::report_from_string_error(src, dst, err, reason);
}

} // namespace sn
