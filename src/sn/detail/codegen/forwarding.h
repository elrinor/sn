#pragma once

#include "sn/core/preprocessor.h"

#include "tuple_types.h"

#define _SN_DEFINE_FORWARDING_FUNCTIONS(NAME, TO_SRC_TYPE, TO_DST_TYPE, FROM_SRC_TYPE, FROM_DST_TYPE, TO_NS, FROM_NS, TAGS_TUPLE) \
    _SN_DEFINE_FORWARDING_FUNCTIONS_I(SN_PP_CAT(try_to_, NAME), SN_PP_CAT(to_, NAME), SN_PP_CAT(try_from_, NAME), SN_PP_CAT(from_, NAME), \
                                      TO_SRC_TYPE, TO_DST_TYPE, FROM_SRC_TYPE, FROM_DST_TYPE,                           \
                                      TO_NS, FROM_NS,                                                                   \
                                      _SN_PP_TUPLE_TYPES_TO_DECL_PARAMS(tag, TAGS_TUPLE), _SN_PP_TUPLE_TYPES_TO_CALL_PARAMS(tag, TAGS_TUPLE))

#define _SN_DEFINE_FORWARDING_FUNCTIONS_I(TRY_TO_NAME, TO_NAME, TRY_FROM_NAME, FROM_NAME, TO_SRC_TYPE, TO_DST_TYPE, FROM_SRC_TYPE, FROM_DST_TYPE, TO_NS, FROM_NS, DECL_TAGS, CALL_TAGS) \
    bool TRY_TO_NAME(TO_SRC_TYPE src, TO_DST_TYPE dst SN_PP_TUPLE_ENUM_TRAILING(DECL_TAGS)) noexcept {                  \
        return TO_NS::TRY_TO_NAME(src, dst SN_PP_TUPLE_ENUM_TRAILING(CALL_TAGS));                                       \
    }                                                                                                                   \
    void TO_NAME(TO_SRC_TYPE src, TO_DST_TYPE dst SN_PP_TUPLE_ENUM_TRAILING(DECL_TAGS)) {                               \
        TO_NS::TO_NAME(src, dst SN_PP_TUPLE_ENUM_TRAILING(CALL_TAGS));                                                  \
    }                                                                                                                   \
    bool TRY_FROM_NAME(FROM_SRC_TYPE src, FROM_DST_TYPE dst SN_PP_TUPLE_ENUM_TRAILING(DECL_TAGS)) noexcept {            \
        return FROM_NS::TRY_FROM_NAME(src, dst SN_PP_TUPLE_ENUM_TRAILING(CALL_TAGS));                                   \
    }                                                                                                                   \
    void FROM_NAME(FROM_SRC_TYPE src, FROM_DST_TYPE dst SN_PP_TUPLE_ENUM_TRAILING(DECL_TAGS)) {                         \
        FROM_NS::FROM_NAME(src, dst SN_PP_TUPLE_ENUM_TRAILING(CALL_TAGS));                                              \
    }
