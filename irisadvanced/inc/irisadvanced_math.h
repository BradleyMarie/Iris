/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    irisadvanced_math.h

Abstract:

    This module includes the Iris Advanced math routines

--*/

#ifndef _MATH_IRIS_ADVANCED_HEADER_
#define _MATH_IRIS_ADVANCED_HEADER_

#include <irisadvanced.h>

//
// Functions
//

IRISADVANCEDAPI
VECTOR3
IrisAdvancedCreateOrthogonalVector(
    _In_ VECTOR3 NormalizedVector
    );

IRISADVANCEDAPI
VECTOR3
IrisAdvancedTransformVector(
    _In_ VECTOR3 NormalizedNormal,
    _In_ VECTOR3 VectorToTransform
    );

_Success_(return == ISTATUS_SUCCESS)
IRISADVANCEDAPI
ISTATUS
IrisAdvancedCosineSampleHemisphere(
    _In_ VECTOR3 NormalizedNormal,
    _Inout_ PRANDOM_REFERENCE Rng,
    _Out_ PVECTOR3 RandomVector
    );

_Success_(return == ISTATUS_SUCCESS)
IRISADVANCEDAPI
ISTATUS
IrisAdvancedUniformSampleHemisphere(
    _In_ VECTOR3 NormalizedNormal,
    _Inout_ PRANDOM_REFERENCE Rng,
    _Out_ PVECTOR3 RandomVector
    );

#endif // _MATH_IRIS_ADVANCED_HEADER_