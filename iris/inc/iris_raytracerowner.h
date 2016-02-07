/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    iris_raytracer.h

Abstract:

    This file contains the definitions for the RAYTRACER_OWNER type.

--*/

#ifndef _RAYTRACER_OWNER_IRIS_
#define _RAYTRACER_OWNER_IRIS_

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
    _In_ PCSHAPE_HIT ShapeHit
    );
    
typedef
_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS 
(*PRAYTRACER_PROCESS_HIT_WITH_COORDINATES_ROUTINE)(
    _Inout_opt_ PVOID Context, 
    _In_ PCSHAPE_HIT ShapeHit,
    _In_ PCMATRIX ModelToWorld,
    _In_ VECTOR3 ModelViewer,
    _In_ POINT3 ModelHitPoint,
    _In_ POINT3 WorldHitPoint
    );

typedef struct _RAYTRACER_OWNER RAYTRACER_OWNER, *PRAYTRACER_OWNER;
typedef CONST RAYTRACER_OWNER *PCRAYTRACER_OWNER;

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISAPI
ISTATUS
RayTracerOwnerAllocate(
    _Out_ PRAYTRACER_OWNER *RayTracerOwner
    );
    
_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISAPI
ISTATUS
RayTracerOwnerTraceSceneProcessClosestHit(
    _Inout_ PRAYTRACER_OWNER RayTracerOwner,
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
RayTracerOwnerTraceSceneProcessClosestHitWithCoordinates(
    _Inout_ PRAYTRACER_OWNER RayTracerOwner,
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
RayTracerOwnerTraceSceneProcessAllHitsOutOfOrder(
    _Inout_ PRAYTRACER_OWNER RayTracerOwner,
    _In_ PCSCENE Scene,
    _In_ RAY Ray,
    _In_ PRAYTRACER_PROCESS_HIT_ROUTINE ProcessHitRoutine,
    _Inout_opt_ PVOID ProcessHitContext
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISAPI
ISTATUS
RayTracerOwnerTraceSceneProcessAllHitsInOrderWithCoordinates(
    _Inout_ PRAYTRACER_OWNER RayTracerOwner,
    _In_ PCSCENE Scene,
    _In_ RAY Ray,
    _In_ PRAYTRACER_PROCESS_HIT_WITH_COORDINATES_ROUTINE ProcessHitRoutine,
    _Inout_opt_ PVOID ProcessHitContext
    );

IRISAPI
VOID
RayTracerOwnerFree(
    _In_opt_ _Post_invalid_ PRAYTRACER_OWNER RayTracerOwner
    );

#endif // _RAYTRACER_OWNER_IRIS_