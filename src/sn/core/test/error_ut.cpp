#include <type_traits>

#include <gtest/gtest.h> // NOLINT: not a C system header.

#include "sn/core/error.h"

TEST(error, empty) {
    sn::error e;
    EXPECT_EQ(e.message(), "");
    EXPECT_EQ(e.path(), "");
    EXPECT_EQ(e.what(), "");
}

TEST(error, message) {
    sn::error e("'zz' is not a number");
    EXPECT_EQ(e.message(), "'zz' is not a number");
    EXPECT_EQ(e.path(), "");
    EXPECT_EQ(e.what(), "'zz' is not a number");
}

TEST(error, path) {
    // Paths are built innermost segment first, while unwinding from the value that failed.
    sn::error e("'zz' is not a number");
    sn::error::prepend_path(&e, "y");
    sn::error::prepend_path(&e, 2);
    sn::error::prepend_path(&e, "points");
    EXPECT_EQ(e.message(), "'zz' is not a number");
    EXPECT_EQ(e.path(), "points[2].y");
    EXPECT_EQ(e.what(), "points[2].y: 'zz' is not a number");
}

TEST(error, path_starting_with_index) {
    sn::error e("x");
    sn::error::prepend_path(&e, "a");
    sn::error::prepend_path(&e, 1);
    sn::error::prepend_path(&e, 0);
    EXPECT_EQ(e.path(), "[0][1].a");
}

TEST(error, prepend_path_to_nullptr) {
    // Does nothing.
    sn::error::prepend_path(nullptr, "a");
    sn::error::prepend_path(nullptr, 0);
}

TEST(error, equality) {
    sn::error a("x");
    sn::error b("x");
    sn::error c("y");
    EXPECT_TRUE(a == b);
    EXPECT_FALSE(a == c);

    sn::error::prepend_path(&b, "k");
    EXPECT_FALSE(a == b);
}

TEST(error, namespace) {
    // sn::error must not be declared directly in sn, see the comment on sn::errors::error.
    static_assert(std::is_same_v<sn::error, sn::errors::error>);
}
