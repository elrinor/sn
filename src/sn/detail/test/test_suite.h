#pragma once

#include <vector>
#include <utility> // For std::pair.
#include <string>
#include <tuple>

#include <gtest/gtest.h> // NOLINT: not a C system header.

namespace sn::detail {

template<class S>
struct test_stringifier;

template<class T, class S>
struct test_serializer;

template<class T, class... Tags>
struct test_suite {
    std::vector<T> throwing_to;
    std::vector<std::pair<T, std::string>> throwing_to_message;
    std::vector<std::string> throwing_from;

    std::vector<std::pair<std::string, T>> valid_from;
    std::vector<std::pair<std::string, T>> valid_fromto;
    std::vector<T> valid_roundtrip;

    std::tuple<Tags...> tags;

    test_suite() = default;
    explicit test_suite(Tags... tags) requires (sizeof...(Tags) > 0) : tags(tags...) {}

    template<class S>
    void run() const {
        std::apply([&](Tags... tags) { do_run<S>(tags...); }, tags);
    }

private:
    template<class S>
    void do_run(Tags... tags) const {
        T tmpv;
        S tmps;

        test_stringifier<S> _;
        test_serializer<T, S> sn;

        for (const T &v : throwing_to) {
            EXPECT_ANY_THROW((void) sn.to(v, tags...)) << "with v = " << v;
            EXPECT_FALSE(sn.try_to(v, &tmps, tags...)) << "with v = " << v;
        }

        for (const auto &[v, m] : throwing_to_message) {
            EXPECT_ANY_THROW((void) sn.to(v, tags...)) << "with v = " << v;
            EXPECT_FALSE(sn.try_to(v, &tmps, tags...)) << "with v = " << v;
            try {
                (void) sn.to(v);
            } catch (const std::exception &e) {
                EXPECT_NE(std::string_view(e.what()).find(m), std::string_view::npos) << "with e.what() = " << e.what() << " and m = " << m;
            }
        }

        for (const std::string &s : throwing_from) {
            EXPECT_ANY_THROW((void) sn.from(_(s), tags...)) << "with s = " << s;
            EXPECT_FALSE(sn.try_from(_(s), &tmpv, tags...)) << "with s = " << s;
        }

        for (const auto &[s, v] : valid_from) {
            EXPECT_EQ(sn.from(_(s), tags...), v);
            EXPECT_TRUE(sn.try_from(_(s), &tmpv, tags...)) << "with s = " << s;
            EXPECT_EQ(tmpv, v);
        }

        for (const auto &[s, v] : valid_fromto) {
            EXPECT_EQ(sn.to(v, tags...), _(s)) << "with v = " << v;
            EXPECT_TRUE(sn.try_to(v, &tmps, tags...)) << "with v = " << v;
            EXPECT_EQ(tmps, _(s));
            EXPECT_EQ(sn.from(_(s), tags...), v) << "with s = " << s;
            EXPECT_TRUE(sn.try_from(_(s), &tmpv, tags...)) << "with s = " << s;
            EXPECT_EQ(tmpv, v);
        }

        for (const T &v : valid_roundtrip) {
            EXPECT_EQ(sn.from(sn.to(v, tags...), tags...), v) << "with v = " << v;
            EXPECT_TRUE(sn.try_to(v, &tmps, tags...)) << "with v = " << v;
            EXPECT_TRUE(sn.try_from(tmps, &tmpv, tags...)) << "with tmps = " << tmps;
            EXPECT_EQ(tmpv, v);
        }
    }
};

} // namespace sn::detail
