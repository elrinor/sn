#pragma once

#include "sn/core/preprocessor.h"

// This is where the common preprocessing routines go that we don't expose as part of SN interface.

#define _SN_PP_DEFAULT_CONSTRUCTORS(I, X) SN_PP_COMMA_IF(I) X()
