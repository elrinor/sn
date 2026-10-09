#pragma once

#include <array>
#include <cstddef>
#include <initializer_list>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

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

template<std::size_t capacity>
struct type_name_buffer {
    std::array<char, capacity> data = {{}};
    std::size_t size = 0;

    [[nodiscard]] constexpr std::string_view view() const {
        return {data.data(), size};
    }
};

/**
 * Normalizes type names as printed by the compiler, so that they're the same on all compilers:
 * - Drops `class`, `struct`, `enum` and `union` in front of type names. MSVC writes these.
 * - Spells the anonymous namespace as `(anonymous namespace)`. GCC and MSVC spell it differently.
 * - Moves `const` and `volatile` in front of the type they apply to. MSVC writes `int const`.
 * - Drops spaces that don't separate words, e.g. `some_class *` becomes `some_class*`.
 *
 * The input is read as a sequence of words and punctuation characters. Words are identifiers, keywords, numbers, and
 * the anonymous namespace. Spaces are dropped, and a single space is put back between consecutive words.
 *
 * Note that this runs at compile time for every type name, and long type names can hit the compiler's constexpr step
 * limit. This is why the output is a plain buffer and not a `std::string`.
 */
class type_name_normalizer {
public:
    /**
     * @tparam capacity                 Capacity of the output buffer. A normalized name is at most twice as long as
     *                                  the original, the worst case being GCC's `{anonymous}`.
     * @param name                      Type name as printed by the compiler.
     * @return                          Normalized type name.
     */
    template<std::size_t capacity>
    static consteval type_name_buffer<capacity> normalize(std::string_view name) {
        type_name_buffer<capacity> result;
        type_name_normalizer normalizer(name, result.data.data());
        while (normalizer._pos < name.size())
            normalizer.process_next();
        result.size = normalizer._size;
        return result;
    }

private:
    // What the output currently ends with.
    enum class ending {
        punctuation,    // E.g. "<" or "*", or nothing at all.
        name,           // A word that's part of a type name, e.g. "int" or "ns".
        qualifier,      // A "const" or "volatile" that applies to what follows, e.g. in "const int".
        template_args,  // A closing ">".
    };

    consteval type_name_normalizer(std::string_view name, char *out) : _name(name), _out(out) {}

    consteval void process_next() {
        char c = _name[_pos];
        if (c == ' ') {
            _pos++;
        } else if (std::string_view word = read_word(); !word.empty()) {
            process_word(word);
        } else {
            process_punctuation(c);
            _pos++;
        }
    }

    consteval void process_word(std::string_view word) {
        // Most words are type names or numbers, so we check the first character before comparing strings.
        char first = word[0];
        if ((first == 'c' || first == 's' || first == 'e' || first == 'u') && (word == "class" || word == "struct" || word == "enum" || word == "union"))
            return;

        // A qualifier right after a type applies to that type, e.g. in "int const" or "X<int> const". A qualifier
        // after "*" applies to the pointer, and a qualifier after ")" applies to the function, so these stay in place.
        bool is_qualifier = (first == 'c' || first == 'v') && (word == "const" || word == "volatile");
        if (is_qualifier && (_ending == ending::name || _ending == ending::template_args)) {
            move_to_type_start(word);
            return; // The output still ends with the same type.
        }

        if (_ending == ending::name || _ending == ending::qualifier)
            append(" ");
        append(word);
        _ending = is_qualifier ? ending::qualifier : ending::name;
    }

    consteval void process_punctuation(char c) {
        if (c == '<' || c == '(') {
            _type_starts.push_back(_size + 1);
        } else if ((c == '>' || c == ')') && !_type_starts.empty()) {
            _type_starts.pop_back();
        } else if (c == ',' && !_type_starts.empty()) {
            _type_starts.back() = _size + 1;
        }

        append({&c, 1});
        _ending = c == '>' ? ending::template_args : ending::punctuation;
    }

    consteval void move_to_type_start(std::string_view qualifier) {
        std::size_t pos = _type_starts.empty() ? 0 : _type_starts.back();

        // Keep "const volatile" in this order, like clang and GCC.
        if (qualifier == "volatile" && std::string_view(_out + pos, _size - pos).starts_with("const "))
            pos += 6;

        insert(pos, " ");
        insert(pos, qualifier);
    }

    consteval void append(std::string_view chars) {
        for (char c : chars)
            _out[_size++] = c;
    }

    consteval void insert(std::size_t pos, std::string_view chars) {
        for (std::size_t i = _size; i > pos; i--)
            _out[i - 1 + chars.size()] = _out[i - 1];
        for (std::size_t i = 0; i < chars.size(); i++)
            _out[pos + i] = chars[i];
        _size += chars.size();
    }

    // Reads the word at the current position, returns an empty string if there is none. The anonymous namespace is
    // returned in clang's spelling.
    consteval std::string_view read_word() {
        std::string_view rest = _name.substr(_pos);

        if (rest[0] == '(' || rest[0] == '{' || rest[0] == '`') {
            // Clang, GCC and MSVC spellings, in this order. MSVC uses both of its spellings, the one with a dash at the
            // top level, and the one with a space inside template arguments.
            for (std::string_view anonymous : {"(anonymous namespace)", "{anonymous}", "`anonymous-namespace'", "`anonymous namespace'"}) {
                if (rest.starts_with(anonymous)) {
                    _pos += anonymous.size();
                    return "(anonymous namespace)";
                }
            }
            return {};
        }

        std::size_t size = 0;
        while (size < rest.size() && is_identifier(rest[size]))
            size++;
        _pos += size;
        return rest.substr(0, size);
    }

    static consteval bool is_identifier(char c) {
        // Bytes >= 0x80 are parts of UTF-8 identifiers.
        return (c >= '0' && c <= '9') || c == '_' || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || static_cast<unsigned char>(c) >= 0x80;
    }

    std::string_view _name;
    std::size_t _pos = 0;
    char *_out = nullptr;
    std::size_t _size = 0;
    ending _ending = ending::punctuation;
    std::vector<std::size_t> _type_starts; // Where the innermost types inside the open "<" and "(" start in the output.
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

    constexpr auto normalized = type_name_normalizer::normalize<type_name.size() * 2>(type_name);

    std::array<char, normalized.size> result = {{}};
    for (std::size_t i = 0; i < normalized.size; i++)
        result[i] = normalized.data[i];
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
