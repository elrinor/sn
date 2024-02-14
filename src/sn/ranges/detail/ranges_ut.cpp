#include <vector>
#include <set>

#include <gtest/gtest.h> // NOLINT: not a C system header.

#include "sn/ranges/clear.h"
#include "sn/ranges/insert.h"

class clearable_vector {
public:
    void clear() {
        _values.clear();
    }

    void refill() {
        _values = {0, 1, 2};
    }

    void push_back(int value) {
        _values.push_back(value);
    }

    [[nodiscard]] bool empty() const {
        return _values.empty();
    }

private:
    std::vector<int> _values;
};

struct weird_container {
    std::vector<int> values;

    friend void clear(weird_container &self) { // NOLINT: intentional lvalue ref.
        self.values.clear();
    }

    friend void insert(weird_container &self, int value) { // NOLINT: intentional lvalue ref.
        self.values.push_back(value);
    }
};

TEST(ranges, clear) {
    std::vector<int> v = {0, 1, 2};
    EXPECT_FALSE(v.empty());
    sn::ranges::clear(v);
    EXPECT_TRUE(v.empty());

    std::set<int> s = {0, 1, 2};
    EXPECT_FALSE(s.empty());
    sn::ranges::clear(s);
    EXPECT_TRUE(s.empty());

    // Test custom clear() method.
    clearable_vector c;
    c.refill();
    EXPECT_FALSE(c.empty());
    sn::ranges::clear(c);
    EXPECT_TRUE(c.empty());

    // Test adl-findable clear().
    weird_container w;
    w.values.push_back(1);
    EXPECT_FALSE(w.values.empty());
    sn::ranges::clear(w);
    EXPECT_TRUE(w.values.empty());
}

TEST(ranges, insert) {
    std::vector<int> v;
    EXPECT_TRUE(v.empty());
    sn::ranges::insert(v, 1);
    EXPECT_EQ(v, std::vector{1});

    std::set<int> s;
    EXPECT_TRUE(s.empty());
    sn::ranges::insert(s, 1);
    EXPECT_EQ(s, std::set{1});

    // Test custom push_back() method.
    clearable_vector c;
    EXPECT_TRUE(c.empty());
    sn::ranges::insert(c, 1);
    EXPECT_FALSE(c.empty());

    // Test adl-findable insert().
    weird_container w;
    EXPECT_TRUE(w.values.empty());
    sn::ranges::insert(w, 1);
    EXPECT_EQ(w.values, std::vector{1});
}






