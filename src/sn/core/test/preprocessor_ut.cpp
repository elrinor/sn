#include <map>

#include <gtest/gtest.h> // NOLINT: not a C system header.

#include "sn/core/preprocessor.h"

// Static tests for SN_PP_IF
static_assert(SN_PP_IF(0, 1, 0) == 0);
static_assert(SN_PP_IF(1, 1, 0) == 1);
static_assert(SN_PP_IF(100, 1, 0) == 1);

// Static tests for SN_PP_TUPLE_SIZE
static_assert(SN_PP_TUPLE_SIZE(()) == 0);
#define EMPTY3 EMPTY2
#define EMPTY2 EMPTY1
#define EMPTY1 EMPTY0
#define EMPTY0 SN_PP_EMPTY
static_assert(SN_PP_TUPLE_SIZE((EMPTY3())) == 0);
static_assert(SN_PP_TUPLE_SIZE((1)) == 1);
static_assert(SN_PP_TUPLE_SIZE((1, 2, 3)) == 3);
static_assert(SN_PP_TUPLE_SIZE((1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64)) == 64);
static_assert(SN_PP_TUPLE_SIZE((SN_PP_TUPLE_ENUM((1, 2, 3)))) == 3);
#define JUST_1_2_3_4 1, 2, 3, 4
static_assert(SN_PP_TUPLE_SIZE((JUST_1_2_3_4)) == 4);

// Static tests for SN_PP_TUPLE_FOR_EACH
#define PREPEND_PLUS(x) +x
static_assert(SN_PP_TUPLE_FOR_EACH(PREPEND_PLUS, (1, 2, 3, 4, 5)) == 15);
static_assert(SN_PP_TUPLE_FOR_EACH(PREPEND_PLUS, ()) true); // SN_PP_TUPLE_FOR_EACH should expand to nothing here.
static_assert(SN_PP_TUPLE_FOR_EACH(PREPEND_PLUS, (EMPTY3())) true); // And here.

TEST(core, pp_for_each_completeness) {
    struct tmp_data {
        int a = 0;
        int b = 1;
        int c = 2;
    };

    tmp_data tmp;

#define INCREMENT_FIELD(x) tmp.x++;
    SN_PP_TUPLE_FOR_EACH(INCREMENT_FIELD, (a, b, c));
    EXPECT_EQ(tmp.a, 1);
    EXPECT_EQ(tmp.b, 2);
    EXPECT_EQ(tmp.c, 3);

    SN_PP_TUPLE_FOR_EACH(INCREMENT_FIELD, SN_PP_TUPLE_ENUM(((a, b))))
    EXPECT_EQ(tmp.a, 2);
    EXPECT_EQ(tmp.b, 3);
    EXPECT_EQ(tmp.c, 3);
}

TEST(core, pp_for_each_order) {
#define LIST_NUMBER(x) x,
    std::initializer_list<int> list = {
        SN_PP_TUPLE_FOR_EACH(LIST_NUMBER, (1, 2, 3))
    };

    EXPECT_EQ(list.size(), 3);
    EXPECT_EQ(*list.begin(), 1);
    EXPECT_EQ(*(list.begin() + 1), 2);
    EXPECT_EQ(*(list.begin() + 2), 3);
}

TEST(core, pp_for_each_i) {
#define LIST_PAIR(i, x) {i, x},
    std::map<int, int> mapping = {
        SN_PP_TUPLE_FOR_EACH_I(LIST_PAIR, (1, 2, 3))
    };

    EXPECT_EQ(mapping.size(), 3);
    EXPECT_EQ(mapping[0], 1);
    EXPECT_EQ(mapping[1], 2);
    EXPECT_EQ(mapping[2], 3);
}
