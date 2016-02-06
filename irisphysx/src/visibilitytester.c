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
// Types
//

typedef struct _TEST_LIGHT_VISIBILITY {
    PCLIGHT TargetLight;
    BOOL Visible;
} TEST_LIGHT_VISIBILITY, *PTEST_LIGHT_VISIBILITY;

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
STATIC
ISTATUS
SpectrumVisibilityTesterCheckLight(
    _Inout_ PVOID Context,
    _In_ PCSHAPE_HIT ShapeHit
    )
{
    PTEST_LIGHT_VISIBILITY TestContext;
    PCSPECTRUM_SHAPE SpectrumShape;
    PCLIGHT ClosestLight;
    
    TestContext = (PTEST_LIGHT_VISIBILITY) Context;
    SpectrumShape = (PCSPECTRUM_SHAPE) ShapeHit->Shape;

    SpectrumShapeGetLight(SpectrumShape, ShapeHit->FaceHit, &ClosestLight);
    
    if (ClosestLight == TestContext->TargetLight)
    {
        TestContext->Visible = TRUE;
    }
    
    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISPHYSXAPI
ISTATUS
SpectrumVisibilityTesterTestLightVisibility(
    _In_ PSPECTRUM_VISIBILITY_TESTER Tester,
    _In_ RAY WorldRay,
    _In_ FLOAT MinimumDistance,
    _In_ PCLIGHT Light,
    _Out_ PBOOL Visible
    )
{
    TEST_LIGHT_VISIBILITY TestLightContext;
    PRAYTRACER_OWNER RayTracerOwner;
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

    TestLightContext.TargetLight = Light;
    TestLightContext.Visible = FALSE;

    Status = RayTracerOwnerTraceSceneFindClosestHit(RayTracerOwner,
                                                    Tester->Scene,
                                                    WorldRay,
                                                    MinimumDistance,
                                                    SpectrumVisibilityTesterCheckLight,
                                                    &TestLightContext);
    
    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }
    
    *Visible = TestLightContext.Visible;
    
    return ISTATUS_SUCCESS;
}