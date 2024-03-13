#pragma once

#include "sn/string/enum_string.h"

enum class MultiTsEnum {
    MULTI_1,
    MULTI_2
};
using enum MultiTsEnum;

SN_DEFINE_ENUM_REFLECTION(MultiTsEnum, ({
    {MULTI_1, "OH"},
    {MULTI_2, "UGH"}
}))
SN_DEFINE_INLINE_ENUM_STRING_FUNCTIONS(MultiTsEnum, sn::case_sensitive)
