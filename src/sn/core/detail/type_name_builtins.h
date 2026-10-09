#pragma once

#include <array>
#include <string>
#include <string_view>
#include <type_traits>

// Note that clang-cl defines both __clang__ and _MSC_VER, and its __FUNCSIG__ looks like clang's __PRETTY_FUNCTION__.
// So we check for clang first.
#if defined(__clang__)
#   define SN_USE_CLANG_TYPE_NAME 1
#elif defined(__GNUC__)
#   define SN_USE_GCC_TYPE_NAME 1
#elif defined(_MSC_VER)
#   define SN_USE_MSVC_TYPE_NAME 1
#else
#   error "Unsupported compiler, sn::type_name needs __PRETTY_FUNCTION__ or __FUNCSIG__"
#endif

namespace sn::detail {

template<std::size_t N>
struct static_string {
    std::array<char, N> data = {{}};
    std::size_t size = 0;
};

template <class T>
consteval auto type_name_static_string() noexcept {
#if SN_USE_CLANG_TYPE_NAME
    constexpr std::string_view prefix   = "[T = ";
    constexpr std::string_view suffix   = "]";
    constexpr std::string_view function = __PRETTY_FUNCTION__;
#elif SN_USE_GCC_TYPE_NAME
    constexpr std::string_view prefix   = "with T = ";
    constexpr std::string_view suffix   = "]";
    constexpr std::string_view function = __PRETTY_FUNCTION__;
#elif SN_USE_MSVC_TYPE_NAME
    constexpr std::string_view prefix   = "type_name_static_string<";
    constexpr std::string_view suffix   = ">(void)";
    constexpr std::string_view function = __FUNCSIG__;
#endif

    constexpr std::size_t start_pos = function.find(prefix);
    static_assert(start_pos != std::string_view::npos);

    constexpr std::size_t end_pos = function.rfind(suffix);
    static_assert(end_pos != std::string_view::npos);
    static_assert(start_pos + prefix.size() < end_pos);

    constexpr std::string_view type_name = function.substr(start_pos + prefix.size(), end_pos - start_pos - prefix.size());

    auto is_identifier = [](char c) {
        return (c >= '0' && c <= '9') || c == '_' || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
    };

    static_string<type_name.size()> result = {{}};
    for (std::size_t i = 0; i < type_name.size(); i++) {
#if SN_USE_MSVC_TYPE_NAME
        // MSVC writes "class ns::X", "enum ns::Y", etc. Drop the keywords.
        if (i == 0 || !is_identifier(type_name[i - 1])) {
            std::size_t keyword_size = 0;
            for (std::string_view keyword : {"class ", "struct ", "enum ", "union "})
                if (type_name.substr(i).starts_with(keyword))
                    keyword_size = keyword.size();

            if (keyword_size != 0) {
                i += keyword_size - 1;
                continue;
            }
        }
#endif

        // Keep spaces between words, e.g. in "unsigned char" or "(anonymous namespace)", and drop the rest, e.g. in
        // "some_class *".
        if (type_name[i] == ' ') {
            bool after_word = result.size > 0 && is_identifier(result.data[result.size - 1]);
            bool before_word = i + 1 < type_name.size() && is_identifier(type_name[i + 1]);
            if (!after_word || !before_word)
                continue;
        }

        result.data[result.size++] = type_name[i];
    }

    return result;
}

template <class T>
struct type_name_holder {
    static inline constexpr auto value = type_name_static_string<T>();
};

template <class T>
constexpr std::string_view type_name_impl() noexcept {
    return {type_name_holder<T>::value.data.data(), type_name_holder<T>::value.size};
}

} // namespace sn::detail

#undef SN_USE_CLANG_TYPE_NAME
#undef SN_USE_GCC_TYPE_NAME
#undef SN_USE_MSVC_TYPE_NAME


namespace sn::detail::builtins {

template<class T>
[[nodiscard]] constexpr std::string_view type_name(std::type_identity<T>) noexcept {
    return sn::detail::type_name_impl<T>();
}

#define SN_DEFINE_BUILTIN_TYPE_NAME(T, NAME)                                                                            \
[[nodiscard]] constexpr std::string_view type_name(std::type_identity<T>) noexcept {                                    \
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

} // namespace sn::detail::builtins


