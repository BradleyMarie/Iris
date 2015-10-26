/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    raytracer.c

Abstract:

    This file contains the definitions for the SPECTRUM_RAYTRACER type.

--*/

#include <irisphysxp.h>

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISPHYSXAPI
ISTATUS
SpectrumRayTracerSetRay(
    _Inout_ PSPECTRUM_RAYTRACER RayTracer,
    _In_ RAY Ray,
    _In_ BOOL NormalizeRay
    )
{
    ISTATUS Status;

    if (RayTracer == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    Status = RayTracerSetRay(RayTracer->RayTracer,
                             Ray,
                             NormalizeRay);

    return Status;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISPHYSXAPI
ISTATUS
SpectrumRayTracerGetRay(
    _In_ PSPECTRUM_RAYTRACER RayTracer,
    _Out_ PRAY Ray
    )
{
    ISTATUS Status;

    if (RayTracer == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    Status = RayTracerGetRay(RayTracer->RayTracer, Ray);

    return Status;
}

IRISPHYSXAPI
ISTATUS
SpectrumRayTracerSort(
    _Inout_ PSPECTRUM_RAYTRACER RayTracer
    )
{
    ISTATUS Status;

    if (RayTracer == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    Status = RayTracerSort(RayTracer->RayTracer);

    return Status;
}

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
    )
{
    PCSHAPE_HIT *IrisShapeHit;
    ISTATUS Status;

    if (RayTracer == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    IrisShapeHit = (PCSHAPE_HIT *) ShapeHit;

    Status = RayTracerGetNextHit(RayTracer->RayTracer, 
                                 IrisShapeHit,
                                 ModelViewer,
                                 ModelHitPoint,
                                 WorldHitPoint,
                                 ModelToWorld);

    return Status;
}