#include <gtest/gtest.h>

#include "sn/detail/test/boolean_test_suite.h"
#include "sn/detail/test/integer_test_suite.h"
#include "sn/detail/test/float_test_suite.h"

#include "qstring.h"

template<class T>
struct qstring_callback {
    [[nodiscard]] bool try_to(const T &src, QString *dst) noexcept {
        return sn::try_to_qstring(src, dst);
    }

    [[nodiscard]] QString to(const T &src) {
        return sn::to_qstring(src);
    }

    [[nodiscard]] bool try_from(QStringView src, T *dst) noexcept {
        return sn::try_from_qstring(src, dst);
    }

    [[nodiscard]] T from(QStringView src) {
        return sn::from_qstring<T>(src);
    }
};

struct qstringifier {
    QString operator()(std::string_view s) const {
        return QString::fromUtf8(s);
    }
};

// GTest integration. Note that the operator is static and won't escape this TS, but will be found via ADL.
// We need both functions b/c operator<< is used in test case comments, and PrintTo is used for printing invalid values.
static std::ostream &operator<<(std::ostream &os, const QString &s) {
    return os << s.toUtf8().toStdString();
}
static void PrintTo(const QString& s, std::ostream* os) {
    *os << s.toUtf8().toStdString();
}

template<class T>
static void run_pointer_tests() {
#ifdef _WIN32
    bool isWindows = true;
#else
    bool isWindows = false;
#endif

    // Check all char pointers & array types.
    EXPECT_FALSE(sn::qstringable<char[4]>);
    EXPECT_FALSE(sn::qstringable<char8_t[4]>);
    EXPECT_TRUE(sn::qstringable<char16_t[4]>);
    EXPECT_FALSE(sn::qstringable<char32_t[4]>);
    EXPECT_FALSE(sn::qstringable<wchar_t[4]>);
    EXPECT_FALSE(sn::qstringable<const char *>);
    EXPECT_FALSE(sn::qstringable<const char8_t *>);
    EXPECT_TRUE(sn::qstringable<const char16_t *>);
    EXPECT_FALSE(sn::qstringable<const char32_t *>);
    EXPECT_FALSE(sn::qstringable<const wchar_t *>);

    // Same checks for from_qstring, albeit this one is more of a sanity check as the first arg is always a QStringView.
    EXPECT_FALSE(requires(T s) { sn::builtins::from_qstring("123", &s); });
    EXPECT_FALSE(requires(T s) { sn::builtins::from_qstring(u8"123", &s); });
    EXPECT_TRUE(requires(T s) { sn::builtins::from_qstring(u"123", &s); });
    EXPECT_FALSE(requires(T s) { sn::builtins::from_qstring(U"123", &s); });
    EXPECT_EQ(requires(T s) { sn::builtins::from_qstring(L"123", &s); }, isWindows);

    // And we also do some sanity checks for non-char pointers.
    EXPECT_FALSE(sn::qstringable<void *>);
    EXPECT_FALSE(sn::qstringable<int *>);
    EXPECT_FALSE(sn::qstringable<unsigned char *>);
    EXPECT_FALSE(sn::qstringable<const void *>);
    EXPECT_FALSE(sn::qstringable<const int *>);
    EXPECT_FALSE(sn::qstringable<const unsigned char *>);
}

TEST(qstring, string) {
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
    EXPECT_FALSE(sn::qstringable<char>);
    EXPECT_FALSE(sn::qstringable<unsigned char>);
    EXPECT_FALSE(sn::qstringable<signed char>);
    EXPECT_FALSE(sn::qstringable<QChar>); // TODO(elric): do we want for this one to work?
}

TEST(qstring, boolean) {
    sn::detail::make_boolean_test_suite<QString>(qstringifier()).run(qstring_callback<bool>());
}

template<class T>
static void run_integer_tests() {
    sn::detail::make_integer_test_suite<T, QString>(qstringifier()).run(qstring_callback<T>());
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
    sn::detail::make_float_test_suite<T, QString>(qstringifier()).run(qstring_callback<T>());
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
