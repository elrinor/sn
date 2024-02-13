#include <gtest/gtest.h>

#include "sn/core/preprocessor.h"

static_assert(SN_PP_TUPLE_SIZE(()) == 1);
static_assert(SN_PP_TUPLE_SIZE((1)) == 1);
static_assert(SN_PP_TUPLE_SIZE((1, 2, 3)) == 3);
static_assert(SN_PP_TUPLE_SIZE((1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64)) == 64);

static_assert(SN_PP_TUPLE_SIZE((SN_PP_REMOVE_PARENS((1, 2, 3)))) == 3);

#define JUST_1_2_3_4 1, 2, 3, 4
static_assert(SN_PP_TUPLE_SIZE((JUST_1_2_3_4)) == 4);

#define PREPEND_PLUS(x) +x
static_assert(SN_PP_TUPLE_FOR_EACH(PREPEND_PLUS, (1, 2, 3, 4, 5)) == 15);

TEST(core, pp_for_each) {
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

    SN_PP_TUPLE_FOR_EACH(INCREMENT_FIELD, SN_PP_REMOVE_PARENS(((a, b))))
    EXPECT_EQ(tmp.a, 2);
    EXPECT_EQ(tmp.b, 3);
    EXPECT_EQ(tmp.c, 3);
}
