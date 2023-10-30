#pragma once

#include <string_view>

#if !SN_USE_TYPEID_TYPE_NAME
#   if defined(_MSC_VER)
#       define SN_USE_MSVC_TYPE_NAME 1
#   elif defined(__clang__)
#       define SN_USE_CLANG_TYPE_NAME 1
#   elif defined(__GNUC__)
#       define SN_USE_GCC_TYPE_NAME 1
#   else
#       define SN_USE_TYPEID_TYPE_NAME 1
#   endif
#endif

#if SN_USE_TYPEID_TYPE_NAME
#   include <typeinfo>
#else
#   include <array>
#   include <type_traits>
#endif

namespace sn::detail {

#if SN_USE_TYPEID_TYPE_NAME
template<class T>
std::string_view type_name_impl() noexcept {
    return typeid(T).name();
}
#else // SN_USE_TYPEID_TYPE_NAME
template <std::size_t... indices>
constexpr auto to_array(std::string_view str, std::index_sequence<indices...>) noexcept {
    return std::array{str[indices]...}; // No null terminator here, so don't depend on it.
}

template <class T>
constexpr auto type_name_array() noexcept {
#   if SN_USE_CLANG_TYPE_NAME
    constexpr std::string_view prefix   = "[T = ";
    constexpr std::string_view suffix   = "]";
    constexpr std::string_view function = __PRETTY_FUNCTION__;
#   elif SN_USE_GCC_TYPE_NAME
    constexpr std::string_view prefix   = "with T = ";
    constexpr std::string_view suffix   = "]";
    constexpr std::string_view function = __PRETTY_FUNCTION__;
#   elif SN_USE_MSVC_TYPE_NAME
    constexpr std::string_view prefix   = "type_name_array<";
    constexpr std::string_view suffix   = ">(void)";
    constexpr std::string_view function = __FUNCSIG__;
#   else
#       error Unsupported compiler
#   endif

    constexpr std::size_t start_pos = function.find(prefix);
    static_assert(start_pos != std::string_view::npos);

    constexpr std::size_t end_pos = function.rfind(suffix);
    static_assert(end_pos != std::string_view::npos);
    static_assert(start_pos + prefix.size() < end_pos);

    constexpr std::string_view result = function.substr(start_pos + prefix.size(), end_pos - start_pos - prefix.size());
    return to_array(result, std::make_index_sequence<result.size()>());
}

template <class T>
struct type_name_holder {
    static inline constexpr auto value = type_name_array<T>();
};

template <class T>
std::string_view type_name_impl() noexcept {
    return {type_name_holder<T>::value.data(), type_name_holder<T>::value.size()};
}
#endif // SN_USE_TYPEID_TYPE_NAME

} // sn::detail


namespace sn::builtins {

template<class T>
std::string_view type_name(std::type_identity<T>) noexcept {
    return sn::detail::type_name_impl<T>();
}

} // namespace sn::builtins
