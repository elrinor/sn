#include "qstring_ut.h"

#include <gtest/gtest.h>

#include "sn/detail/test/boolean_test_suite.h"
#include "sn/detail/test/integer_test_suite.h"
#include "sn/detail/test/float_test_suite.h"

template<class T>
static void check_supported() {
    static_assert(sn::concepts::has_to_qstring<T>);
    static_assert(sn::concepts::has_try_to_qstring<T>);
    static_assert(sn::concepts::has_from_qstring<T>);
    static_assert(sn::concepts::has_try_from_qstring<T>);
}

template<class T>
static void check_unsupported() {
    static_assert(!sn::concepts::has_to_qstring<T>);
    static_assert(!sn::concepts::has_try_to_qstring<T>);
    static_assert(!sn::concepts::has_from_qstring<T>);
    static_assert(!sn::concepts::has_try_from_qstring<T>);
}

template<class T>
static void check_has_to_string_only() {
    static_assert(sn::concepts::has_to_qstring<T>);
    static_assert(sn::concepts::has_try_to_qstring<T>);
    static_assert(!sn::concepts::has_from_qstring<T>);
    static_assert(!sn::concepts::has_try_from_qstring<T>);
}

template<class T>
static void run_pointer_tests() {
#ifdef _WIN32
    constexpr bool isWindows = true;
#else
    constexpr bool isWindows = false;
#endif

    // Check all char pointers & array types.
    check_unsupported<char[4]>();
    check_unsupported<char8_t[4]>();
    check_has_to_string_only<char16_t[4]>();
    check_unsupported<char32_t[4]>();
    check_unsupported<wchar_t[4]>();
    check_unsupported<const char *>();
    check_unsupported<const char8_t *>();
    check_has_to_string_only<const char16_t *>();
    check_unsupported<const char32_t *>();
    check_unsupported<const wchar_t *>();
    check_unsupported<char *>();
    check_unsupported<char8_t *>();
    check_has_to_string_only<char16_t *>();
    check_unsupported<char32_t *>();
    check_unsupported<wchar_t *>();

    // Same checks for from_qstring, albeit this one is more of a sanity check as the first arg is always a QStringView.
    static_assert(!requires(T s) { sn::detail::builtins::from_qstring("123", &s); });
    static_assert(!requires(T s) { sn::detail::builtins::from_qstring(u8"123", &s); });
    static_assert(requires(T s) { sn::detail::builtins::from_qstring(u"123", &s); });
    static_assert(!requires(T s) { sn::detail::builtins::from_qstring(U"123", &s); });
    static_assert(requires(T s) { sn::detail::builtins::from_qstring(L"123", &s); } == isWindows);

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

TEST(qstring, string) { // NOLINT: this is not std::string.
    run_pointer_tests<QString>();

    // char16_t strings work.
    const char16_t *str = u"1234";
    EXPECT_EQ(sn::to_qstring(str), QStringLiteral("1234"));
    EXPECT_EQ(sn::to_qstring(u"123"), QStringLiteral("123"));
    EXPECT_EQ(sn::to_qstring(QString::fromUtf16(u"123")), QStringLiteral("123"));
    EXPECT_EQ(sn::to_qstring(QStringView(u"123")), QStringLiteral("123"));
    EXPECT_EQ(sn::from_qstring<QString>(u"123"), QStringLiteral("123"));
    EXPECT_EQ(sn::from_qstring<QString>(QString::fromUtf16(u"123")), QStringLiteral("123"));
    EXPECT_EQ(sn::from_qstring<QString>(QStringView(u"123")), QStringLiteral("123"));
}

TEST(qstring, char) {
    check_unsupported<char>();
    check_unsupported<unsigned char>();
    check_unsupported<signed char>();
    check_unsupported<QChar>(); // TODO(elric): do we want for this one to work?
}

TEST(qstring, boolean) {
    sn::detail::run_boolean_test_suite(sn::detail::qstring_ops());
}

template<class T>
static void run_integer_tests() {
    sn::detail::run_integer_test_suite<T>(sn::detail::qstring_ops());
}

TEST(qstring, ints) {
    run_integer_tests<short>();
    run_integer_tests<unsigned short>();
    run_integer_tests<int>();
    run_integer_tests<unsigned int>();
    run_integer_tests<long>();
    run_integer_tests<unsigned long>();
    run_integer_tests<long long>();
    run_integer_tests<unsigned long long>();
}

template<class T>
static void run_float_tests() {
    sn::detail::run_float_test_suite<T>(sn::detail::qstring_ops());
}

TEST(qstring, floats) {
    run_float_tests<float>();
    run_float_tests<double>();
}

namespace qfriendlyns {
struct qfriendly {
    qfriendly() = default;
    explicit qfriendly(int value) : value(value) {}
    friend auto operator<=>(const qfriendly &, const qfriendly &) = default;

    SN_DECLARE_FRIEND_QSTRING_FUNCTIONS(qfriendly)

    int value = 0;
};

void to_qstring(const qfriendly &src, QString *dst) {
    sn::to_qstring(src.value, dst);
}

void from_qstring(QStringView src, qfriendly *dst) {
    sn::from_qstring(src, &dst->value);
}
} // namespace qfriendlyns

TEST(qstring, friend) {
    EXPECT_EQ(sn::to_qstring(qfriendlyns::qfriendly(1)), QStringLiteral("1"));
    EXPECT_EQ(sn::from_qstring<qfriendlyns::qfriendly>(QStringLiteral("1")), qfriendlyns::qfriendly(1));
}

class Base {};
class Derived : public Base {};
SN_DECLARE_QSTRING_FUNCTIONS(Base)

TEST(qstring, slicing) {
    check_supported<Base>();
    check_unsupported<Derived>();
}

TEST(qstring, base16) {
    EXPECT_EQ(sn::to_qstring(100, tn::hex), QStringLiteral("64"));
}

TEST(qstring, base9) {
    EXPECT_EQ(sn::to_qstring(100, tn::base<9>), QStringLiteral("121"));
}
