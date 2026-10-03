#include <string>
#include <string_view>
#include <functional> // For std::identity.

#include <gtest/gtest.h> // NOLINT: not a C system header.

#include "sn/detail/test/integer_test_suite.h"
#include "sn/detail/test/boolean_test_suite.h"
#include "sn/detail/test/float_test_suite.h"
#include "sn/string/string.h"

template<class T>
static void check_supported() {
    static_assert(sn::concepts::to_stringable<T>);
    static_assert(sn::concepts::try_to_stringable<T>);
    static_assert(sn::concepts::from_stringable<T>);
    static_assert(sn::concepts::try_from_stringable<T>);
}

template<class T>
static void check_unsupported() {
    static_assert(!sn::concepts::to_stringable<T>);
    static_assert(!sn::concepts::try_to_stringable<T>);
    static_assert(!sn::concepts::from_stringable<T>);
    static_assert(!sn::concepts::try_from_stringable<T>);
}

template<class T>
static void check_has_to_string_only() {
    static_assert(sn::concepts::to_stringable<T>);
    static_assert(sn::concepts::try_to_stringable<T>);
    static_assert(!sn::concepts::from_stringable<T>);
    static_assert(!sn::concepts::try_from_stringable<T>);
}

template<class T>
static void run_pointer_tests() {
    // Check all char pointers & array types.
    check_has_to_string_only<char[4]>();
    check_unsupported<char8_t[4]>();
    check_unsupported<char16_t[4]>();
    check_unsupported<char32_t[4]>();
    check_unsupported<wchar_t[4]>();
    check_has_to_string_only<const char *>();
    check_unsupported<const char8_t *>();
    check_unsupported<const char16_t *>();
    check_unsupported<const char32_t *>();
    check_unsupported<const wchar_t *>();
    check_has_to_string_only<char *>();
    check_unsupported<char8_t *>();
    check_unsupported<char16_t *>();
    check_unsupported<char32_t *>();
    check_unsupported<wchar_t *>();

    // Same checks for from_string, albeit this one is more of a sanity check as the first arg is always a std::string_view.
    static_assert(requires(T s) { sn::detail::builtins::from_string("123", &s); });
    static_assert(!requires(T s) { sn::detail::builtins::from_string(u8"123", &s); });
    static_assert(!requires(T s) { sn::detail::builtins::from_string(u"123", &s); });
    static_assert(!requires(T s) { sn::detail::builtins::from_string(U"123", &s); });
    static_assert(!requires(T s) { sn::detail::builtins::from_string(L"123", &s); });

    // And we also do some sanity checks for non-char pointers.
    check_unsupported<unsigned char *>();
    check_unsupported<signed char *>();
    check_unsupported<const unsigned char *>();
    check_unsupported<const signed char *>();
    check_unsupported<void *>();
    check_unsupported<int *>();
    check_unsupported<unsigned int *>();
    check_unsupported<const void *>();
    check_unsupported<const int *>();
    check_unsupported<const unsigned char *>();
}

TEST(string, string) {
    run_pointer_tests<std::string>();

    // Char strings work.
    const char *str = "1234";
    EXPECT_EQ(sn::to_string(str), "1234");
    EXPECT_EQ(sn::to_string("123"), "123");
    EXPECT_EQ(sn::to_string(std::string("123")), "123");
    EXPECT_EQ(sn::to_string(std::string_view("123")), "123");
    EXPECT_EQ(sn::from_string<std::string>("123"), "123");
    EXPECT_EQ(sn::from_string<std::string>(std::string("123")), "123");
    EXPECT_EQ(sn::from_string<std::string>(std::string_view("123")), "123");
}

TEST(string, char) {
    check_unsupported<char>();
    check_unsupported<unsigned char>();
    check_unsupported<signed char>();
}

TEST(string, boolean) {
    sn::detail::run_boolean_test_suite();
}

TEST(string, ints) {
    sn::detail::run_integer_test_suite<short>();
    sn::detail::run_integer_test_suite<unsigned short>();
    sn::detail::run_integer_test_suite<int>();
    sn::detail::run_integer_test_suite<unsigned int>();
    sn::detail::run_integer_test_suite<long>();
    sn::detail::run_integer_test_suite<unsigned long>();
    sn::detail::run_integer_test_suite<long long>();
    sn::detail::run_integer_test_suite<unsigned long long>();
}

TEST(string, floats) {
    sn::detail::run_float_test_suite<float>();
    sn::detail::run_float_test_suite<double>();
}

namespace friendlyns {
struct friendly {
    friendly() = default;
    explicit friendly(int value) : value(value) {}
    friend auto operator<=>(const friendly &, const friendly &) = default;

    SN_DECLARE_FRIEND_STRING_FUNCTIONS(friendly)

    int value = 0;
};

void to_string(const friendly &src, std::string *dst) {
    sn::to_string(src.value, dst);
}

void from_string(std::string_view src, friendly *dst) {
    sn::from_string(src, &dst->value);
}
} // namespace friendlyns

TEST(string, friend) { // NOLINT: this is not std::string.
    EXPECT_EQ(sn::to_string(friendlyns::friendly(1)), "1");
    EXPECT_EQ(sn::from_string<friendlyns::friendly>("1"), friendlyns::friendly(1));
}

class Base {};
class Derived : public Base {};
SN_DECLARE_STRING_FUNCTIONS(Base)

TEST(string, slicing) {
    check_supported<Base>();
    check_unsupported<Derived>();
}
