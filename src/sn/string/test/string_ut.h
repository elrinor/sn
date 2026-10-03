#pragma once

#include <string>
#include <string_view>

#include "sn/string/string.h"

namespace sn::detail {

struct string_ops {
    template<class T, class... Tags>
    [[nodiscard]] bool try_to(const T &src, std::string *dst, Tags... tags) const noexcept {
        return sn::try_to_string(src, dst, tags...);
    }

    template<class T, class... Tags>
    void to(const T &src, std::string *dst, Tags... tags) const {
        sn::to_string(src, dst, tags...);
    }

    template<class T, class... Tags>
    [[nodiscard]] bool try_from(std::string_view src, T *dst, Tags... tags) const noexcept {
        return sn::try_from_string(src, dst, tags...);
    }

    template<class T, class... Tags>
    void from(std::string_view src, T *dst, Tags... tags) const {
        sn::from_string(src, dst, tags...);
    }
};

} // namespace sn::detail
