#pragma once

#if defined(_MSC_VER) && (!defined(_MSVC_TRADITIONAL) || _MSVC_TRADITIONAL)
#   error "sn doesn't support traditional preprocessor, build with /Zc:preprocessor"
#endif
