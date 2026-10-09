#pragma once

#include <algorithm> // For std::min.
#include <array>
#include <cstddef>
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

template<std::size_t capacity>
struct type_name_buffer {
    std::array<char, capacity> data = {{}};
    std::size_t size = 0;

    [[nodiscard]] constexpr std::string_view view() const {
        return {data.data(), size};
    }
};

/**
 * Normalizes type names as printed by the compiler, removing the most visible differences between compilers:
 * - Spells the anonymous namespace as `(anonymous namespace)`. GCC and MSVC spell it differently.
 * - Moves `const` and `volatile` in front of the type they apply to. MSVC writes `int const`.
 * - Drops spaces that don't separate words, e.g. `some_class *` becomes `some_class*`.
 * - For MSVC, drops `class`, `struct`, `enum` and `union` in front of type names.
 *
 * Builtin types are not normalized, e.g. GCC writes `long int` where clang writes `long`.
 *
 * The input is read as a sequence of words, punctuation characters, and character and string literals. Words are
 * identifiers, keywords, numbers, and the anonymous namespace. Spaces are dropped, and a single space is put back
 * between consecutive words. Literals are copied as is.
 *
 * Note that this runs at compile time for every type name, and long type names can hit the compiler's constexpr step
 * limit. This is why the output and the stack below are plain buffers. `std::string` and `std::vector` are a lot slower
 * to evaluate.
 */
class type_name_normalizer {
public:
    // This should be private, but MSVC doesn't let normalize() call a private constructor during constant evaluation
    // (C2248), whether the constructor is consteval or constexpr. Use normalize() instead.
    constexpr type_name_normalizer(std::string_view name, bool msvc, char *out, std::size_t *type_starts) :
        _name(name),
        _msvc(msvc),
        _out(out),
        _type_starts(type_starts)
    {}

    /**
     * @tparam capacity                 Capacity of the output buffer. A normalized name is at most twice as long as
     *                                  the original, the worst case being GCC's `{anonymous}`.
     * @param name                      Type name as printed by the compiler.
     * @param msvc                      Whether the name was printed by MSVC.
     * @return                          Normalized type name.
     */
    template<std::size_t capacity>
    static consteval type_name_buffer<capacity> normalize(std::string_view name, bool msvc) {
        type_name_buffer<capacity> result;
        std::array<std::size_t, capacity> type_starts = {{}};
        type_name_normalizer normalizer(name, msvc, result.data.data(), type_starts.data());
        while (normalizer._pos < name.size())
            normalizer.process_next();
        result.size = normalizer._size;
        return result;
    }

private:
    // What the output currently ends with.
    enum class ending {
        PUNCTUATION,    // E.g. "<" or ",", or nothing at all.
        POINTER,        // "*" or "&".
        TYPE_NAME,      // A word that's part of a type name, e.g. "int" or "ns".
        QUALIFIER,      // A qualifier that stays where it is, e.g. "const" in "const int" or in "int *const".
        TEMPLATE_ARGS,  // A closing ">".
    };
    using enum ending;

    // Clang, GCC and MSVC spellings, in this order. MSVC uses both of its spellings, the one with a dash at the top
    // level, and the one with a space inside template arguments.
    static constexpr std::array<std::string_view, 4> anonymous_namespace_spellings = {
        "(anonymous namespace)", "{anonymous}", "`anonymous-namespace'", "`anonymous namespace'"
    };

    consteval void process_next() {
        char c = _name[_pos];
        if (c == ' ') {
            _pos++;
        } else if (c == '\'' || c == '"') {
            copy_literal();
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
        if (_msvc && (first == 'c' || first == 's' || first == 'e' || first == 'u') && (word == "class" || word == "struct" || word == "enum" || word == "union"))
            return;

        // A qualifier right after a type applies to that type, e.g. in "int const" or "X<int> const", and we move it in
        // front of the type. A qualifier after "*" applies to the pointer, and a qualifier after ")" applies to the
        // function, so these stay where they are. Same for compiler-specific pointer modifiers like "__restrict".
        bool is_qualifier = (first == 'c' || first == 'v') && (word == "const" || word == "volatile");
        if (is_qualifier && (_ending == TYPE_NAME || _ending == TEMPLATE_ARGS)) {
            move_to_type_start(word);
            return; // The output still ends with the same type.
        }

        bool is_pointer_modifier = _ending == POINTER && word.starts_with("__");
        if (_ending == TYPE_NAME || _ending == QUALIFIER)
            append(" ");
        append(word);
        _ending = is_qualifier || is_pointer_modifier ? QUALIFIER : TYPE_NAME;
    }

    consteval void process_punctuation(char c) {
        if (c == '<' || c == '(') {
            _type_starts[_depth++] = _size + 1;
        } else if ((c == '>' || c == ')') && _depth > 0) {
            _depth--;
        } else if (c == ',' && _depth > 0) {
            _type_starts[_depth - 1] = _size + 1;
        }

        append({&c, 1});
        if (c == '>') {
            _ending = TEMPLATE_ARGS;
        } else if (c == '*' || c == '&') {
            _ending = POINTER;
        } else {
            _ending = PUNCTUATION;
        }
    }

    // Copies a character or string literal, e.g. in "X<' '>", without looking inside.
    consteval void copy_literal() {
        char quote = _name[_pos];
        std::size_t end = _pos + 1;
        while (end < _name.size() && _name[end] != quote)
            end += _name[end] == '\\' ? 2 : 1;
        end = std::min(end + 1, _name.size());

        append(_name.substr(_pos, end - _pos));
        _pos = end;
        _ending = PUNCTUATION;
    }

    consteval void move_to_type_start(std::string_view word) {
        std::size_t pos = _depth == 0 ? 0 : _type_starts[_depth - 1];

        // Keep "const volatile" in this order, like clang and GCC.
        if (word == "volatile" && std::string_view(_out + pos, _size - pos).starts_with("const "))
            pos += 6;

        insert(pos, " ");
        insert(pos, word);
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
            for (std::string_view anonymous : anonymous_namespace_spellings) {
                if (rest.starts_with(anonymous)) {
                    _pos += anonymous.size();
                    return anonymous_namespace_spellings[0];
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
    bool _msvc = false;
    std::size_t _pos = 0;
    char *_out = nullptr;
    std::size_t _size = 0;
    ending _ending = PUNCTUATION;
    std::size_t *_type_starts = nullptr; // Stack of where the innermost types inside the open "<" and "(" start in the output.
    std::size_t _depth = 0; // Size of the stack above.
};

template <class T>
consteval auto type_name_static_string() noexcept {
#if SN_USE_CLANG_TYPE_NAME
    constexpr std::string_view prefix   = "[T = ";
    constexpr std::string_view suffix   = "]";
    constexpr std::string_view function = __PRETTY_FUNCTION__;
    constexpr bool msvc = false;
#elif SN_USE_GCC_TYPE_NAME
    constexpr std::string_view prefix   = "with T = ";
    constexpr std::string_view suffix   = "]";
    constexpr std::string_view function = __PRETTY_FUNCTION__;
    constexpr bool msvc = false;
#elif SN_USE_MSVC_TYPE_NAME
    constexpr std::string_view prefix   = "type_name_static_string<";
    constexpr std::string_view suffix   = ">(void)";
    constexpr std::string_view function = __FUNCSIG__;
    constexpr bool msvc = true;
#endif

    constexpr std::size_t start_pos = function.find(prefix);
    static_assert(start_pos != std::string_view::npos);

    constexpr std::size_t end_pos = function.rfind(suffix);
    static_assert(end_pos != std::string_view::npos);
    static_assert(start_pos + prefix.size() < end_pos);

    constexpr std::string_view type_name = function.substr(start_pos + prefix.size(), end_pos - start_pos - prefix.size());

    constexpr auto normalized = type_name_normalizer::normalize<type_name.size() * 2>(type_name, msvc);

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
