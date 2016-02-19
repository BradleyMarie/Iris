/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    iris_raytracerreference.h

Abstract:

    This file contains the declarations for the 
    RAYTRACER_REFERENCE type.

--*/

#ifndef _RAYTRACER_REFERENCE_IRIS_
#define _RAYTRACER_REFERENCE_IRIS_

#include <iris.h>

//
// Types
//

typedef struct _RAYTRACER_REFERENCE RAYTRACER_REFERENCE, *PRAYTRACER_REFERENCE;
typedef CONST RAYTRACER_REFERENCE *PCRAYTRACER_REFERENCE;

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISAPI
ISTATUS
RayTracerReferenceTraceShape(
    _Inout_ PRAYTRACER_REFERENCE RayTracerReference,
    _In_ PCSHAPE Shape
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISAPI
ISTATUS
RayTracerReferenceTraceShapeWithTransform(
    _Inout_ PRAYTRACER_REFERENCE RayTracerReference,
    _In_ PCSHAPE Shape,
    _In_opt_ PCMATRIX ModelToWorld,
    _In_ BOOL Premultiplied
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISAPI
ISTATUS
RayTracerReferenceTracePremultipliedShapeWithTransform(
    _Inout_ PRAYTRACER_REFERENCE RayTracerReference,
    _In_ PCSHAPE Shape,
    _In_opt_ PCMATRIX ModelToWorld
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISAPI
ISTATUS
RayTracerReferenceGetRay(
    _In_ PRAYTRACER_REFERENCE RayTracerReference,
    _Out_ PRAY Ray
    );

#endif // _RAYTRACER_REFERENCE_IRIS_