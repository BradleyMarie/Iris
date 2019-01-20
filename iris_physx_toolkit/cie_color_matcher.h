/*++

Copyright (c) 2019 Brad Weinberger

Module Name:

    cie_color_matcher.h

Abstract:

    Creates a color matcher which generates a color using the CIE 1931 2 degree
    standard observer sampled once per nanometer.

--*/

#ifndef _IRIS_PHYSX_TOOLKIT_CIE_COLOR_MATCHER_
#define _IRIS_PHYSX_TOOLKIT_CIE_COLOR_MATCHER_

#include "iris_physx/iris_physx.h"

#if __cplusplus 
extern "C" {
#endif // __cplusplus

//
// Functions
//

ISTATUS
CieColorMatcherAllocate(
    _Out_ PCOLOR_MATCHER *color_matcher
    );

#if __cplusplus 
}
#endif // __cplusplus

#endif // _IRIS_PHYSX_TOOLKIT_CIE_COLOR_MATCHER_