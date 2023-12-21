#include <gtest/gtest.h>

#include "qstring.h"

TEST(qstring, boolean) {
    EXPECT_EQ(sn::from_qstring<bool>(sn::to_qstring(true)), true);
    EXPECT_EQ(sn::from_qstring<bool>(sn::to_qstring(false)), false);
    EXPECT_EQ(sn::from_qstring<bool>(QStringLiteral("0")), false);
    EXPECT_EQ(sn::from_qstring<bool>(QStringLiteral("1")), true);

    EXPECT_ANY_THROW((void) sn::from_qstring<bool>(QString::fromUtf8("da")));
    EXPECT_ANY_THROW((void) sn::from_qstring<bool>(QString()));
}
