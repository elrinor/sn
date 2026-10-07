#pragma once

#include <vector>
#include <utility> // For std::pair.
#include <string>
#include <tuple>
#include <type_traits>

#include <gtest/gtest.h> // NOLINT: not a C system header.

#include "sn/core/type_name.h"
#include "sn/string/string.h"
#include "sn/string/string_tags.h"
#include "sn/detail/format/format.h"

namespace sn::detail {

// TODO(elric): sn/debug
inline std::string to_debug_string() {
    return "<none>";
}

inline std::string to_debug_string(sn::tags::dynamic_base_tag tag) {
    return sn::detail::format("sn::tags::dynamic_base_tag({})", tag.value());
}

template<int base>
inline std::string to_debug_string(sn::tags::base_tag<base>) {
    return sn::detail::format("sn::tags::base_tag<{}>()", base);
}

template<sn::concepts::tag... Tags>
inline std::string to_debug_string(Tags... tags) {
    return ((std::string(sn::type_name<Tags>()) + " ") + ...);
}

template<class T>
class tester {
public:
    template<class... Tags>
    void expect_throwing_to(const T &value, Tags... tags) {
        EXPECT_ANY_THROW(to(value, &_tmps, tags...)) << "with value = " << value << " and tags = " << to_debug_string(tags...);
        EXPECT_FALSE(try_to(value, &_tmps, tags...)) << "with value = " << value << " and tags = " << to_debug_string(tags...);
    }

    template<class... Tags>
    void expect_throwing_to(std::initializer_list<T> values, Tags... tags) {
        for (const T &value : values)
            expect_throwing_to(value, tags...);
    }

    template<class... Tags>
    void expect_nonthrowing_to(const T &value, Tags... tags) {
        EXPECT_NO_THROW(to(value, &_tmps, tags...)) << "with value = " << value << " and tags = " << to_debug_string(tags...);
        EXPECT_TRUE(try_to(value, &_tmps, tags...)) << "with value = " << value << " and tags = " << to_debug_string(tags...);
    }

    template<class... Tags>
    void expect_nonthrowing_to(std::initializer_list<T> values, Tags... tags) {
        for (const T &value : values)
            expect_nonthrowing_to(value, tags...);
    }

    template<class... Tags>
    void expect_throwing_to_with_message(const T &value, std::string_view message, Tags... tags) {
        EXPECT_ANY_THROW(to(value, &_tmps, tags...)) << "with value = " << value << " and tags = " << to_debug_string(tags...);
        EXPECT_FALSE(try_to(value, &_tmps, tags...)) << "with value = " << value << " and tags = " << to_debug_string(tags...);
        try {
            (void) to(value, &_tmps, tags...);
        } catch (const std::exception &e) {
            EXPECT_NE(std::string_view(e.what()).find(message), std::string_view::npos) << "with e.what() = " << e.what() << ", message = " << message << " and tags = " << to_debug_string(tags...);
        }
    }

    template<class... Tags>
    void expect_throwing_to_with_message(std::initializer_list<std::pair<T, std::string_view>> pairs, Tags... tags) {
        for (const auto &[value, message] : pairs)
            expect_throwing_to_with_message(value, message, tags...);
    }

    template<class... Tags>
    void expect_throwing_from(std::string_view str, Tags... tags) {
        EXPECT_ANY_THROW(from(str, &_tmpv, tags...)) << "with str = " << str << " and tags = " << to_debug_string(tags...);
        EXPECT_FALSE(try_from(str, &_tmpv, tags...)) << "with str = " << str << " and tags = " << to_debug_string(tags...);
    }

    template<class... Tags>
    void expect_throwing_from(std::initializer_list<std::string_view> strs, Tags... tags) {
        for (std::string_view str : strs)
            expect_throwing_from(str, tags...);
    }

    template<class... Tags>
    void expect_nonthrowing_from(std::string_view str, Tags... tags) {
        EXPECT_NO_THROW(from(str, &_tmpv, tags...)) << "with str = " << str << " and tags = " << to_debug_string(tags...);
        EXPECT_TRUE(try_from(str, &_tmpv, tags...)) << "with str = " << str << " and tags = " << to_debug_string(tags...);
    }

    template<class... Tags>
    void expect_nonthrowing_from(std::initializer_list<std::string_view> strs, Tags... tags) {
        for (std::string_view str : strs)
            expect_nonthrowing_from(str, tags...);
    }

    template<class... Tags>
    void expect_valid_from(std::string_view str, const T &value, Tags... tags) {
        reset_value(value);
        EXPECT_NO_THROW(from(str, &_tmpv, tags...)) << "with str = " << str << " and tags = " << to_debug_string(tags...);
        EXPECT_EQ(_tmpv, value) << "with str = " << str;
        reset_value(value);
        EXPECT_TRUE(try_from(str, &_tmpv, tags...)) << "with str = " << str << " and tags = " << to_debug_string(tags...);
        EXPECT_EQ(_tmpv, value) << "with str = " << str;
    }

    template<class... Tags>
    void expect_valid_from(std::initializer_list<std::pair<std::string_view, T>> pairs, Tags... tags) {
        for (const auto &[str, value] : pairs)
            expect_valid_from(str, value, tags...);
    }

    template<class... Tags>
    void expect_valid_to(const T &value, std::string_view str, Tags... tags) {
        EXPECT_NO_THROW(to(value, &_tmps, tags...)) << "with value = " << value << " and tags = " << to_debug_string(tags...);
        EXPECT_EQ(_tmps, str) << "with value = " << value;
        EXPECT_TRUE(try_to(value, &_tmps, tags...)) << "with value = " << value << " and tags = " << to_debug_string(tags...);
        EXPECT_EQ(_tmps, str) << "with value = " << value;
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
        reset_value(value);
        EXPECT_NO_THROW(to(value, &_tmps, tags...)) << "with value = " << value << " and tags = " << to_debug_string(tags...);
        EXPECT_NO_THROW(from(_tmps, &_tmpv, tags...)) << "with value = " << value << " and tags = " << to_debug_string(tags...);
        EXPECT_EQ(_tmpv, value) << "with value = " << value << " and tags = " << to_debug_string(tags...);

        EXPECT_TRUE(try_to(value, &_tmps, tags...)) << "with value = " << value << " and tags = " << to_debug_string(tags...);
        EXPECT_TRUE(try_from(_tmps, &_tmpv, tags...)) << "with value = " << value << " and tags = " << to_debug_string(tags...);
        EXPECT_EQ(_tmpv, value) << "with value = " << value << " and tags = " << to_debug_string(tags...);
    }

    template<class... Tags>
    void expect_valid_roundtrip(std::initializer_list<T> values, Tags... tags) {
        for (const T &value : values)
            expect_valid_roundtrip(value, tags...);
    }

private:
    template<class... Tags>
    [[nodiscard]] static bool try_to(const T &src, std::string *dst, Tags... tags) noexcept {
        *dst = "<uninitialized>";
        return sn::try_to_string(src, dst, tags...);
    }

    template<class... Tags>
    static void to(const T &src, std::string *dst, Tags... tags) {
        *dst = "<uninitialized>";
        sn::to_string(src, dst, tags...);
    }

    template<class... Tags>
    [[nodiscard]] static bool try_from(std::string_view src, T *dst, Tags... tags) noexcept {
        return sn::try_from_string(src, dst, tags...);
    }

    template<class... Tags>
    static void from(std::string_view src, T *dst, Tags... tags) {
        sn::from_string(src, dst, tags...);
    }

    // Sets the value to something other than what's expected, so that a conversion that doesn't write anything is
    // noticed even if the expected value is T().
    void reset_value(const T &expected) {
        if constexpr (std::is_enum_v<T> || std::is_arithmetic_v<T>) {
            _tmpv = expected == T() ? static_cast<T>(1) : T();
        } else {
            _tmpv = T();
        }
    }

private:
    std::string _tmps;
    T _tmpv = {};
};

} // namespace sn::detail
