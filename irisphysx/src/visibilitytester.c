/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    visibilitytester.c

Abstract:

    This file contains the function definitions for the 
    SPECTRUM_VISIBILITY_TESTER type.

--*/

#include <irisphysxp.h>

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
SpectrumVisibilityTesterTestVisibility(
    _In_ PSPECTRUM_VISIBILITY_TESTER Tester,
    _In_ RAY WorldRay,
    _In_ FLOAT DistanceToObject,
    _Out_ PBOOL Visible
    )
{
    ISTATUS Status;

    ASSERT(Tester->Scene != NULL);

    //
    // Ray is validated by RayTracerTestOwnerVisibility
    //

    if (Tester == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (IsFiniteFloat(DistanceToObject) == FALSE ||
        IsGreaterThanOrEqualToZeroFloat(DistanceToObject) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (Visible == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    Status = RayTracerOwnerTestVisibility(Tester->RayTracerOwner,
                                          WorldRay,
                                          Tester->Epsilon,
                                          Tester->Scene,
                                          DistanceToObject,
                                          Visible);

    return ISTATUS_SUCCESS;
}

//
// This is needed since not all C environments support the INFINITY macro
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
SpectrumVisibilityTesterTestVisibilityAnyDistance(
    _In_ PSPECTRUM_VISIBILITY_TESTER Tester,
    _In_ RAY WorldRay,
    _Out_ PBOOL Visible
    )
{
    ISTATUS Status;

    ASSERT(Tester->Scene != NULL);

    //
    // Ray is validated by RayTracerTestOwnerVisibilityAnyDistance
    //

    if (Tester == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (Visible == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    Status = RayTracerOwnerTestVisibilityAnyDistance(Tester->RayTracerOwner,
                                                     WorldRay,
                                                     Tester->Epsilon,
                                                     Tester->Scene,
                                                     Visible);

    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISPHYSXAPI
ISTATUS
SpectrumVisibilityTesterTestLightVisibility(
    _In_ PSPECTRUM_VISIBILITY_TESTER Tester,
    _In_ RAY WorldRay,
    _In_ PCLIGHT Light,
    _Out_ PBOOL Visible
    )
{
    PRAYTRACER_OWNER RayTracerOwner;
    PCSPECTRUM_SHAPE ClosestShape;
    PCSHAPE_HIT ClosestShapeHit;
    PRAYTRACER RayTracer;
    PCSHAPE_HIT ShapeHit;
    PCLIGHT ClosestLight;
    ISTATUS Status;

    ASSERT(Tester->Scene != NULL);

    //
    // Ray is validated by RayTracerOwnerGetRayTracer
    //

    if (Tester == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (Light == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (Visible == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    RayTracerOwner = Tester->RayTracerOwner;

    Status = RayTracerOwnerGetRayTracer(RayTracerOwner,
                                        WorldRay,
                                        &RayTracer);

    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }

    Status = SceneTrace(Tester->Scene, RayTracer);

    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }

    Status = RayTracerOwnerGetNextShapeHit(RayTracerOwner, 
                                           &ShapeHit);

    if (Status == ISTATUS_NO_MORE_DATA)
    {
        *Visible = FALSE;
        return ISTATUS_SUCCESS;
    }

    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }

    ClosestShapeHit = ShapeHit;

    do
    {
        if (ShapeHit->Distance < ClosestShapeHit->Distance)
        {
            ClosestShapeHit = ShapeHit;
        }

        Status = RayTracerOwnerGetNextShapeHit(RayTracerOwner, &ShapeHit);
    } while (Status == ISTATUS_SUCCESS);

    if (Status != ISTATUS_NO_MORE_DATA)
    {
        return Status;
    }

    ClosestShape = (PCSPECTRUM_SHAPE) ShapeHit->Shape;

    SpectrumShapeGetLight(ClosestShape, ShapeHit->FaceHit, &ClosestLight);
    *Visible = (ClosestLight == Light) ? TRUE : FALSE;
    
    return ISTATUS_SUCCESS;
}