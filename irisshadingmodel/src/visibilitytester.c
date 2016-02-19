/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    visibilitytester.c

Abstract:

    This file contains the function definitions for the VISIBILITY_TESTER type.

--*/

#include <irisshadingmodelp.h>
#include <iriscommon_visibility.h>

struct _VISIBILITY_TESTER {
    PRAYTRACER RayTracer;
    FLOAT Epsilon;
    PSCENE Scene;
};

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
VisibilityTesterAllocate(
    _In_ PSCENE Scene,
    _In_ FLOAT Epsilon,
    _Out_ PVISIBILITY_TESTER *VisibilityTester
    )
{
    PRAYTRACER RayTracer;
    PVISIBILITY_TESTER Tester;
    ISTATUS Status;

    if (Scene == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (IsNormalFloat(Epsilon) == FALSE ||
        IsFiniteFloat(Epsilon) == FALSE ||
        Epsilon < (FLOAT) 0.0)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (VisibilityTester == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    Tester = (PVISIBILITY_TESTER) malloc(sizeof(VISIBILITY_TESTER));

    if (Tester == NULL)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    Status = RayTracerAllocate(&RayTracer);

    if (Status != ISTATUS_SUCCESS)
    {
        free(Tester);
        return ISTATUS_ALLOCATION_FAILED;
    }

    SceneRetain(Scene);

    Tester->Scene = Scene;
    Tester->RayTracer = RayTracer;
    Tester->Epsilon = MaxFloat(Epsilon, (FLOAT) 0.0);

    *VisibilityTester = Tester;

    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
VisibilityTesterTestVisibility(
    _In_ PVISIBILITY_TESTER Tester,
    _In_ RAY WorldRay,
    _In_ FLOAT DistanceToObject,
    _Out_ PBOOL Visible
    )
{
    ISTATUS Status;

    ASSERT(Tester->Scene != NULL);

    //
    // Ray is validated by RayTracerSetRay
    //

    if (Tester == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (IsNormalFloat(DistanceToObject) == FALSE ||
        IsFiniteFloat(DistanceToObject) == FALSE ||
        DistanceToObject < (FLOAT) 0.0)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (Visible == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    WorldRay = RayNormalize(WorldRay);

    Status = RayTracerTestVisibility(Tester->RayTracer,
                                     Tester->Scene,
                                     WorldRay,
                                     DistanceToObject,
                                     Tester->Epsilon,
                                     Visible);

    return Status;
}

//
// This is needed since not all C environments support the INFINITY macro
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISSHADINGMODELAPI
ISTATUS
VisibilityTesterTestVisibilityAnyDistance(
    _In_ PVISIBILITY_TESTER Tester,
    _In_ RAY WorldRay,
    _Out_ PBOOL Visible
    )
{
    ISTATUS Status;

    ASSERT(Tester->Scene != NULL);

    //
    // Ray is validated by RayTracerSetRay
    //

    if (Tester == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (Visible == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    WorldRay = RayNormalize(WorldRay);

    Status = RayTracerTestVisibilityAnyDistance(Tester->RayTracer,
                                                Tester->Scene,
                                                WorldRay,
                                                Tester->Epsilon,
                                                Visible);

    return Status;
}

VOID
VisibilityTesterFree(
    _In_opt_ _Post_invalid_ PVISIBILITY_TESTER Tester
    )
{
    if (Tester == NULL)
    {
        return;
    }

    RayTracerFree(Tester->RayTracer);
    SceneRelease(Tester->Scene);
    free(Tester);
}