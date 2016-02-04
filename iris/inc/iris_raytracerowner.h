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
RayTracerOwnerTraceScene(
    _In_ PRAYTRACER_OWNER RayTracerOwner,
    _In_ PCSCENE Scene,
    _In_ RAY Ray
    );

IRISAPI
ISTATUS
RayTracerOwnerSort(
    _Inout_ PRAYTRACER_OWNER RayTracerOwner
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISAPI
ISTATUS
RayTracerOwnerGetNextShapeHit(
    _Inout_ PRAYTRACER_OWNER RayTracerOwner,
    _Out_ PCSHAPE_HIT *ShapeHit
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISAPI
ISTATUS
RayTracerOwnerGetNextHit(
    _Inout_ PRAYTRACER_OWNER RayTracerOwner,
    _Out_ PCSHAPE_HIT *ShapeHit,
    _Out_opt_ PVECTOR3 ModelViewer,
    _Out_opt_ PPOINT3 ModelHitPoint,
    _Out_opt_ PPOINT3 WorldHitPoint,
    _Out_opt_ PCMATRIX *ModelToWorld
    );

IRISAPI
VOID
RayTracerOwnerFree(
    _In_opt_ _Post_invalid_ PRAYTRACER_OWNER RayTracerOwner
    );

#endif // _RAYTRACER_OWNER_IRIS_