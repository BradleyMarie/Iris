/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    iris_raytracer.h

Abstract:

    This file contains the definitions for the SCENE_OBJECT type.

--*/

#ifndef _RAYTRACER_IRIS_
#define _RAYTRACER_IRIS_

#include <iris.h>

//
// Types
//

typedef struct _RAYTRACER RAYTRACER, *PRAYTRACER;
typedef CONST RAYTRACER *PCRAYTRACER;

//
// Functions
//

_Check_return_
_Ret_opt_
IRISAPI
PRAYTRACER
RayTracerAllocate(
    _In_ RAY Ray
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISAPI
ISTATUS
RayTracerSetRay(
    _Inout_ PRAYTRACER RayTracer,
    _In_ RAY Ray
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISAPI
ISTATUS
RayTracerTraceGeometry(
    _Inout_ PRAYTRACER RayTracer,
    _In_ PCGEOMETRY Geometry
    );

IRISAPI
ISTATUS
RayTracerSort(
    _Inout_ PRAYTRACER RayTracer
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISAPI
ISTATUS
RayTracerGetNextGeometryHit(
    _Inout_ PRAYTRACER RayTracer,
    _Out_ PGEOMETRY_HIT GeometryHit
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISAPI
ISTATUS
RayTracerGetNextShapeHit(
    _Inout_ PRAYTRACER RayTracer,
    _Out_ PCSHAPE_HIT *ShapeHit
    );

IRISAPI
VOID
RayTracerFree(
    _In_opt_ _Post_invalid_ PRAYTRACER RayTracer
    );

#endif // _RAYTRACER_IRIS_