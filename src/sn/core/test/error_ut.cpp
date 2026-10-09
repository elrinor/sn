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
    e.prepend_path("y");
    e.prepend_path(2);
    e.prepend_path("points");
    EXPECT_EQ(e.message(), "'zz' is not a number");
    EXPECT_EQ(e.path(), "points[2].y");
    EXPECT_EQ(e.what(), "points[2].y: 'zz' is not a number");
}

TEST(error, path_starting_with_index) {
    sn::error e("x");
    e.prepend_path("a");
    e.prepend_path(1);
    e.prepend_path(0);
    EXPECT_EQ(e.path(), "[0][1].a");
}

TEST(error, prepend_error_path) {
    sn::error e("x");
    sn::prepend_error_path(&e, "a");
    sn::prepend_error_path(&e, 0);
    EXPECT_EQ(e.path(), "[0].a");

    // Does nothing for nullptr.
    sn::prepend_error_path(nullptr, "a");
    sn::prepend_error_path(nullptr, 0);
}

TEST(error, equality) {
    sn::error a("x");
    sn::error b("x");
    sn::error c("y");
    EXPECT_TRUE(a == b);
    EXPECT_FALSE(a == c);

    b.prepend_path("k");
    EXPECT_FALSE(a == b);
}

TEST(error, namespace) {
    // sn::error must not be declared directly in sn, see the comment on sn::errors::error.
    static_assert(std::is_same_v<sn::error, sn::errors::error>);
}
