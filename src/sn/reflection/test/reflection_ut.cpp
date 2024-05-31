#include <gtest/gtest.h>

#include "sn/core/tag.h"
#include "sn/reflection/class_reflection.h"

struct MyPair {
    int a = 0;
    int b = 0;

    void setB(const int &value) {
        b = value;
    }

    int getB() const {
        return b;
    }
};

struct some_tag : public sn::tags::tag {};

SN_DEFINE_CLASS_REFLECTION(MyPair, (
    (&MyPair::a, "a", some_tag()),
    (&MyPair::getB, &MyPair::setB, "b", some_tag()),
    (&MyPair::b, "bbb")
));

TEST(reflection, simple) {
    auto refl = reflect_class(std::type_identity<MyPair>());

    MyPair tmp;

    get<0>(refl.fields).setter(tmp, 1);
    EXPECT_EQ(get<0>(refl.fields).getter(tmp), 1);
    get<0>(refl.fields).setter(tmp, 2);
    EXPECT_EQ(get<0>(refl.fields).getter(tmp), 2);

    get<1>(refl.fields).setter(tmp, 1);
    EXPECT_EQ(get<1>(refl.fields).getter(tmp), 1);
    EXPECT_EQ(get<2>(refl.fields).getter(tmp), 1);
    get<1>(refl.fields).setter(tmp, 2);
    EXPECT_EQ(get<1>(refl.fields).getter(tmp), 2);
    EXPECT_EQ(get<2>(refl.fields).getter(tmp), 2);
}
