#pragma once

#include <string>
#include <string_view>

#include "sn/detail/test/test_suite.h"

#include "string.h"

namespace sn::detail {
template<>
struct test_stringifier<std::string> {
    std::string operator()(std::string_view s) const {
        return std::string(s);
    }
};

template<class T>
struct test_serializer<T, std::string> {
    template<class... Tags>
    [[nodiscard]] bool try_to(const T &src, std::string *dst, Tags... tags) noexcept {
        return sn::try_to_string(src, dst, tags...);
    }

    template<class... Tags>
    [[nodiscard]] std::string to(const T &src, Tags... tags) {
        return sn::to_string(src, tags...);
    }

    template<class... Tags>
    [[nodiscard]] bool try_from(std::string_view src, T *dst, Tags... tags) noexcept {
        return sn::try_from_string(src, dst, tags...);
    }

    template<class... Tags>
    [[nodiscard]] T from(std::string_view src, Tags... tags) {
        return sn::from_string<T>(src, tags...);
    }
};
} // namespace sn::detail
