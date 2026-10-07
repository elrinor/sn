#pragma once

#include "sn/core/error_fwd.h"
#include "sn/core/preprocessor.h"

#include "tuple_types.h"

#define _SN_DEFINE_FORWARDING_FUNCTIONS(NAME, TO_SRC_TYPE, TO_DST_TYPE, FROM_SRC_TYPE, FROM_DST_TYPE, TO_NS, FROM_NS, TAGS_TUPLE) \
    _SN_DEFINE_FORWARDING_FUNCTIONS_I(SN_PP_CAT(to_, NAME), SN_PP_CAT(from_, NAME),                                     \
                                      TO_SRC_TYPE, TO_DST_TYPE, FROM_SRC_TYPE, FROM_DST_TYPE,                           \
                                      TO_NS, FROM_NS,                                                                   \
                                      _SN_PP_TUPLE_TYPES_TO_DECL_PARAMS(tag, TAGS_TUPLE), _SN_PP_TUPLE_TYPES_TO_CALL_PARAMS(tag, TAGS_TUPLE))

#define _SN_DEFINE_FORWARDING_FUNCTIONS_I(TO_NAME, FROM_NAME, TO_SRC_TYPE, TO_DST_TYPE, FROM_SRC_TYPE, FROM_DST_TYPE, TO_NS, FROM_NS, DECL_TAGS, CALL_TAGS) \
    bool TO_NAME(TO_SRC_TYPE src, TO_DST_TYPE dst, sn::error *err SN_PP_TUPLE_ENUM_TRAILING(DECL_TAGS)) {               \
        return TO_NS::TO_NAME(src, dst, err SN_PP_TUPLE_ENUM_TRAILING(CALL_TAGS));                                      \
    }                                                                                                                   \
    bool FROM_NAME(FROM_SRC_TYPE src, FROM_DST_TYPE dst, sn::error *err SN_PP_TUPLE_ENUM_TRAILING(DECL_TAGS)) {         \
        return FROM_NS::FROM_NAME(src, dst, err SN_PP_TUPLE_ENUM_TRAILING(CALL_TAGS));                                  \
    }
