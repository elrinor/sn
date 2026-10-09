#pragma once

#include <array>
#include <initializer_list>
#include <string>
#include <string_view>
#include <type_traits>

#include "sn/core/type_name_fwd.h"

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

/**
 * Normalizes a type name as printed by the compiler, so that it's the same on all compilers:
 * - Drops spaces that don't separate words.
 * - Spells the anonymous namespace as `(anonymous namespace)`.
 * - Moves `const` and `volatile` in front of the type they apply to, e.g. `int const` becomes `const int`.
 * - On MSVC, drops `class`, `struct`, `enum` and `union` in front of type names.
 *
 * @param name                          Type name as printed by the compiler.
 * @param out                           Output buffer, or `nullptr` to only compute the size.
 * @return                              Size of the normalized type name.
 */
consteval std::size_t normalize_type_name(std::string_view name, char *out) {
    // Bytes >= 0x80 are parts of UTF-8 identifiers.
    auto is_identifier = [](char c) {
        return (c >= '0' && c <= '9') || c == '_' || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || static_cast<unsigned char>(c) >= 0x80;
    };

    std::size_t size = 0;
    char last = '\0';
    bool last_is_qualifier = false; // Whether the last word is a qualifier that stays in place, e.g. in "const int".
    auto append_char = [&](char c) {
        if (out)
            out[size] = c;
        size++;
        last = c;
        last_is_qualifier = false;
    };
    auto append = [&](std::string_view chars) {
        for (char c : chars)
            append_char(c);
    };

    // Returns the qualifier that starts at pos, if it's a whole word.
    auto qualifier_at = [&](std::size_t pos) -> std::string_view {
        if (pos >= name.size() || (name[pos] != 'c' && name[pos] != 'v') || (pos > 0 && is_identifier(name[pos - 1])))
            return {};
        for (std::string_view qualifier : {"const", "volatile"})
            if (name.substr(pos).starts_with(qualifier) && (pos + qualifier.size() == name.size() || !is_identifier(name[pos + qualifier.size()])))
                return qualifier;
        return {};
    };

    // A qualifier that comes right after a type name or a template argument list applies to that type, e.g. in
    // "int const" or "X<int> const". A qualifier after "*" applies to the pointer and stays where it is.
    auto is_after_type = [&] {
        return (is_identifier(last) && !last_is_qualifier) || last == '>';
    };

    // Note that this runs at compile time, and long type names can hit the constexpr step limit. So we only look for
    // longer patterns at characters that can start them.
    for (std::size_t i = 0; i < name.size(); i++) {
        char c = name[i];

        if (c == 'c' || c == 'v') {
            std::string_view qualifier = qualifier_at(i);
            if (!qualifier.empty() && !is_after_type()) {
                append(qualifier);
                last_is_qualifier = true;
                i += qualifier.size() - 1;
                continue;
            }

            if (!qualifier.empty()) {
                // Insert the qualifier before the type, which starts after the closest unmatched '<' or '(', or ','.
                std::size_t insert_size = qualifier.size() + 1;
                if (out) {
                    std::size_t start = size;
                    for (std::size_t depth = 0; start > 0; start--) {
                        char prev = out[start - 1];
                        if (prev == '>' || prev == ')') {
                            depth++;
                        } else if (prev == '<' || prev == '(' || prev == ',') {
                            if (depth == 0)
                                break;
                            if (prev != ',')
                                depth--;
                        }
                    }

                    // Keep "const volatile" in this order, like clang and GCC.
                    if (std::string_view(out + start, size - start).starts_with("const "))
                        start += 6;

                    for (std::size_t pos = size; pos > start; pos--)
                        out[pos - 1 + insert_size] = out[pos - 1];
                    for (std::size_t pos = 0; pos < qualifier.size(); pos++)
                        out[start + pos] = qualifier[pos];
                    out[start + qualifier.size()] = ' ';
                }
                size += insert_size;
                i += qualifier.size() - 1;
                continue;
            }
        }

        // Clang, GCC and MSVC spellings, in this order. We use clang's.
        if (c == '(' || c == '{' || c == '`') {
            std::size_t anonymous_size = 0;
            for (std::string_view anonymous : {"(anonymous namespace)", "{anonymous}", "`anonymous-namespace'"})
                if (name.substr(i).starts_with(anonymous))
                    anonymous_size = anonymous.size();
            if (anonymous_size != 0) {
                if (is_identifier(last))
                    append(" "); // The space before it was dropped below, e.g. in "const (anonymous namespace)::X".
                append("(anonymous namespace)");
                i += anonymous_size - 1;
                continue;
            }
        }

#if SN_USE_MSVC_TYPE_NAME
        // MSVC writes "class ns::X", "enum ns::Y", etc. Drop the keywords.
        if (is_identifier(c) && (i == 0 || !is_identifier(name[i - 1]))) {
            std::size_t keyword_size = 0;
            for (std::string_view keyword : {"class ", "struct ", "enum ", "union "})
                if (name.substr(i).starts_with(keyword))
                    keyword_size = keyword.size();
            if (keyword_size != 0) {
                i += keyword_size - 1;
                continue;
            }
        }
#endif

        // Keep spaces between words, e.g. in "unsigned char", and drop the rest, e.g. in "some_class *". Also drop the
        // space before a qualifier that we're going to move.
        if (c == ' ') {
            bool between_words = is_identifier(last) && i + 1 < name.size() && is_identifier(name[i + 1]);
            if (!between_words || (is_after_type() && !qualifier_at(i + 1).empty()))
                continue;
        }

        append_char(c);
    }

    return size;
}

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

    std::array<char, normalize_type_name(type_name, nullptr)> result = {{}};
    normalize_type_name(type_name, result.data());
    return result;
}

template <class T>
struct type_name_holder {
    static inline constexpr auto value = type_name_static_string<T>();
};

template <class T>
constexpr std::string_view type_name_impl() noexcept {
    return {type_name_holder<T>::value.data(), type_name_holder<T>::value.size()};
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

SN_DEFINE_TYPE_NAME(bool, "bool")
SN_DEFINE_TYPE_NAME(short, "short")
SN_DEFINE_TYPE_NAME(unsigned short, "unsigned short")
SN_DEFINE_TYPE_NAME(int, "int")
SN_DEFINE_TYPE_NAME(unsigned int, "unsigned int")
SN_DEFINE_TYPE_NAME(long, "long")
SN_DEFINE_TYPE_NAME(unsigned long, "unsigned long")
SN_DEFINE_TYPE_NAME(long long, "long long")
SN_DEFINE_TYPE_NAME(unsigned long long, "unsigned long long")
SN_DEFINE_TYPE_NAME(float, "float")
SN_DEFINE_TYPE_NAME(double, "double")
SN_DEFINE_TYPE_NAME(long double, "long double")

SN_DEFINE_TYPE_NAME(char, "char")
SN_DEFINE_TYPE_NAME(unsigned char, "unsigned char")
SN_DEFINE_TYPE_NAME(signed char, "signed char")

SN_DEFINE_TYPE_NAME(std::string, "std::string")
SN_DEFINE_TYPE_NAME(std::string_view, "std::string_view")
SN_DEFINE_TYPE_NAME(std::wstring, "std::wstring")
SN_DEFINE_TYPE_NAME(std::wstring_view, "std::wstring_view")
SN_DEFINE_TYPE_NAME(std::u8string, "std::u8string")
SN_DEFINE_TYPE_NAME(std::u8string_view, "std::u8string_view")
SN_DEFINE_TYPE_NAME(std::u16string, "std::u16string")
SN_DEFINE_TYPE_NAME(std::u16string_view, "std::u16string_view")
SN_DEFINE_TYPE_NAME(std::u32string, "std::u32string")
SN_DEFINE_TYPE_NAME(std::u32string_view, "std::u32string_view")

} // namespace sn::detail::builtins


