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

typedef struct _SPECTRUM_VISIBILITY_TESTER_TEST_LIGHT_VISIBILITY_PROCESS_HIT_CONTEXT {
    PCLIGHT TargetLight;
    BOOL Visible;
} SPECTRUM_VISIBILITY_TESTER_TEST_LIGHT_VISIBILITY_PROCESS_HIT_CONTEXT, *PSPECTRUM_VISIBILITY_TESTER_TEST_LIGHT_VISIBILITY_PROCESS_HIT_CONTEXT;

//
// Static Functions
//

SFORCEINLINE
SPECTRUM_VISIBILITY_TESTER_TEST_LIGHT_VISIBILITY_PROCESS_HIT_CONTEXT
SpectrumVisibilityTesterTestLightVisibilityProcessHitCreateContext(
    _In_ PCLIGHT TargetLight,
    _In_ BOOL Visible
    )
{
    SPECTRUM_VISIBILITY_TESTER_TEST_LIGHT_VISIBILITY_PROCESS_HIT_CONTEXT Context;

    ASSERT(TargetLight != NULL);

    Context.TargetLight = TargetLight;
    Context.Visible = Visible;

    return Context;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
STATIC
ISTATUS
SpectrumVisibilityTesterTestLightVisibilityProcessHit(
    _Inout_ PVOID Context,
    _In_ PCHIT Hit
    )
{
    PSPECTRUM_VISIBILITY_TESTER_TEST_LIGHT_VISIBILITY_PROCESS_HIT_CONTEXT TestContext;
    PCSPECTRUM_SHAPE SpectrumShape;
    PCLIGHT ClosestLight;
    
    TestContext = (PSPECTRUM_VISIBILITY_TESTER_TEST_LIGHT_VISIBILITY_PROCESS_HIT_CONTEXT) Context;
    SpectrumShape = (PCSPECTRUM_SHAPE) Hit->ShapeReference;

    SpectrumShapeGetLight(SpectrumShape, Hit->FaceHit, &ClosestLight);
    
    if (ClosestLight == TestContext->TargetLight)
    {
        TestContext->Visible = TRUE;
    }
    
    return ISTATUS_SUCCESS;
}

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

    if (Tester == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (Visible == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    Status = RayTracerOwnerTestVisibility(Tester->RayTracerOwner,
                                          Tester->Scene,
                                          WorldRay,
                                          DistanceToObject,
                                          Tester->Epsilon,
                                          Visible);

    if (Status != ISTATUS_SUCCESS)
    {
        if (Status == ISTATUS_INVALID_ARGUMENT_02)
        {
            return ISTATUS_INVALID_ARGUMENT_01;
        }

        if (Status == ISTATUS_INVALID_ARGUMENT_03)
        {
            return ISTATUS_INVALID_ARGUMENT_02;
        }

        return Status;
    }

    return Status;
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

    if (Tester == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (Visible == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    Status = RayTracerOwnerTestVisibilityAnyDistance(Tester->RayTracerOwner,
                                                     Tester->Scene,
                                                     WorldRay,
                                                     Tester->Epsilon,
                                                     Visible);

    if (Status == ISTATUS_INVALID_ARGUMENT_02)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }
    
    return Status;
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
    SPECTRUM_VISIBILITY_TESTER_TEST_LIGHT_VISIBILITY_PROCESS_HIT_CONTEXT ProcessHitContext;
    PRAYTRACER_OWNER RayTracerOwner;
    ISTATUS Status;

    ASSERT(Tester->Scene != NULL);

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

    ProcessHitContext = SpectrumVisibilityTesterTestLightVisibilityProcessHitCreateContext(Light, FALSE);

    Status = RayTracerOwnerTraceSceneProcessClosestHit(RayTracerOwner,
                                                       Tester->Scene,
                                                       WorldRay,
                                                       Tester->Epsilon,
                                                       SpectrumVisibilityTesterTestLightVisibilityProcessHit,
                                                       &ProcessHitContext);
    
    if (Status != ISTATUS_SUCCESS)
    {
        if (Status == ISTATUS_INVALID_ARGUMENT_02)
        {
            return ISTATUS_INVALID_ARGUMENT_01;
        }

        return Status;
    }
    
    *Visible = ProcessHitContext.Visible;
    
    return ISTATUS_SUCCESS;
}