#pragma once

#include <vector>
#include <utility> // For std::pair.
#include <string>
#include <tuple>

#include <gtest/gtest.h> // NOLINT: not a C system header.

namespace sn::detail {

template<class T, class S, class... Tags>
struct test_suite {
    std::vector<T> throwing_to;
    std::vector<std::pair<T, std::string>> throwing_to_message;
    std::vector<S> throwing_from;

    std::vector<std::pair<S, T>> valid_from;
    std::vector<std::pair<S, T>> valid_fromto;
    std::vector<T> valid_roundtrip;

    std::tuple<Tags...> tags;

    test_suite() = default;
    explicit test_suite(Tags... tags) requires (sizeof...(Tags) > 0) : tags(tags...) {}

    template<class Callback>
    void run(Callback callback) const {
        [&]<std::size_t... indices>(std::index_sequence<indices...>) {
            do_run(callback, std::get<indices>(this->tags)...);
        }(std::index_sequence_for<Tags...>());
    }

private:
    template<class Callback>
    void do_run(Callback callback, Tags... tags) const {
        T tmpv;
        S tmps;

        for (const T &v : throwing_to) {
            EXPECT_ANY_THROW((void) callback.to(v, tags...)) << "with v = " << v;
            EXPECT_FALSE(callback.try_to(v, &tmps, tags...)) << "with v = " << v;
        }

        for (const auto &[v, m] : throwing_to_message) {
            EXPECT_ANY_THROW((void) callback.to(v, tags...)) << "with v = " << v;
            EXPECT_FALSE(callback.try_to(v, &tmps, tags...)) << "with v = " << v;
            try {
                (void) callback.to(v);
            } catch (const std::exception &e) {
                EXPECT_NE(std::string_view(e.what()).find(m), std::string_view::npos) << "with e.what() = " << e.what() << " and m = " << m;
            }
        }

        for (const S &s : throwing_from) {
            EXPECT_ANY_THROW((void) callback.template from<T>(s, tags...)) << "with s = " << s;
            EXPECT_FALSE(callback.try_from(s, &tmpv, tags...)) << "with s = " << s;
        }

        for (const auto &[s, v] : valid_from) {
            EXPECT_EQ(callback.template from<T>(s, tags...), v);
            EXPECT_TRUE(callback.try_from(s, &tmpv, tags...)) << "with s = " << s;
            EXPECT_EQ(tmpv, v);
        }

        for (const auto &[s, v] : valid_fromto) {
            EXPECT_EQ(callback.to(v, tags...), s) << "with v = " << v;
            EXPECT_TRUE(callback.try_to(v, &tmps, tags...)) << "with v = " << v;
            EXPECT_EQ(tmps, s);
            EXPECT_EQ(callback.template from<T>(s, tags...), v) << "with s = " << s;
            EXPECT_TRUE(callback.try_from(s, &tmpv, tags...)) << "with s = " << s;
            EXPECT_EQ(tmpv, v);
        }

        for (const T &v : valid_roundtrip) {
            EXPECT_EQ(callback.template from<T>(callback.to(v, tags...), tags...), v) << "with v = " << v;
            EXPECT_TRUE(callback.try_to(v, &tmps, tags...)) << "with v = " << v;
            EXPECT_TRUE(callback.try_from(tmps, &tmpv, tags...)) << "with tmps = " << tmps;
            EXPECT_EQ(tmpv, v);
        }
    }
};

} // namespace sn::detail
