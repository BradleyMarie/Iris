/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    iriscamera_raytracer.h

Abstract:

    This file contains the definitions for the CAMERA_RAYTRACER type.

--*/

#ifndef _CAMERA_RAYTRACER_IRIS_CAMERA_
#define _CAMERA_RAYTRACER_IRIS_CAMERA_

#include <iriscamera.h>

//
// Types
//

typedef
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
(*PCAMERA_RAYTRACER_TRACE_RAY_ROUTINE)(
    _In_ PCVOID Context,
    _In_ RAY WorldRay,
    _In_ PRANDOM_REFERENCE Rng,
    _Out_ PCOLOR3 Color
    );

typedef struct _CAMER_RAYTRACER_VTABLE {
    PCAMERA_RAYTRACER_TRACE_RAY_ROUTINE TraceRayRoutine;
    PFREE_ROUTINE FreeRoutine;
} CAMERA_RAYTRACER_VTABLE, *PCAMERA_RAYTRACER_VTABLE;

typedef CONST CAMERA_RAYTRACER_VTABLE *PCCAMERA_RAYTRACER_VTABLE;

typedef struct _CAMERA_RAYTRACER CAMERA_RAYTRACER, *PCAMERA_RAYTRACER;
typedef CONST CAMERA_RAYTRACER *PCCAMERA_RAYTRACER;

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISCAMERAAPI
ISTATUS
CameraRayTracerAllocate(
    _In_ PCCAMERA_RAYTRACER_VTABLE CameraRayTracerVTable,
    _When_(DataSizeInBytes != 0, _In_reads_bytes_opt_(DataSizeInBytes)) PCVOID Data,
    _In_ SIZE_T DataSizeInBytes,
    _When_(DataSizeInBytes != 0, _Pre_satisfies_(_Curr_ != 0 && (_Curr_ & (_Curr_ - 1)) == 0 && DataSizeInBytes % _Curr_ == 0)) SIZE_T DataAlignment,
    _Out_ PCAMERA_RAYTRACER *CameraRayTracer
    );

_Success_(return == ISTATUS_SUCCESS)
ISTATUS
CameraRayTracerTraceRay(
    _In_ PCCAMERA_RAYTRACER CameraRayTracer,
    _In_ RAY WorldRay,
    _In_ PRANDOM_REFERENCE Rng,
    _Out_ PCOLOR3 Color
    );

IRISCAMERAAPI
VOID
CameraRayTracerFree(
    _In_opt_ _Post_invalid_ PCAMERA_RAYTRACER CameraRayTracer
    );

#endif // _PIXEL_SAMPLER_IRIS_CAMERA_