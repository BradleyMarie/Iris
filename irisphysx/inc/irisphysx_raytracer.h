/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    irisphysx_raytracer.h

Abstract:

    This file contains the definitions for the SPECTRUM_RAYTRACER type.

--*/

#ifndef _SPECTRUM_RAYTRACER_IRIS_PHYSX_
#define _SPECTRUM_RAYTRACER_IRIS_PHYSX_

#include <irisphysx.h>

//
// Types
//

typedef struct _SPECTRUM_SHAPE_HIT {
    PCSPECTRUM_SHAPE Shape;
    FLOAT Distance;
    INT32 FaceHit;
    _Field_size_bytes_opt_(AdditionalDataSizeInBytes) PCVOID AdditionalData;
    SIZE_T AdditionalDataSizeInBytes;
} SPECTRUM_SHAPE_HIT, *PSPECTRUM_SHAPE_HIT;

typedef CONST SPECTRUM_SHAPE_HIT *PCSPECTRUM_SHAPE_HIT;

typedef
_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS 
(*PSPECTRUM_RAYTRACER_PROCESS_HIT_ROUTINE)(
    _Inout_opt_ PVOID Context, 
    _In_ PCSPECTRUM_SHAPE_HIT ShapeHit,
    _In_ PCMATRIX ModelToWorld,
    _In_ VECTOR3 ModelViewer,
    _In_ POINT3 ModelHitPoint,
    _In_ POINT3 WorldHitPoint
    );

typedef struct _SPECTRUM_RAYTRACER SPECTRUM_RAYTRACER, *PSPECTRUM_RAYTRACER;
typedef CONST SPECTRUM_RAYTRACER *PCSPECTRUM_RAYTRACER;

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISPHYSXAPI
ISTATUS
SpectrumRayTracerTraceSceneProcessClosestHit(
    _Inout_ PSPECTRUM_RAYTRACER SpectrumRayTracer,
    _In_ PCSPECTRUM_SCENE Scene,
    _In_ RAY Ray,
    _In_ FLOAT MinimumDistance,
    _In_ PSPECTRUM_RAYTRACER_PROCESS_HIT_ROUTINE ProcessHitRoutine,
    _Inout_opt_ PVOID ProcessHitContext
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISPHYSXAPI
ISTATUS
SpectrumRayTracerTraceSceneProcessAllHitsInOrder(
    _Inout_ PSPECTRUM_RAYTRACER SpectrumRayTracer,
    _In_ PCSPECTRUM_SCENE Scene,
    _In_ RAY Ray,
    _In_ PSPECTRUM_RAYTRACER_PROCESS_HIT_ROUTINE ProcessHitRoutine,
    _Inout_opt_ PVOID ProcessHitContext
    );

#endif // _SPECTRUM_RAYTRACER_IRIS_PHYSX_