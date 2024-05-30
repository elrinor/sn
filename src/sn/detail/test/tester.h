#pragma once

#include <vector>
#include <utility> // For std::pair.
#include <string>
#include <tuple>

#include <gtest/gtest.h> // NOLINT: not a C system header.

namespace sn::detail {

template<class Ops>
class initializing_ops_wrapper {
public:
    explicit initializing_ops_wrapper(const Ops &base) : _base(base) {}

    template<class T, class... Tags>
    [[nodiscard]] bool try_to(const T &src, std::string *dst, Tags... tags) const noexcept {
        *dst = "<uninitialized>";
        return _base.try_to(src, dst, tags...);
    }

    template<class T, class... Tags>
    void to(const T &src, std::string *dst, Tags... tags) const {
        *dst = "<uninitialized>";
        return _base.to(src, dst, tags...);
    }

    template<class T, class... Tags>
    [[nodiscard]] bool try_from(std::string_view src, T *dst, Tags... tags) const noexcept {
        *dst = T();
        return _base.try_from(src, dst, tags...);
    }

    template<class T, class... Tags>
    void from(std::string_view src, T *dst, Tags... tags) const {
        *dst = T();
        return _base.from(src, dst, tags...);
    }

private:
    Ops _base;
};

template<class T, class Ops>
class tester {
public:
    explicit tester(const Ops &ops) : _ops(ops) {}

    template<class... Tags>
    void expect_throwing_to(const std::type_identity_t<T> &value, Tags... tags) {
        EXPECT_ANY_THROW(_ops.to(value, &_tmps, tags...)) << "with value = " << value;
        EXPECT_FALSE(_ops.try_to(value, &_tmps, tags...)) << "with value = " << value;
    }

    template<class... Tags>
    void expect_throwing_to(std::initializer_list<std::type_identity_t<T>> values, Tags... tags) {
        for (const T &value : values)
            expect_throwing_to(value, tags...);
    }

    template<class... Tags>
    void expect_throwing_to_with_message(const std::type_identity_t<T> &value, std::string_view message, Tags... tags) {
        EXPECT_ANY_THROW(_ops.to(value, &_tmps, tags...)) << "with value = " << value;
        EXPECT_FALSE(_ops.try_to(value, &_tmps, tags...)) << "with value = " << value;
        try {
            (void) _ops.to(value, &_tmps, tags...);
        } catch (const std::exception &e) {
            EXPECT_NE(std::string_view(e.what()).find(message), std::string_view::npos) << "with e.what() = " << e.what() << " and message = " << message;
        }
    }

    template<class... Tags>
    void expect_throwing_to_with_message(std::initializer_list<std::pair<std::type_identity_t<T>, std::string_view>> pairs, Tags... tags) {
        for (const auto &[value, message] : pairs)
            expect_throwing_to_with_message(value, message, tags...);
    }

    template<class... Tags>
    void expect_throwing_from(std::string_view str, Tags... tags) {
        EXPECT_ANY_THROW(_ops.from(str, &_tmpv, tags...)) << "with str = " << str;
        EXPECT_FALSE(_ops.try_from(str, &_tmpv, tags...)) << "with str = " << str;
    }

    template<class... Tags>
    void expect_throwing_from(std::initializer_list<std::string_view> strs, Tags... tags) {
        for (std::string_view str : strs)
            expect_throwing_from(str, tags...);
    }

    template<class... Tags>
    void expect_valid_from(std::string_view str, const std::type_identity_t<T> &value, Tags... tags) {
        EXPECT_NO_THROW(_ops.from(str, &_tmpv, tags...)) << "with str = " << str;
        EXPECT_EQ(_tmpv, value) << "with str = " << str;
        EXPECT_TRUE(_ops.try_from(str, &_tmpv, tags...)) << "with str = " << str;
        EXPECT_EQ(_tmpv, value) << "with str = " << str;
    }

    template<class... Tags>
    void expect_valid_from(std::initializer_list<std::pair<std::string_view, std::type_identity_t<T>>> pairs, Tags... tags) {
        for (const auto &[str, value] : pairs)
            expect_valid_from(str, value, tags...);
    }

    template<class... Tags>
    void expect_valid_to(const std::type_identity_t<T> &value, std::string_view str, Tags... tags) {
        EXPECT_NO_THROW(_ops.to(value, &_tmps, tags...)) << "with value = " << value;
        EXPECT_EQ(_tmps, str) << "with value = " << value;
        EXPECT_TRUE(_ops.try_to(value, &_tmps, tags...)) << "with value = " << value;
        EXPECT_EQ(_tmps, str) << "with value = " << value;
    }

    template<class... Tags>
    void expect_valid_to(std::initializer_list<std::pair<std::type_identity_t<T>, std::string_view>> pairs, Tags... tags) {
        for (const auto &[value, str] : pairs)
            expect_valid_to(value, str, tags...);
    }

    template<class... Tags>
    void expect_valid_fromto(std::string_view str, const std::type_identity_t<T> &value, Tags... tags) {
        expect_valid_from(str, value, tags...);
        expect_valid_to(value, str, tags...);
    }

    template<class... Tags>
    void expect_valid_fromto(std::initializer_list<std::pair<std::string_view, std::type_identity_t<T>>> pairs, Tags... tags) {
        for (const auto &[str, value] : pairs)
            expect_valid_fromto(str, value, tags...);
    }

    template<class... Tags>
    void expect_valid_roundtrip(const std::type_identity_t<T> &value, Tags... tags) {
        EXPECT_NO_THROW(_ops.to(value, &_tmps, tags...)) << "with value = " << value;
        EXPECT_NO_THROW(_ops.from(_tmps, &_tmpv, tags...)) << "with value = " << value;
        EXPECT_EQ(_tmpv, value) << "with value = " << value;

        EXPECT_TRUE(_ops.try_to(value, &_tmps, tags...)) << "with value = " << value;
        EXPECT_TRUE(_ops.try_from(_tmps, &_tmpv, tags...)) << "with value = " << value;
        EXPECT_EQ(_tmpv, value) << "with value = " << value;
    }

    template<class... Tags>
    void expect_valid_roundtrip(std::initializer_list<std::type_identity_t<T>> values, Tags... tags) {
        for (const T &value : values)
            expect_valid_roundtrip(value, tags...);
    }

private:
    initializing_ops_wrapper<Ops> _ops;
    std::string _tmps;
    T _tmpv = {};
};

} // namespace sn::detail
