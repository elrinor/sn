#pragma once

#include <string>
#include <string_view>

#include "string.h"

namespace sn {

struct string_callback {
    template<class T, class... Tags>
    [[nodiscard]] bool try_to(const T &src, std::string *dst, Tags... tags) noexcept {
        return sn::try_to_string(src, dst, tags...);
    }

    template<class T, class... Tags>
    [[nodiscard]] std::string to(const T &src, Tags... tags) {
        return sn::to_string(src, tags...);
    }

    template<class T, class... Tags>
    [[nodiscard]] bool try_from(std::string_view src, T *dst, Tags... tags) noexcept {
        return sn::try_from_string(src, dst, tags...);
    }

    template<class T, class... Tags>
    [[nodiscard]] T from(std::string_view src, Tags... tags) {
        return sn::from_string<T>(src, tags...);
    }
};

} // namespace sn
