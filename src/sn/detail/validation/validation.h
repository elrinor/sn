#pragma once

#include "sn/core/preprocessor.h"

#define _SN_DEFINE_VALIDATION_FUNCTIONS(TO_NAME, FROM_NAME, TO_ARG, FROM_ARG)                                           \
    _SN_DEFINE_VALIDATION_FUNCTIONS_I(SN_PP_CAT(try_, TO_NAME), TO_NAME, SN_PP_CAT(try_, FROM_NAME), FROM_NAME, TO_ARG, FROM_ARG)

#define _SN_DEFINE_VALIDATION_FUNCTIONS_I(TRY_TO_NAME, TO_NAME, TRY_FROM_NAME, FROM_NAME, TO_ARG, FROM_ARG)             \
    _SN_DEFINE_VALIDATION_FUNCTIONS_II(SN_PP_CAT(validate_, TRY_TO_NAME),   SN_PP_CAT(has_, TRY_TO_NAME),   bool, TRY_TO_NAME,   (const T &, TO_ARG)) \
    _SN_DEFINE_VALIDATION_FUNCTIONS_II(SN_PP_CAT(validate_, TO_NAME),       SN_PP_CAT(has_, TO_NAME),       void, TO_NAME,       (const T &, TO_ARG)) \
    _SN_DEFINE_VALIDATION_FUNCTIONS_II(SN_PP_CAT(validate_, TRY_FROM_NAME), SN_PP_CAT(has_, TRY_FROM_NAME), bool, TRY_FROM_NAME, (FROM_ARG, T *)) \
    _SN_DEFINE_VALIDATION_FUNCTIONS_II(SN_PP_CAT(validate_, FROM_NAME),     SN_PP_CAT(has_, FROM_NAME),     void, FROM_NAME,     (FROM_ARG, T *))

// TODO(elric): #cpp26 use sn::type_name and formatting to get better error messages here.
#define _SN_DEFINE_VALIDATION_FUNCTIONS_II(VALIDATOR_NAME, CONCEPT_NAME, FUNCTION_RETURN, FUNCTION_NAME, FUNCTION_ARGS)  \
    template<class T, class... Tags>                                                                                    \
    consteval void VALIDATOR_NAME() {                                                                                   \
        if constexpr (sizeof...(Tags) == 0) {                                                                           \
            static_assert(sn::concepts::CONCEPT_NAME<T>,                                                                \
                          "Type T is not supported, did you forget to declare `"                                        \
                          SN_PP_STRINGIZE(FUNCTION_RETURN) " " SN_PP_STRINGIZE(FUNCTION_NAME) SN_PP_STRINGIZE(FUNCTION_ARGS) \
                          "` in T's namespace?");                                                                       \
        } else {                                                                                                        \
            static_assert(sn::concepts::CONCEPT_NAME<T, Tags...>,                                                       \
                          "Type T with provided Tags is not supported, did you forget to declare `"                     \
                          SN_PP_STRINGIZE(FUNCTION_RETURN) " " SN_PP_STRINGIZE(FUNCTION_NAME)                           \
                          "(" SN_PP_STRINGIZE(SN_PP_TUPLE_ELEM(0, FUNCTION_ARGS)) ", "                                  \
                          SN_PP_STRINGIZE(SN_PP_TUPLE_ELEM(1, FUNCTION_ARGS)) ", Tags...)"                              \
                          "` in T's namespace?");                                                                       \
        }                                                                                                               \
    }
