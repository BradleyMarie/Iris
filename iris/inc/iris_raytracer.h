/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    iris_raytracer.h

Abstract:

    This file contains the definitions for the RAYTRACER type.

--*/

#ifndef _RAYTRACER_IRIS_
#define _RAYTRACER_IRIS_

#include <iris.h>

//
// Types
//

typedef
_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS 
(*PRAYTRACER_PROCESS_HIT_ROUTINE)(
    _Inout_opt_ PVOID Context, 
    _In_ PCHIT Hit
    );
    
typedef
_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS 
(*PRAYTRACER_PROCESS_HIT_WITH_COORDINATES_ROUTINE)(
    _Inout_opt_ PVOID Context, 
    _In_ PCHIT Hit,
    _In_ PCMATRIX ModelToWorld,
    _In_ VECTOR3 ModelViewer,
    _In_ POINT3 ModelHitPoint,
    _In_ POINT3 WorldHitPoint
    );

typedef struct _RAYTRACER RAYTRACER, *PRAYTRACER;
typedef CONST RAYTRACER *PCRAYTRACER;

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISAPI
ISTATUS
RayTracerAllocate(
    _Out_ PRAYTRACER *RayTracer
    );
    
_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISAPI
ISTATUS
RayTracerTraceSceneProcessClosestHit(
    _Inout_ PRAYTRACER RayTracer,
    _In_ PCSCENE Scene,
    _In_ RAY Ray,
    _In_ FLOAT MinimumDistance,
    _In_ PRAYTRACER_PROCESS_HIT_ROUTINE ProcessHitRoutine,
    _Inout_opt_ PVOID ProcessHitContext
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISAPI
ISTATUS
RayTracerTraceSceneProcessClosestHitWithCoordinates(
    _Inout_ PRAYTRACER RayTracer,
    _In_ PCSCENE Scene,
    _In_ RAY Ray,
    _In_ FLOAT MinimumDistance,
    _In_ PRAYTRACER_PROCESS_HIT_WITH_COORDINATES_ROUTINE ProcessHitRoutine,
    _Inout_opt_ PVOID ProcessHitContext
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISAPI
ISTATUS
RayTracerTraceSceneProcessAllHitsOutOfOrder(
    _Inout_ PRAYTRACER RayTracer,
    _In_ PCSCENE Scene,
    _In_ RAY Ray,
    _In_ PRAYTRACER_PROCESS_HIT_ROUTINE ProcessHitRoutine,
    _Inout_opt_ PVOID ProcessHitContext
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISAPI
ISTATUS
RayTracerTraceSceneProcessAllHitsInOrderWithCoordinates(
    _Inout_ PRAYTRACER RayTracer,
    _In_ PCSCENE Scene,
    _In_ RAY Ray,
    _In_ PRAYTRACER_PROCESS_HIT_WITH_COORDINATES_ROUTINE ProcessHitRoutine,
    _Inout_opt_ PVOID ProcessHitContext
    );

IRISAPI
VOID
RayTracerFree(
    _In_opt_ _Post_invalid_ PRAYTRACER RayTracer
    );

#endif // _RAYTRACER_IRIS_