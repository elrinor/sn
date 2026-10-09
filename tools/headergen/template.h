#pragma once
@INCLUDES@
#include <expected> // For std::unexpected.
#include <utility> // For std::move.

#include "sn/core/error.h"
#include "sn/core/expected.h"
#include "sn/core/tag.h"
#include "sn/@LOWER@/detail/@LOWER@_dispatch.h"
#include "sn/@LOWER@/detail/@LOWER@_errors.h"

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
[[nodiscard]] bool to_@LOWER@(const T &src, @DST@, sn::error *err, Tags... tags) {
    return sn::detail::do_to_@LOWER@(src, dst, err, tags...);
}

/**
 * Serializes `src`.
 *
 * @param src                           Value to serialize.
 * @param tags                          Tags, if any.
 * @return                              Serialized value, or an error.
 */
template<class T, sn::concepts::tag... Tags>
[[nodiscard]] sn::expected<@TYPE@> to_@LOWER@(const T &src, Tags... tags) {
    @TYPE@ result;
    sn::error error;
    if (!sn::detail::do_to_@LOWER@(src, &result, &error, tags...))
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
[[nodiscard]] bool from_@LOWER@(@SRC@, T *dst, sn::error *err, Tags... tags) {
    return sn::detail::do_from_@LOWER@(src, dst, err, tags...);
}

/**
 * Deserializes `src` as `T`.
 *
 * @param src                           Value to deserialize.
 * @param tags                          Tags, if any.
 * @return                              Deserialized value, or an error.
 */
template<class T, sn::concepts::tag... Tags>
[[nodiscard]] sn::expected<T> from_@LOWER@(@SRC@, Tags... tags) {
    T result;
    sn::error error;
    if (!sn::detail::do_from_@LOWER@(src, &result, &error, tags...))
        return std::unexpected(std::move(error));
    return result;
}

/**
 * Reports that `src` couldn't be deserialized as `T`. To be used in `from_@LOWER@` extension points.
 *
 * Writes "Cannot deserialize '<src>' as '<T>'" into `*err`. Does nothing if `err` is `nullptr`.
 *
 * @param err                           Error output, can be `nullptr`.
 * @param src                           Value that couldn't be deserialized.
 */
template<class T>
void report_from_@LOWER@_error(sn::error *err, @SRC@) {
    sn::detail::report_from_@LOWER@_error<T>(err, src);
}

/**
 * Same as above, but with a reason. Writes "Cannot deserialize '<src>' as '<T>': <reason>" into `*err`.
 *
 * @param err                           Error output, can be `nullptr`.
 * @param src                           Value that couldn't be deserialized.
 * @param reason                        Why it couldn't be deserialized.
 */
template<class T>
void report_from_@LOWER@_error(sn::error *err, @SRC@, std::string_view reason) {
    sn::detail::report_from_@LOWER@_error<T>(err, src, reason);
}

} // namespace sn
