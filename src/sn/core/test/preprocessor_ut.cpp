#include <string_view>

#include "sn/core/preprocessor.h"

#define SN_PP_STATIC_TEST(MACRO, RESULT)                                                                                \
    _SN_PP_STATIC_TEST_I(MACRO, RESULT, SN_PP_CAT(left_, __LINE__), SN_PP_CAT(right_, __LINE__))
#define _SN_PP_STATIC_TEST_I(MACRO, RESULT, MACRO_VAR, RESULT_VAR)                                                      \
    static constexpr char MACRO_VAR[] = SN_PP_STRINGIZE(MACRO);                                                         \
    static constexpr char RESULT_VAR[] = RESULT;                                                                        \
    static_assert(std::string_view(MACRO_VAR) == std::string_view(RESULT_VAR));

// Static tests for SN_PP_IF
SN_PP_STATIC_TEST(SN_PP_IF(1, T, F), "T");
SN_PP_STATIC_TEST(SN_PP_IF(0, T, F), "F");
SN_PP_STATIC_TEST(SN_PP_IF(100, T, F), "T");

// Static tests for SN_PP_TUPLE_SIZE
SN_PP_STATIC_TEST(SN_PP_TUPLE_SIZE(()), "0");
#define EMPTY3 EMPTY2
#define EMPTY2 EMPTY1
#define EMPTY1 EMPTY0
#define EMPTY0 SN_PP_EMPTY
SN_PP_STATIC_TEST(SN_PP_TUPLE_SIZE((EMPTY3())), "0");
SN_PP_STATIC_TEST(SN_PP_TUPLE_SIZE((1)), "1");
SN_PP_STATIC_TEST(SN_PP_TUPLE_SIZE((1, 2, 3)), "3");
SN_PP_STATIC_TEST(SN_PP_TUPLE_SIZE((1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64)), "64");
SN_PP_STATIC_TEST(SN_PP_TUPLE_SIZE((SN_PP_TUPLE_ENUM((1, 2, 3)))), "3");
#define JUST_1_2_3_4 1, 2, 3, 4
SN_PP_STATIC_TEST(SN_PP_TUPLE_SIZE((JUST_1_2_3_4)), "4");
SN_PP_STATIC_TEST(SN_PP_TUPLE_SIZE((0, JUST_1_2_3_4)), "5");

// Static tests for SN_PP_TUPLE_FOR_EACH
#define SUM_ONE(X) +X
SN_PP_STATIC_TEST(SN_PP_TUPLE_FOR_EACH(SUM_ONE, ()), "");
SN_PP_STATIC_TEST(SN_PP_TUPLE_FOR_EACH(SUM_ONE, (EMPTY3())), "");
SN_PP_STATIC_TEST(SN_PP_TUPLE_FOR_EACH(SUM_ONE, (1)), "+1");
SN_PP_STATIC_TEST(SN_PP_TUPLE_FOR_EACH(SUM_ONE, (1, 2, 3, 4, 5)), "+1 +2 +3 +4 +5");

// Static tests for SN_PP_TUPLE_FOR_EACH_I
#define SUM_TWO(A, B) +A+B
SN_PP_STATIC_TEST(SN_PP_TUPLE_FOR_EACH_I(SUM_TWO, ()), "");
SN_PP_STATIC_TEST(SN_PP_TUPLE_FOR_EACH_I(SUM_TWO, (EMPTY3())), "");
SN_PP_STATIC_TEST(SN_PP_TUPLE_FOR_EACH_I(SUM_TWO, (1)), "+0+1");
SN_PP_STATIC_TEST(SN_PP_TUPLE_FOR_EACH_I(SUM_TWO, (1, 2, 3, 4, 5)), "+0+1 +1+2 +2+3 +3+4 +4+5");



// Static tests for SN_PP_TUPLE_TRANSFORM
SN_PP_STATIC_TEST(SN_PP_TUPLE_TRANSFORM(SUM_ONE, ()), "()");
SN_PP_STATIC_TEST(SN_PP_TUPLE_TRANSFORM(SUM_ONE, (EMPTY3())), "()");
SN_PP_STATIC_TEST(SN_PP_TUPLE_TRANSFORM(SUM_ONE, (1)), "(+1)");
SN_PP_STATIC_TEST(SN_PP_TUPLE_TRANSFORM(SUM_ONE, (1, 2, 3, 4, 5)), "(+1, +2, +3, +4, +5)");
