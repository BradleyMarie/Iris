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

typedef struct _SPECTRUM_RAYTRACER SPECTRUM_RAYTRACER, *PSPECTRUM_RAYTRACER;
typedef CONST SPECTRUM_RAYTRACER *PCSPECTRUM_RAYTRACER;

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISPHYSXAPI
ISTATUS
SpectrumRayTracerTraceRay(
    _Inout_ PSPECTRUM_RAYTRACER RayTracer,
    _In_ RAY Ray,
    _In_ BOOL NormalizeRay
    );

IRISPHYSXAPI
ISTATUS
SpectrumRayTracerSort(
    _Inout_ PSPECTRUM_RAYTRACER RayTracer
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISPHYSXAPI
ISTATUS
SpectrumRayTracerGetNextHit(
    _Inout_ PSPECTRUM_RAYTRACER RayTracer,
    _Out_ PCSPECTRUM_SHAPE_HIT *ShapeHit,
    _Out_opt_ PVECTOR3 ModelViewer,
    _Out_opt_ PPOINT3 ModelHitPoint,
    _Out_opt_ PPOINT3 WorldHitPoint,
    _Out_opt_ PCMATRIX *ModelToWorld
    );

#endif // _SPECTRUM_RAYTRACER_IRIS_PHYSX_