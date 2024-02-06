#include <gtest/gtest.h>

#include "sn/detail/test/integer_test_suite.h"

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

// GTest integration. Note that the operator is static and won't escape this TS, but will be found via ADL.
static std::ostream &operator<<(std::ostream &os, const QString &s) {
    return os << s.toUtf8().toStdString();
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

template<class T>
static void run_integer_tests() {
    auto stringifier = [] (std::string_view s) {
        return QString::fromUtf8(s);
    };
    sn::detail::make_integer_test_suite<T, QString>(stringifier).run(qstring_callback<T>());
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
