#pragma once

#include <string>
#include <string_view>

#if SN_USE_TYPEID_TYPE_NAME
#   include <typeinfo>
#elif SN_USE_FUNCSIG_TYPE_NAME
#   if defined(_MSC_VER)
#       define SN_USE_MSVC_TYPE_NAME 1
#   elif defined(__clang__)
#       define SN_USE_CLANG_TYPE_NAME 1
#   elif defined(__GNUC__)
#       define SN_USE_GCC_TYPE_NAME 1
#   else
#       define SN_USE_TYPEID_TYPE_NAME 1
#   endif
#   include <array>
#   include <type_traits>
#else
#   error "Type name implementation is not configured"
#endif

namespace sn::detail {

#if SN_USE_TYPEID_TYPE_NAME
template<class T>
std::string_view type_name_impl() noexcept {
    return typeid(T).name();
}
#elif SN_USE_FUNCSIG_TYPE_NAME
template<std::size_t N>
struct static_string {
    std::array<char, N> data = {{}};
    std::size_t size = 0;
};

template <class T>
consteval auto type_name_static_string() noexcept {
#   if SN_USE_CLANG_TYPE_NAME
    constexpr std::string_view prefix   = "[T = ";
    constexpr std::string_view suffix   = "]";
    constexpr std::string_view function = __PRETTY_FUNCTION__;
#   elif SN_USE_GCC_TYPE_NAME
    constexpr std::string_view prefix   = "with T = ";
    constexpr std::string_view suffix   = "]";
    constexpr std::string_view function = __PRETTY_FUNCTION__;
#   elif SN_USE_MSVC_TYPE_NAME
    constexpr std::string_view prefix   = "type_name_static_string<";
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

    constexpr std::string_view type_name = function.substr(start_pos + prefix.size(), end_pos - start_pos - prefix.size());

    static_string<type_name.size()> result = {{}};

#if SN_USE_MSVC_TYPE_NAME
    auto isIdentifier = [](char c) constexpr {
        return (c >= '0' && c <= '9') || c == '_' || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
    };
#endif

    for (std::size_t i = 0; i < type_name.size(); i++) {
        if (type_name[i] == ' ')
            continue;

#if SN_USE_MSVC_TYPE_NAME
        if (type_name.substr(i).starts_with("class ") && (i == 0 || !isIdentifier(type_name[i - 1]))) {
            i += 5;
            continue; // Drop "class ".
        }
        if (type_name.substr(i).starts_with("struct ") && (i == 0 || !isIdentifier(type_name[i - 1]))) {
            i += 6;
            continue; // Drop "struct ".
        }
#endif

        result.data[result.size++] = type_name[i];
    }

    return result;
}

template <class T>
struct type_name_holder {
    static inline constexpr auto value = type_name_static_string<T>();
};

template <class T>
std::string_view type_name_impl() noexcept {
    return {type_name_holder<T>::value.data.data(), type_name_holder<T>::value.size};
}
#endif

} // namespace sn::detail


namespace sn::builtins {

template<class T>
[[nodiscard]] std::string_view type_name(std::type_identity<T>) noexcept {
    return sn::detail::type_name_impl<T>();
}

#define SN_DEFINE_BUILTIN_TYPE_NAME(T, NAME)                                                                            \
[[nodiscard]] std::string_view type_name(std::type_identity<T>) noexcept {                                              \
    return NAME;                                                                                                        \
}

SN_DEFINE_BUILTIN_TYPE_NAME(bool, "bool")
SN_DEFINE_BUILTIN_TYPE_NAME(short, "short")
SN_DEFINE_BUILTIN_TYPE_NAME(unsigned short, "unsigned short")
SN_DEFINE_BUILTIN_TYPE_NAME(int, "int")
SN_DEFINE_BUILTIN_TYPE_NAME(unsigned int, "unsigned int")
SN_DEFINE_BUILTIN_TYPE_NAME(long, "long")
SN_DEFINE_BUILTIN_TYPE_NAME(unsigned long, "unsigned long")
SN_DEFINE_BUILTIN_TYPE_NAME(long long, "long long")
SN_DEFINE_BUILTIN_TYPE_NAME(unsigned long long, "unsigned long long")
SN_DEFINE_BUILTIN_TYPE_NAME(float, "float")
SN_DEFINE_BUILTIN_TYPE_NAME(double, "double")

SN_DEFINE_BUILTIN_TYPE_NAME(char, "char")
SN_DEFINE_BUILTIN_TYPE_NAME(unsigned char, "unsigned char")
SN_DEFINE_BUILTIN_TYPE_NAME(signed char, "signed char")

SN_DEFINE_BUILTIN_TYPE_NAME(std::string, "std::string")
SN_DEFINE_BUILTIN_TYPE_NAME(std::string_view, "std::string_view")
SN_DEFINE_BUILTIN_TYPE_NAME(std::wstring, "std::wstring")
SN_DEFINE_BUILTIN_TYPE_NAME(std::wstring_view, "std::wstring_view")

#undef SN_DEFINE_BUILTIN_TYPE_NAME

} // namespace sn::builtins


