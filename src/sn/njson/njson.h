#pragma once

#include <nlohmann/json_fwd.hpp>

#include "sn/core/tag.h"
#include "sn/njson/detail/njson_dispatch.h"

namespace sn {

template<class T, sn::concepts::tag... Tags>
[[nodiscard]] bool try_to_njson(const T &src, nlohmann::json *dst, Tags... tags) noexcept {
    return sn::detail::do_try_to_njson(src, dst, tags...);
}

template<class T, sn::concepts::tag... Tags>
void to_njson(const T &src, nlohmann::json *dst, Tags... tags) {
    sn::detail::do_to_njson(src, dst, tags...);
}

template<class T, sn::concepts::tag... Tags>
[[nodiscard]] nlohmann::json to_njson(const T &src, Tags... tags) {
    nlohmann::json result;
    sn::detail::do_to_njson(src, &result, tags...);
    return result;
}

template<class T, sn::concepts::tag... Tags>
[[nodiscard]] bool try_from_njson(const nlohmann::json &src, T *dst, Tags... tags) noexcept {
    return sn::detail::do_try_from_njson(src, dst, tags...);
}

template<class T, sn::concepts::tag... Tags>
void from_njson(const nlohmann::json &src, T *dst, Tags... tags) {
    sn::detail::do_from_njson(src, dst, tags...);
}

template<class T, sn::concepts::tag... Tags>
[[nodiscard]] T from_njson(const nlohmann::json &src, Tags... tags) {
    T result;
    sn::detail::do_from_njson(src, &result, tags...);
    return result;
}

} // namespace sn
