#include <gtest/gtest.h>

#include <any>
#include <array>
#include <string>
#include <string_view>
#include <vector>

#include "sn/core/type_name.h"
#include "sn/core/type_name_fwd.h"

TEST(core, type_name_builtin) {
    EXPECT_EQ(sn::type_name<bool>(), "bool");

    EXPECT_EQ(sn::type_name<char>(), "char");
    EXPECT_EQ(sn::type_name<short>(), "short");
    EXPECT_EQ(sn::type_name<int>(), "int");
    EXPECT_EQ(sn::type_name<long>(), "long");
    EXPECT_EQ(sn::type_name<long long>(), "long long");

    EXPECT_EQ(sn::type_name<unsigned char>(), "unsigned char");
    EXPECT_EQ(sn::type_name<unsigned short>(), "unsigned short");
    EXPECT_EQ(sn::type_name<unsigned int>(), "unsigned int");
    EXPECT_EQ(sn::type_name<unsigned long>(), "unsigned long");
    EXPECT_EQ(sn::type_name<unsigned long long>(), "unsigned long long");

    EXPECT_EQ(sn::type_name<signed char>(), "signed char");
    EXPECT_EQ(sn::type_name<signed short>(), "short");
    EXPECT_EQ(sn::type_name<signed int>(), "int");
    EXPECT_EQ(sn::type_name<signed long>(), "long");
    EXPECT_EQ(sn::type_name<signed long long>(), "long long");

    EXPECT_EQ(sn::type_name<float>(), "float");
    EXPECT_EQ(sn::type_name<double>(), "double");
    EXPECT_EQ(sn::type_name<long double>(), "long double");

    EXPECT_EQ(sn::type_name<char8_t>(), "char8_t");
    EXPECT_EQ(sn::type_name<char16_t>(), "char16_t");
    EXPECT_EQ(sn::type_name<char32_t>(), "char32_t");
    EXPECT_EQ(sn::type_name<wchar_t>(), "wchar_t");
}

class Something {};

template<class T>
class Nothing {};

struct Struct {};

class some_class {};
struct some_struct {};

template<template<class...> class>
class templated {};

TEST(core, type_name_template) {
    EXPECT_EQ(sn::type_name<Struct>(), "Struct");
    EXPECT_EQ(sn::type_name<Something>(), "Something");
    EXPECT_EQ(sn::type_name<Nothing<Something>>(), "Nothing<Something>");
    EXPECT_EQ(sn::type_name<Nothing<Nothing<Something>>>(), "Nothing<Nothing<Something>>");
    EXPECT_EQ(sn::type_name<some_class *>(), "some_class*");
    EXPECT_EQ(sn::type_name<some_struct &>(), "some_struct&");
    EXPECT_EQ(sn::type_name<some_class **>(), "some_class**");
    EXPECT_EQ(sn::type_name<some_struct &&>(), "some_struct&&");
    EXPECT_EQ(sn::type_name<templated<std::vector>>(), "templated<std::vector>");
}

TEST(core, type_name_namespace) {
    EXPECT_EQ(sn::type_name<std::any>(), "std::any");
}

namespace ns {
enum class scoped_enum { value };
enum unscoped_enum { unscoped_value };
union some_union {};

class renamed {};
SN_DEFINE_TYPE_NAME(renamed, "Renamed")
} // namespace ns

TEST(core, type_name_override) {
    static_assert(sn::type_name<ns::renamed>() == "Renamed");
    EXPECT_EQ(sn::type_name<ns::renamed>(), "Renamed");
}

namespace {
class anonymous_class {};
} // namespace

TEST(core, type_name_keywords) {
    // MSVC writes these with "enum " / "union " in front.
    EXPECT_EQ(sn::type_name<ns::scoped_enum>(), "ns::scoped_enum");
    EXPECT_EQ(sn::type_name<ns::unscoped_enum>(), "ns::unscoped_enum");
    EXPECT_EQ(sn::type_name<ns::some_union>(), "ns::some_union");
    EXPECT_EQ(sn::type_name<Nothing<ns::scoped_enum>>(), "Nothing<ns::scoped_enum>");
}

TEST(core, type_name_spaces) {
    EXPECT_EQ(sn::type_name<Nothing<unsigned char>>(), "Nothing<unsigned char>");
    EXPECT_EQ(sn::type_name<Nothing<long double>>(), "Nothing<long double>");
}

TEST(core, type_name_anonymous_namespace) {
    // GCC and MSVC spell it differently, but we use clang's spelling everywhere.
    EXPECT_EQ(sn::type_name<anonymous_class>(), "(anonymous namespace)::anonymous_class");
    EXPECT_EQ(sn::type_name<Nothing<anonymous_class>>(), "Nothing<(anonymous namespace)::anonymous_class>");

    EXPECT_EQ(sn::type_name<Nothing<const anonymous_class>>(), "Nothing<const (anonymous namespace)::anonymous_class>");
}

TEST(core, type_name_qualifiers) {
    EXPECT_EQ(sn::type_name<Nothing<const int>>(), "Nothing<const int>");
    EXPECT_EQ(sn::type_name<Nothing<const volatile int>>(), "Nothing<const volatile int>");
    EXPECT_EQ(sn::type_name<Nothing<const Nothing<int>>>(), "Nothing<const Nothing<int>>");
    EXPECT_EQ(sn::type_name<Nothing<const char *>>(), "Nothing<const char*>");
    EXPECT_EQ(sn::type_name<Nothing<const char *const>>(), "Nothing<const char*const>");
}

// Checks normalize_type_name on spellings that only some compilers produce, e.g. MSVC's "int const".
consteval bool normalizes_to(std::string_view name, std::string_view expected) {
    std::array<char, 128> buffer = {};
    std::size_t size = sn::detail::normalize_type_name(name, buffer.data());
    return size == sn::detail::normalize_type_name(name, nullptr) && std::string_view(buffer.data(), size) == expected;
}

TEST(core, type_name_normalize) {
    static_assert(normalizes_to("int const", "const int"));
    static_assert(normalizes_to("unsigned int const", "const unsigned int"));
    static_assert(normalizes_to("int volatile", "volatile int"));
    static_assert(normalizes_to("int const volatile", "const volatile int"));
    static_assert(normalizes_to("int volatile const", "const volatile int"));
    static_assert(normalizes_to("const volatile int", "const volatile int"));
    static_assert(normalizes_to("volatile int const", "const volatile int"));
    static_assert(normalizes_to("char const *", "const char*"));
    static_assert(normalizes_to("char const * const", "const char*const"));
    static_assert(normalizes_to("Nothing<int const >", "Nothing<const int>"));
    static_assert(normalizes_to("Nothing<Nothing<int> const >", "Nothing<const Nothing<int>>"));
    static_assert(normalizes_to("std::pair<int const,Nothing<int> const >", "std::pair<const int,const Nothing<int>>"));
    static_assert(normalizes_to("Nothing<`anonymous-namespace'::X const >", "Nothing<const (anonymous namespace)::X>"));
    static_assert(normalizes_to("Nothing<{anonymous}::X>", "Nothing<(anonymous namespace)::X>"));
    static_assert(normalizes_to("Nothing<const_iterator>", "Nothing<const_iterator>"));
    static_assert(normalizes_to("void (int) const", "void(int)const"));
}

TEST(core, type_name_string) {
    EXPECT_EQ(sn::type_name<std::string>(), "std::string");
    EXPECT_EQ(sn::type_name<std::string_view>(), "std::string_view");
    EXPECT_EQ(sn::type_name<std::wstring>(), "std::wstring");
    EXPECT_EQ(sn::type_name<std::wstring_view>(), "std::wstring_view");
    EXPECT_EQ(sn::type_name<std::u8string>(), "std::u8string");
    EXPECT_EQ(sn::type_name<std::u8string_view>(), "std::u8string_view");
    EXPECT_EQ(sn::type_name<std::u16string>(), "std::u16string");
    EXPECT_EQ(sn::type_name<std::u16string_view>(), "std::u16string_view");
    EXPECT_EQ(sn::type_name<std::u32string>(), "std::u32string");
    EXPECT_EQ(sn::type_name<std::u32string_view>(), "std::u32string_view");
}

TEST(core, type_name_constexpr) {
    static_assert(sn::type_name<int>() == "int");
    static_assert(sn::type_name<std::string>() == "std::string");
    static_assert(sn::type_name<Nothing<Something>>() == "Nothing<Something>");
}
