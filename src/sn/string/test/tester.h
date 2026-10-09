#pragma once

#include <initializer_list>
#include <optional>
#include <string>
#include <string_view>
#include <utility> // For std::pair, std::forward.

#include <gtest/gtest.h> // NOLINT: not a C system header.

#include "sn/core/error.h"
#include "sn/core/expected.h"
#include "sn/core/type_name.h"
#include "sn/core/detail/concat.h"
#include "sn/string/string.h"
#include "sn/string/string_tags.h"

namespace sn::detail {

// TODO(elric): sn/debug
inline std::string to_debug_string() {
    return "<none>";
}

inline std::string to_debug_string(sn::tags::dynamic_base_tag tag) {
    return sn::detail::concat("sn::tags::dynamic_base_tag(", std::to_string(tag.value()), ")");
}

template<int base>
inline std::string to_debug_string(sn::tags::base_tag<base>) {
    return sn::detail::concat("sn::tags::base_tag<", std::to_string(base), ">()");
}

template<sn::concepts::tag... Tags>
inline std::string to_debug_string(Tags... tags) {
    return ((std::string(sn::type_name<Tags>()) + " ") + ...);
}

/**
 * Checks all ways of calling `sn::to_string` / `sn::from_string`, and that they agree with each other:
 * - The `bool` form with `nullptr` as the error.
 * - The `bool` form with an error, checking that the error is written on failure and left untouched on success.
 * - The `sn::expected` form, checking that `value()` throws an exception with the same message.
 */
template<class T>
class tester {
public:
    template<class... Tags>
    void expect_failing_to(const T &value, Tags... tags) {
        expect_failing_to_with_message(value, "", tags...);
    }

    template<class... Tags>
    void expect_failing_to(std::initializer_list<T> values, Tags... tags) {
        for (const T &value : values)
            expect_failing_to(value, tags...);
    }

    template<class... Tags>
    void expect_failing_to_with_message(const T &value, std::string_view message, Tags... tags) {
        SCOPED_TRACE(::testing::Message() << "with value = " << value << " and tags = " << to_debug_string(tags...));
        std::string result;
        std::optional<sn::error> error = run_to(value, &result, tags...);
        ASSERT_TRUE(error.has_value());
        EXPECT_TRUE(error->what().contains(message)) << "with error = " << error->what() << " and message = " << message;
    }

    template<class... Tags>
    void expect_failing_to_with_message(std::initializer_list<std::pair<T, std::string_view>> pairs, Tags... tags) {
        for (const auto &[value, message] : pairs)
            expect_failing_to_with_message(value, message, tags...);
    }

    template<class... Tags>
    void expect_succeeding_to(const T &value, Tags... tags) {
        SCOPED_TRACE(::testing::Message() << "with value = " << value << " and tags = " << to_debug_string(tags...));
        std::string result;
        std::optional<sn::error> error = run_to(value, &result, tags...);
        EXPECT_FALSE(error.has_value()) << "with error = " << error.value_or(sn::error()).what();
    }

    template<class... Tags>
    void expect_succeeding_to(std::initializer_list<T> values, Tags... tags) {
        for (const T &value : values)
            expect_succeeding_to(value, tags...);
    }

    template<class... Tags>
    void expect_failing_from(std::string_view str, Tags... tags) {
        expect_failing_from_with_message(str, "", tags...);
    }

    template<class... Tags>
    void expect_failing_from(std::initializer_list<std::string_view> strs, Tags... tags) {
        for (std::string_view str : strs)
            expect_failing_from(str, tags...);
    }

    template<class... Tags>
    void expect_failing_from_with_message(std::string_view str, std::string_view message, Tags... tags) {
        SCOPED_TRACE(::testing::Message() << "with str = " << str << " and tags = " << to_debug_string(tags...));
        T result = T();
        std::optional<sn::error> error = run_from(str, &result, tags...);
        ASSERT_TRUE(error.has_value());
        EXPECT_TRUE(error->what().contains(message)) << "with error = " << error->what() << " and message = " << message;
    }

    template<class... Tags>
    void expect_succeeding_from(std::string_view str, Tags... tags) {
        SCOPED_TRACE(::testing::Message() << "with str = " << str << " and tags = " << to_debug_string(tags...));
        T result = T();
        std::optional<sn::error> error = run_from(str, &result, tags...);
        EXPECT_FALSE(error.has_value()) << "with error = " << error.value_or(sn::error()).what();
    }

    template<class... Tags>
    void expect_succeeding_from(std::initializer_list<std::string_view> strs, Tags... tags) {
        for (std::string_view str : strs)
            expect_succeeding_from(str, tags...);
    }

    template<class... Tags>
    void expect_valid_from(std::string_view str, const T &value, Tags... tags) {
        SCOPED_TRACE(::testing::Message() << "with str = " << str << " and tags = " << to_debug_string(tags...));
        T result = T();
        std::optional<sn::error> error = run_from(str, &result, tags...);
        EXPECT_FALSE(error.has_value()) << "with error = " << error.value_or(sn::error()).what();
        EXPECT_EQ(result, value);
    }

    template<class... Tags>
    void expect_valid_from(std::initializer_list<std::pair<std::string_view, T>> pairs, Tags... tags) {
        for (const auto &[str, value] : pairs)
            expect_valid_from(str, value, tags...);
    }

    template<class... Tags>
    void expect_valid_to(const T &value, std::string_view str, Tags... tags) {
        SCOPED_TRACE(::testing::Message() << "with value = " << value << " and tags = " << to_debug_string(tags...));
        std::string result;
        std::optional<sn::error> error = run_to(value, &result, tags...);
        EXPECT_FALSE(error.has_value()) << "with error = " << error.value_or(sn::error()).what();
        EXPECT_EQ(result, str);
    }

    template<class... Tags>
    void expect_valid_to(std::initializer_list<std::pair<T, std::string_view>> pairs, Tags... tags) {
        for (const auto &[value, str] : pairs)
            expect_valid_to(value, str, tags...);
    }

    template<class... Tags>
    void expect_valid_fromto(std::string_view str, const T &value, Tags... tags) {
        expect_valid_from(str, value, tags...);
        expect_valid_to(value, str, tags...);
    }

    template<class... Tags>
    void expect_valid_fromto(std::initializer_list<std::pair<std::string_view, T>> pairs, Tags... tags) {
        for (const auto &[str, value] : pairs)
            expect_valid_fromto(str, value, tags...);
    }

    template<class... Tags>
    void expect_valid_roundtrip(const T &value, Tags... tags) {
        SCOPED_TRACE(::testing::Message() << "with value = " << value << " and tags = " << to_debug_string(tags...));
        std::string str;
        std::optional<sn::error> to_error = run_to(value, &str, tags...);
        EXPECT_FALSE(to_error.has_value()) << "with error = " << to_error.value_or(sn::error()).what();

        T result = T();
        std::optional<sn::error> from_error = run_from(str, &result, tags...);
        EXPECT_FALSE(from_error.has_value()) << "with error = " << from_error.value_or(sn::error()).what();
        EXPECT_EQ(result, value);
    }

    template<class... Tags>
    void expect_valid_roundtrip(std::initializer_list<T> values, Tags... tags) {
        for (const T &value : values)
            expect_valid_roundtrip(value, tags...);
    }

private:
    static constexpr std::string_view untouched = "<untouched>";

    template<class F>
    static std::string thrown_message(F &&f) {
        try {
            std::forward<F>(f)();
        } catch (const sn::bad_expected_access &e) {
            return e.what();
        }
        return "<nothing thrown>";
    }

    /**
     * Runs all forms of `sn::to_string` and checks that they agree.
     *
     * @return                          Reported error, or `std::nullopt` on success.
     */
    template<class... Tags>
    static std::optional<sn::error> run_to(const T &value, std::string *result, Tags... tags) {
        std::string nullptr_result = "<uninitialized>";
        bool nullptr_success = sn::to_string(value, &nullptr_result, nullptr, tags...);

        std::string error_result = "<uninitialized>";
        sn::error error{std::string(untouched)};
        bool error_success = sn::to_string(value, &error_result, &error, tags...);

        sn::expected<std::string> expected_result = sn::to_string(value, tags...);

        EXPECT_EQ(nullptr_success, error_success);
        EXPECT_EQ(error_success, expected_result.has_value());
        if (error_success) {
            EXPECT_EQ(error.message(), untouched); // Must not be touched on success.
            EXPECT_EQ(nullptr_result, error_result);
            if (expected_result.has_value())
                EXPECT_EQ(*expected_result, error_result);
            *result = error_result;
            return std::nullopt;
        }

        EXPECT_NE(error.message(), untouched);
        EXPECT_FALSE(error.message().empty());
        if (!expected_result.has_value()) {
            EXPECT_TRUE(expected_result.error() == error);
            EXPECT_EQ(thrown_message([&] { (void) expected_result.value(); }), error.what());
        }
        return error;
    }

    /**
     * Runs all forms of `sn::from_string` and checks that they agree.
     *
     * @return                          Reported error, or `std::nullopt` on success.
     */
    template<class... Tags>
    static std::optional<sn::error> run_from(std::string_view str, T *result, Tags... tags) {
        T nullptr_result = T();
        bool nullptr_success = sn::from_string(str, &nullptr_result, nullptr, tags...);

        T error_result = T();
        sn::error error{std::string(untouched)};
        bool error_success = sn::from_string(str, &error_result, &error, tags...);

        sn::expected<T> expected_result = sn::from_string<T>(str, tags...);

        EXPECT_EQ(nullptr_success, error_success);
        EXPECT_EQ(error_success, expected_result.has_value());
        if (error_success) {
            EXPECT_EQ(error.message(), untouched); // Must not be touched on success.
            EXPECT_EQ(nullptr_result, error_result);
            if (expected_result.has_value())
                EXPECT_EQ(*expected_result, error_result);
            *result = error_result;
            return std::nullopt;
        }

        EXPECT_NE(error.message(), untouched);
        EXPECT_FALSE(error.message().empty());
        if (!expected_result.has_value()) {
            EXPECT_TRUE(expected_result.error() == error);
            EXPECT_EQ(thrown_message([&] { (void) expected_result.value(); }), error.what());
        }
        return error;
    }
};

} // namespace sn::detail
