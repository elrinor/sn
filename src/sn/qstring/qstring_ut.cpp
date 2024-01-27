#include <gtest/gtest.h>

#include "qstring.h"

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

    // This one is checking that to_string with non-compatible pointers doesn't compile. We have to call directly into
    // sn::builtins because the entrypoint in namespace sn is unconstrained.
    EXPECT_FALSE(requires(T s) { sn::builtins::to_qstring("123", &s); });
    EXPECT_FALSE(requires(T s) { sn::builtins::to_qstring(U"123", &s); });
    EXPECT_FALSE(requires(T s) { sn::builtins::to_qstring(u8"123", &s); });
    EXPECT_EQ(requires(T s) { sn::builtins::to_qstring(L"123", &s); }, isWindows);

    // Same for from_string, albeit this one is more of a sanity check as the first arg is always a QStringView.
    EXPECT_FALSE(requires(T s) { sn::builtins::from_qstring("123", &s); });
    EXPECT_FALSE(requires(T s) { sn::builtins::from_qstring(U"123", &s); });
    EXPECT_FALSE(requires(T s) { sn::builtins::from_qstring(u8"123", &s); });
    EXPECT_EQ(requires(T s) { sn::builtins::from_qstring(L"123", &s); }, isWindows);

    // And we also do some sanity checks for non-char pointers.
    EXPECT_FALSE(requires(T s) { sn::builtins::to_qstring(static_cast<void *>(nullptr), &s); });
    EXPECT_FALSE(requires(T s) { sn::builtins::to_qstring(static_cast<int *>(nullptr), &s); });
    EXPECT_FALSE(requires(T s) { sn::builtins::to_qstring(static_cast<const void *>(nullptr), &s); });
    EXPECT_FALSE(requires(T s) { sn::builtins::to_qstring(static_cast<const int *>(nullptr), &s); });
}

TEST(qstring, string) {
    run_pointer_tests<QString>();

    // char16_t strings work.
    EXPECT_EQ(sn::to_qstring(u"123"), QStringLiteral("123"));
    EXPECT_EQ(sn::to_qstring(QString::fromUtf16(u"123")), QStringLiteral("123"));
    EXPECT_EQ(sn::to_qstring(QStringView(u"123")), QStringLiteral("123"));
    EXPECT_EQ(sn::from_qstring<QString>(u"123"), QStringLiteral("123"));
    EXPECT_EQ(sn::from_qstring<QString>(QString::fromUtf16(u"123")), QStringLiteral("123"));
    EXPECT_EQ(sn::from_qstring<QString>(QStringView(u"123")), QStringLiteral("123"));
}

TEST(qstring, boolean) {
    EXPECT_EQ(sn::from_qstring<bool>(sn::to_qstring(true)), true);
    EXPECT_EQ(sn::from_qstring<bool>(sn::to_qstring(false)), false);
    EXPECT_EQ(sn::from_qstring<bool>(QStringLiteral("0")), false);
    EXPECT_EQ(sn::from_qstring<bool>(QStringLiteral("1")), true);

    EXPECT_ANY_THROW((void) sn::from_qstring<bool>(QString::fromUtf8("da")));
    EXPECT_ANY_THROW((void) sn::from_qstring<bool>(QString()));
}

static QString prepend_zeros(int zeros, QStringView number_string) {
    QString result;

    if (number_string.startsWith(QLatin1Char('-'))) {
        result = QStringLiteral("-");
        number_string = number_string.mid(1);
    }

    result += QString(zeros, QLatin1Char('0'));
    result += number_string;
    return result;
}

template<class T>
static void run_integer_tests() {
    QString s;

    EXPECT_EQ(sn::to_qstring(static_cast<T>(0)), QStringLiteral("0"));
    EXPECT_EQ(sn::from_qstring<T>(QStringLiteral("0")), 0);

    EXPECT_EQ(sn::to_qstring(static_cast<T>(1)), QStringLiteral("1"));
    EXPECT_EQ(sn::from_qstring<T>(QStringLiteral("1")), 1);

    EXPECT_EQ(sn::to_qstring(static_cast<T>(100)), QStringLiteral("100"));
    EXPECT_EQ(sn::from_qstring<T>(QStringLiteral("100")), 100);

    EXPECT_EQ(sn::from_qstring<T>(sn::to_qstring(std::numeric_limits<T>::max())), std::numeric_limits<T>::max());
    EXPECT_EQ(sn::from_qstring<T>(sn::to_qstring(std::numeric_limits<T>::min())), std::numeric_limits<T>::min());

    EXPECT_EQ(sn::from_qstring<T>(prepend_zeros(1, sn::to_qstring(std::numeric_limits<T>::max()))), std::numeric_limits<T>::max());
    EXPECT_EQ(sn::from_qstring<T>(prepend_zeros(1, sn::to_qstring(std::numeric_limits<T>::min()))), std::numeric_limits<T>::min());

    EXPECT_EQ(sn::from_qstring<T>(prepend_zeros(100, sn::to_qstring(std::numeric_limits<T>::max()))), std::numeric_limits<T>::max());
    EXPECT_EQ(sn::from_qstring<T>(prepend_zeros(100, sn::to_qstring(std::numeric_limits<T>::min()))), std::numeric_limits<T>::min());

    if constexpr (sizeof(T) < sizeof(long long)) {
        EXPECT_ANY_THROW((void) sn::from_qstring<T>(sn::to_qstring(static_cast<long long>(std::numeric_limits<T>::max()) + 1)));
        EXPECT_ANY_THROW((void) sn::from_qstring<T>(sn::to_qstring(static_cast<long long>(std::numeric_limits<T>::min()) - 1)));
    } else {
        static_assert(sizeof(T) == 8);

        if constexpr (std::is_unsigned_v<T>) {
            EXPECT_ANY_THROW((void) sn::from_qstring<T>(QStringLiteral("-1")));
            EXPECT_ANY_THROW((void) sn::from_qstring<T>(QStringLiteral("18446744073709551616"))); // max unsigned long long +1
        } else {
            EXPECT_ANY_THROW((void) sn::from_qstring<T>(QStringLiteral("–9223372036854775809"))); // min long long -1
            EXPECT_ANY_THROW((void) sn::from_qstring<T>(QStringLiteral("9223372036854775808"))); // max long long +1
        }
    }

    EXPECT_ANY_THROW((void) sn::from_qstring<T>(QStringLiteral("")));
    EXPECT_ANY_THROW((void) sn::from_qstring<T>(QStringLiteral(" 1")));
    EXPECT_ANY_THROW((void) sn::from_qstring<T>(QStringLiteral("1 ")));
    EXPECT_ANY_THROW((void) sn::from_qstring<T>(QStringLiteral("+1")));
    EXPECT_ANY_THROW((void) sn::from_qstring<T>(QStringLiteral("--1")));
    EXPECT_ANY_THROW((void) sn::from_qstring<T>(QStringLiteral("0x1")));
    EXPECT_ANY_THROW((void) sn::from_qstring<T>(QStringLiteral("0b1")));
    EXPECT_ANY_THROW((void) sn::from_qstring<T>(QStringLiteral("0-0")));
    EXPECT_ANY_THROW((void) sn::from_qstring<T>(QStringLiteral("0-1")));
}

TEST(string, ints) {
    run_integer_tests<short>();
    run_integer_tests<unsigned short>();
    run_integer_tests<int>();
    run_integer_tests<unsigned int>();
    run_integer_tests<long>();
    run_integer_tests<unsigned long>();
    run_integer_tests<long long>();
    run_integer_tests<unsigned long long>();
}
