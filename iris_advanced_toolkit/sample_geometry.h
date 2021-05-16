/*++

Copyright (c) 2020 Brad Weinberger

Module Name:

    sample_geometry.h

Abstract:

    Routines for sampling geometry randomly.

--*/

#ifndef _IRIS_ADVANCED_TOOLKIT_SAMPLE_GEOMETRY_
#define _IRIS_ADVANCED_TOOLKIT_SAMPLE_GEOMETRY_

#include "iris_advanced/iris_advanced.h"

//
// Functions
//

ISTATUS
SampleHemisphereWithCosineWeighting(
    _In_ VECTOR3 surface_normal,
    _Inout_ PRANDOM rng,
    _Out_ PVECTOR3 result
    );

ISTATUS
SampleHemisphereUniformly(
    _In_ VECTOR3 surface_normal,
    _Inout_ PRANDOM rng,
    _Out_ PVECTOR3 result
    );

ISTATUS
SampleSphereUniformly(
    _In_ float_t radius,
    _Inout_ PRANDOM rng,
    _Out_ PVECTOR3 result
    );

#endif // _IRIS_ADVANCED_TOOLKIT_SAMPLE_GEOMETRY_