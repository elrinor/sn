#include <exception>
#include <expected>
#include <string>
#include <type_traits>
#include <utility> // For std::move, std::forward.

#include <gtest/gtest.h> // NOLINT: not a C system header.

#include "sn/core/expected.h"

static sn::expected<int> parse(bool ok) {
    if (ok)
        return 5;
    return std::unexpected(sn::error("'zz' is not a number"));
}

template<class F>
static std::string thrown_message(F &&f) {
    try {
        std::forward<F>(f)();
    } catch (const sn::bad_expected_access &e) {
        return e.what();
    }
    return "<nothing thrown>";
}

TEST(expected, is_std_expected) {
    static_assert(std::is_same_v<sn::expected<int>, std::expected<int, sn::error>>);
    static_assert(std::is_same_v<sn::bad_expected_access, std::bad_expected_access<sn::error>>);
}

TEST(expected, value_throws_with_message) {
    sn::expected<int> bad = parse(false);
    const sn::expected<int> &const_bad = bad;
    EXPECT_EQ(thrown_message([&] { (void) bad.value(); }), "'zz' is not a number");
    EXPECT_EQ(thrown_message([&] { (void) const_bad.value(); }), "'zz' is not a number");
    EXPECT_EQ(thrown_message([] { (void) parse(false).value(); }), "'zz' is not a number");
    EXPECT_EQ(thrown_message([] { (void) sn::expected<void>(std::unexpect, "x").value(); }), "x");
}

TEST(expected, value_throws_with_path) {
    sn::error error("'zz' is not a number");
    error.prepend_path("y");
    error.prepend_path(2);
    error.prepend_path("points");
    sn::expected<int> bad = std::unexpected(error);
    EXPECT_EQ(thrown_message([&] { (void) bad.value(); }), "points[2].y: 'zz' is not a number");
}

TEST(expected, exception) {
    try {
        (void) parse(false).value();
        FAIL() << "Nothing thrown";
    } catch (const std::exception &e) {
        EXPECT_STREQ(e.what(), "'zz' is not a number");
    }

    sn::bad_expected_access e(sn::error("'zz' is not a number"));
    EXPECT_EQ(e.error().message(), "'zz' is not a number");
    EXPECT_EQ(std::as_const(e).error().message(), "'zz' is not a number");
    sn::error moved = std::move(e).error();
    EXPECT_EQ(moved.message(), "'zz' is not a number");
}
