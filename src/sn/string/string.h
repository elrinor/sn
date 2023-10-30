#pragma once

#include "string_builtins.h"

namespace sn::detail {

template<class T>
bool do_try_to_string(const T &src, std::string *dst) noexcept {
    using sn::builtins::try_to_string;
    return try_to_string(src, dst);
}

template<class T>
void do_to_string(const T &src, std::string *dst) {
    using sn::builtins::to_string;
    to_string(src, dst);
}

template<class T>
bool do_try_from_string(std::string_view src, T *dst) noexcept {
    using sn::builtins::try_from_string;
    return try_from_string(src, dst);
}

template<class T>
void do_from_string(std::string_view src, T *dst) {
    using sn::builtins::from_string;
    from_string(src, dst);
}

} // namespace sn::detail

namespace sn::niebloids {

struct to_string {
    template<class T>
    std::string operator()(const T &src) const {
        std::string result;
        sn::detail::do_to_string(src, &result);
        return result;
    }
};

template<class T>
struct from_string {
    T operator()(std::string_view src) const {
        T result;
        sn::detail::do_from_string(src, &result);
        return result;
    }
};

} // namespace sn::niebloids

namespace sn {

inline constexpr sn::niebloids::to_string to_string_v;
template<class T>
inline constexpr sn::niebloids::from_string<T> from_string_v;

template<class T>
bool try_to_string(const T &src, std::string *dst) noexcept {
    return sn::detail::do_try_to_string(src, dst);
}

template<class T>
void to_string(const T &src, std::string *dst) {
    sn::detail::do_to_string(src, dst);
}

template<class T>
std::string to_string(const T &src) {
    std::string result;
    sn::detail::do_to_string(src, &result);
    return result;
}

template<class T>
bool try_from_string(std::string_view src, T *dst) noexcept {
    return sn::detail::do_try_from_string(src, dst);
}

template<class T>
void from_string(std::string_view src, T *dst) {
    sn::detail::do_from_string(src, dst);
}

template<class T>
T from_string(std::string_view src) {
    T result;
    sn::detail::do_from_string(src, &result);
    return result;
}

} // namespace sn
