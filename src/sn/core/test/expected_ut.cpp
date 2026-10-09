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

// Error type that's not sn::error, but is comparable with it.
struct message_error {
    explicit message_error(std::string message) : message(std::move(message)) {}

    friend bool operator==(const message_error &l, const sn::error &r) {
        return l.message == r.message();
    }

    std::string message;
};

template<class L, class R>
constexpr bool equality_comparable = requires(const L &l, const R &r) { l == r; };

template<class F>
static std::string thrown_message(F &&f) {
    try {
        std::forward<F>(f)();
    } catch (const sn::bad_expected_access &e) {
        return e.what();
    }
    return "<nothing thrown>";
}

template<class U>
static bool takes_std_expected(const std::expected<U, sn::error> &e) {
    return e.has_value();
}

TEST(expected, value) {
    sn::expected<int> ok = parse(true);
    EXPECT_EQ(ok.value(), 5);
    EXPECT_EQ(ok.or_throw(), 5);
    EXPECT_EQ(*ok, 5);
    EXPECT_EQ(parse(true).value(), 5);
}

TEST(expected, value_throws_with_message) {
    sn::expected<int> bad = parse(false);
    EXPECT_THROW((void) bad.value(), sn::bad_expected_access);
    EXPECT_THROW((void) bad.or_throw(), sn::bad_expected_access);
    EXPECT_EQ(thrown_message([&] { (void) bad.value(); }), "'zz' is not a number");
    EXPECT_EQ(thrown_message([&] { (void) bad.or_throw(); }), "'zz' is not a number");
    EXPECT_EQ(thrown_message([] { (void) parse(false).value(); }), "'zz' is not a number");

    try {
        (void) bad.value();
    } catch (const std::bad_expected_access<sn::error> &e) {
        EXPECT_EQ(e.error().message(), "'zz' is not a number");
    }
}

TEST(expected, value_throws_with_path) {
    sn::error error("'zz' is not a number");
    error.prepend_path("y");
    error.prepend_path(2);
    error.prepend_path("points");
    sn::expected<int> bad = std::unexpected(error);
    EXPECT_EQ(thrown_message([&] { (void) bad.value(); }), "points[2].y: 'zz' is not a number");
}

TEST(expected, void) {
    sn::expected<void> ok;
    sn::expected<void> bad = std::unexpected(sn::error("value 42 is not valid"));
    EXPECT_NO_THROW(ok.value());
    EXPECT_NO_THROW(ok.or_throw());
    EXPECT_EQ(thrown_message([&] { bad.value(); }), "value 42 is not valid");
    EXPECT_EQ(thrown_message([&] { bad.or_throw(); }), "value 42 is not valid");
}

TEST(expected, and_then) {
    auto doubled = parse(true).and_then([](int x) { return sn::expected<int>(x * 2); });
    auto mixed = parse(true).and_then([](int x) { return std::expected<int, sn::error>(x + 1); });
    auto text = parse(true).and_then([](int x) { return sn::expected<std::string>(std::to_string(x)); });
    static_assert(std::is_same_v<decltype(doubled), sn::expected<int>>);
    static_assert(std::is_same_v<decltype(mixed), sn::expected<int>>);
    static_assert(std::is_same_v<decltype(text), sn::expected<std::string>>);
    EXPECT_EQ(doubled.value(), 10);
    EXPECT_EQ(mixed.value(), 6);
    EXPECT_EQ(text.value(), "5");

    auto failed = parse(false).and_then([](int x) { return sn::expected<int>(x * 2); });
    EXPECT_EQ(thrown_message([&] { (void) failed.value(); }), "'zz' is not a number");

    auto from_void = sn::expected<void>().and_then([] { return sn::expected<int>(1); });
    static_assert(std::is_same_v<decltype(from_void), sn::expected<int>>);
    EXPECT_EQ(from_void.value(), 1);
}

TEST(expected, or_else) {
    auto recovered = parse(false).or_else([](const sn::error &) { return sn::expected<int>(0); });
    static_assert(std::is_same_v<decltype(recovered), sn::expected<int>>);
    EXPECT_EQ(recovered.value(), 0);

    auto untouched = parse(true).or_else([](const sn::error &) { return sn::expected<int>(0); });
    EXPECT_EQ(untouched.value(), 5);

    auto other_error = parse(false).or_else([](const sn::error &e) { return std::expected<int, std::string>(std::unexpected(e.message())); });
    static_assert(std::is_same_v<decltype(other_error), std::expected<int, std::string>>);
    EXPECT_EQ(other_error.error(), "'zz' is not a number");
}

TEST(expected, transform) {
    auto text = parse(true).transform([](int x) { return std::to_string(x); });
    static_assert(std::is_same_v<decltype(text), sn::expected<std::string>>);
    EXPECT_EQ(text.value(), "5");

    auto chained = parse(false).transform([](int x) { return x; }).and_then([](int x) { return sn::expected<int>(x); });
    EXPECT_EQ(thrown_message([&] { (void) chained.value(); }), "'zz' is not a number");

    auto to_void = parse(true).transform([](int) {});
    static_assert(std::is_same_v<decltype(to_void), sn::expected<void>>);
    EXPECT_TRUE(to_void.has_value());
}

TEST(expected, transform_error) {
    auto with_path = parse(false).transform_error([](sn::error e) {
        e.prepend_path("x");
        return e;
    });
    static_assert(std::is_same_v<decltype(with_path), sn::expected<int>>);
    EXPECT_EQ(thrown_message([&] { (void) with_path.value(); }), "x: 'zz' is not a number");

    auto as_string = parse(false).transform_error([](const sn::error &e) { return e.message(); });
    static_assert(std::is_same_v<decltype(as_string), std::expected<int, std::string>>);
    EXPECT_EQ(as_string.error(), "'zz' is not a number");
}

TEST(expected, comparisons) {
    sn::expected<int> ok = parse(true);
    sn::expected<int> bad = parse(false);
    EXPECT_EQ(ok, 5);
    EXPECT_EQ(5, ok);
    EXPECT_NE(ok, 6);
    EXPECT_EQ(ok, sn::expected<int>(5));
    EXPECT_NE(bad, 5);
    EXPECT_NE(ok, bad);
    EXPECT_EQ(sn::expected<void>(), sn::expected<void>());
}

TEST(expected, comparisons_with_unexpected) {
    sn::expected<int> ok = parse(true);
    sn::expected<int> bad = parse(false);
    EXPECT_EQ(bad, std::unexpected(sn::error("'zz' is not a number")));
    EXPECT_EQ(std::unexpected(sn::error("'zz' is not a number")), bad);
    EXPECT_NE(bad, std::unexpected(sn::error("x")));
    EXPECT_NE(std::unexpected(sn::error("x")), bad);
    EXPECT_NE(ok, std::unexpected(sn::error("'zz' is not a number")));
    EXPECT_NE(std::unexpected(sn::error("'zz' is not a number")), ok);

    // Different error type.
    EXPECT_EQ(bad, std::unexpected(message_error("'zz' is not a number")));
    EXPECT_EQ(std::unexpected(message_error("'zz' is not a number")), bad);
    EXPECT_NE(bad, std::unexpected(message_error("x")));
    EXPECT_NE(std::unexpected(message_error("x")), bad);
}

TEST(expected, comparisons_with_std_expected) {
    sn::expected<int> ok = parse(true);
    sn::expected<int> bad = parse(false);
    std::expected<int, sn::error> std_ok = 5;
    std::expected<int, sn::error> std_bad = std::unexpected(sn::error("'zz' is not a number"));
    EXPECT_EQ(ok, std_ok);
    EXPECT_EQ(std_ok, ok);
    EXPECT_EQ(bad, std_bad);
    EXPECT_EQ(std_bad, bad);
    EXPECT_NE(ok, std_bad);
    EXPECT_NE(std_bad, ok);
    EXPECT_NE(ok, (std::expected<int, sn::error>(6)));
    EXPECT_NE((std::expected<int, sn::error>(6)), ok);

    // Different value or error type only works with sn::expected on the left, see the comment in sn::expected.
    std::expected<int, message_error> other_bad = std::unexpected(message_error("'zz' is not a number"));
    EXPECT_EQ(ok, (std::expected<long, sn::error>(5)));
    EXPECT_EQ(bad, other_bad);
    EXPECT_NE(ok, other_bad);
    static_assert(!equality_comparable<std::expected<long, sn::error>, sn::expected<int>>);
    static_assert(!equality_comparable<std::expected<int, message_error>, sn::expected<int>>);

    // Void.
    EXPECT_EQ(sn::expected<void>(), (std::expected<void, sn::error>()));
    EXPECT_EQ((std::expected<void, sn::error>()), sn::expected<void>());
    EXPECT_NE(sn::expected<void>(), (std::expected<void, sn::error>(std::unexpect, "x")));
    EXPECT_NE((std::expected<void, sn::error>(std::unexpect, "x")), sn::expected<void>());
    std::expected<void, message_error> other_void_bad = std::unexpected(message_error("x"));
    EXPECT_EQ(sn::expected<void>(std::unexpect, "x"), other_void_bad);
    EXPECT_EQ(other_void_bad, sn::expected<void>(std::unexpect, "x"));
}

TEST(expected, std_interop) {
    EXPECT_TRUE(takes_std_expected(parse(true)));
    EXPECT_FALSE(takes_std_expected(parse(false)));

    std::expected<int, sn::error> std_ok = 1;
    sn::expected<int> converted = std_ok;
    EXPECT_EQ(converted.value(), 1);

    static_assert(sizeof(sn::expected<int>) == sizeof(std::expected<int, sn::error>));
}
