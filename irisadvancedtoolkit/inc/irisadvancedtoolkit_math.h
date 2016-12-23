/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    irisadvancedtoolkit_math.h

Abstract:

    This module includes the IrisRandomToolkit math routines

--*/

#ifndef _MATH_IRIS_ADVANCED_TOOLKIT_HEADER_
#define _MATH_IRIS_ADVANCED_TOOLKIT_HEADER_

#include <irisadvancedtoolkit.h>

//
// Functions
//

_Success_(return == ISTATUS_SUCCESS)
IRISADVANCEDTOOLKITAPI
ISTATUS
CosineSampleHemisphere(
    _In_ VECTOR3 NormalizedNormal,
    _Inout_ PRANDOM Rng,
    _Out_ PVECTOR3 RandomVector
    );

_Success_(return == ISTATUS_SUCCESS)
IRISADVANCEDTOOLKITAPI
ISTATUS
UniformSampleHemisphere(
    _In_ VECTOR3 NormalizedNormal,
    _Inout_ PRANDOM Rng,
    _Out_ PVECTOR3 RandomVector
    );

#endif // _MATH_IRIS_ADVANCED_TOOLKIT_HEADER_