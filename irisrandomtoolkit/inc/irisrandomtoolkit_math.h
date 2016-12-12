/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    irisrandomtoolkit_math.h

Abstract:

    This module includes the IrisRandomToolkit math routines

--*/

#ifndef _MATH_IRIS_RANDOM_TOOLKIT_HEADER_
#define _MATH_IRIS_RANDOM_TOOLKIT_HEADER_

#include <irisrandomtoolkit.h>

//
// Functions
//

_Success_(return == ISTATUS_SUCCESS)
IRISRANDOMTOOLKITAPI
ISTATUS
CosineSampleHemisphere(
    _In_ VECTOR3 NormalizedNormal,
    _Inout_ PRANDOM_REFERENCE Rng,
    _Out_ PVECTOR3 RandomVector
    );

_Success_(return == ISTATUS_SUCCESS)
IRISRANDOMTOOLKITAPI
ISTATUS
UniformSampleHemisphere(
    _In_ VECTOR3 NormalizedNormal,
    _Inout_ PRANDOM_REFERENCE Rng,
    _Out_ PVECTOR3 RandomVector
    );

#endif // _MATH_IRIS_RANDOM_TOOLKIT_HEADER_