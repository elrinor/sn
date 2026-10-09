#include <gtest/gtest.h>

#include <any>
#include <string>
#include <vector>

#include "sn/core/type_name.h"

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
} // namespace ns

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

    // Compilers spell the anonymous namespace differently, e.g. GCC writes "{anonymous}".
#if defined(__clang__)
    EXPECT_EQ(sn::type_name<anonymous_class>(), "(anonymous namespace)::anonymous_class");
#else
    EXPECT_TRUE(sn::type_name<anonymous_class>().ends_with("::anonymous_class")) << sn::type_name<anonymous_class>();
#endif
}

TEST(core, type_name_string) {
    EXPECT_EQ(sn::type_name<std::string>(), "std::string");
    EXPECT_EQ(sn::type_name<std::string_view>(), "std::string_view");
    EXPECT_EQ(sn::type_name<std::wstring>(), "std::wstring");
    EXPECT_EQ(sn::type_name<std::wstring_view>(), "std::wstring_view");
}

TEST(core, type_name_constexpr) {
    static_assert(sn::type_name<int>() == "int");
    static_assert(sn::type_name<std::string>() == "std::string");
    static_assert(sn::type_name<Nothing<Something>>() == "Nothing<Something>");
}
