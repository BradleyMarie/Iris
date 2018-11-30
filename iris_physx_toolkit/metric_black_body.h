/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    metric_black_body.h

Abstract:

    Creates a black body spectrum whose wavelengths are specifed in nanometers
    and whose spectral radience are returned in watts per meter squared per
    steradian per meter.

--*/

#ifndef _IRIS_PHYSX_TOOLKIT_METRIC_BLACK_BODY_
#define _IRIS_PHYSX_TOOLKIT_METRIC_BLACK_BODY_

#include "iris_physx/iris_physx.h"

#if __cplusplus 
extern "C" {
#endif // __cplusplus

//
// Functions
//

ISTATUS
MetricBlackBodyAllocate(
    _In_ float_t temperature,
    _Out_ PSPECTRUM *spectrum
    );

#if __cplusplus 
}
#endif // __cplusplus

#endif // _IRIS_PHYSX_TOOLKIT_METRIC_BLACK_BODY_