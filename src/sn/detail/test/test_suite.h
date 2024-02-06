#pragma once

#include <vector>
#include <utility>

#include <gtest/gtest.h> // NOLINT: not a C system header.

namespace sn::detail {

template<class T, class S>
struct test_suite {
    std::vector<T> throwing_to;
    std::vector<S> throwing_from;

    std::vector<std::pair<S, T>> valid_from;
    std::vector<std::pair<S, T>> valid_fromto;
    std::vector<T> valid_roundtrip;

    template<class Callback>
    void run(Callback callback) const {
        T tmpv;
        S tmps;

        for (const T &v : throwing_to) {
            EXPECT_ANY_THROW((void) callback.to(v)) << "with v = " << v;
            EXPECT_FALSE(callback.try_to(v, &tmps)) << "with v = " << v;
        }

        for (const S &s : throwing_from) {
            EXPECT_ANY_THROW((void) callback.from(s)) << "with s = " << s;
            EXPECT_FALSE(callback.try_from(s, &tmpv)) << "with s = " << s;
        }

        for (const auto &[s, v] : valid_from) {
            EXPECT_EQ(callback.from(s), v);
            EXPECT_TRUE(callback.try_from(s, &tmpv)) << "with s = " << s;
            EXPECT_EQ(tmpv, v);
        }

        for (const auto &[s, v] : valid_fromto) {
            EXPECT_EQ(callback.to(v), s) << "with v = " << v;
            EXPECT_TRUE(callback.try_to(v, &tmps)) << "with v = " << v;
            EXPECT_EQ(tmps, s);
            EXPECT_EQ(callback.from(s), v) << "with s = " << s;
            EXPECT_TRUE(callback.try_from(s, &tmpv)) << "with s = " << s;
            EXPECT_EQ(tmpv, v);
        }

        for (const T &v : valid_roundtrip) {
            EXPECT_EQ(callback.from(callback.to(v)), v) << "with v = " << v;
            EXPECT_TRUE(callback.try_to(v, &tmps)) << "with v = " << v;
            EXPECT_TRUE(callback.try_from(tmps, &tmpv)) << "with tmps = " << tmps;
            EXPECT_EQ(tmpv, v);
        }
    }
};

} // namespace sn::detail
