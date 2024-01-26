#include <gtest/gtest.h>

#include "qstring.h"

void PrintTo(const QString& s, std::ostream* os) {
    *os << s.toUtf8().toStdString();
}

template<class T>
void run_pointer_tests() {
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
