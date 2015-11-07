/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    visibilitytester.c

Abstract:

    This file contains the function definitions for the VISIBILITY_TESTER type.

--*/

#include <irisshadingmodelp.h>

struct _VISIBILITY_TESTER {
    PRAYTRACER_OWNER RayTracerOwner;
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
    VECTOR3 TemporaryDirection;
    PVISIBILITY_TESTER Tester;
    POINT3 TemporaryOrigin;
    PRAYTRACER_OWNER RayTracerOwner;
    RAY TemporaryRay;
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

    TemporaryDirection = VectorCreate((FLOAT) 0.0, (FLOAT) 0.0, (FLOAT) 1.0);
    TemporaryOrigin = PointCreate((FLOAT) 0.0, (FLOAT) 0.0, (FLOAT) 0.0);
    TemporaryRay = RayCreate(TemporaryOrigin, TemporaryDirection);

    Status = RayTracerOwnerAllocate(TemporaryRay, &RayTracerOwner);

    if (Status != ISTATUS_SUCCESS)
    {
        free(Tester);
        return ISTATUS_ALLOCATION_FAILED;
    }

    SceneReference(Scene);

    Tester->Scene = Scene;
    Tester->RayTracerOwner = RayTracerOwner;
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
    PRAYTRACER RayTracer;
    PCSHAPE_HIT ShapeHit;
    ISTATUS Status;

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

    Status = RayTracerOwnerSetRay(Tester->RayTracerOwner, WorldRay, TRUE);

    switch (Status)
    {
        case ISTATUS_SUCCESS:
            break;
        case ISTATUS_INVALID_ARGUMENT_01:
            return ISTATUS_INVALID_ARGUMENT_01;
        default:
            ASSERT(FALSE);
            return Status;
    }

    RayTracer = RayTracerOwnerGetRayTracer(Tester->RayTracerOwner);

    Status = SceneTrace(Tester->Scene,
                        RayTracer);

    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }

    Status = RayTracerOwnerGetNextShapeHit(Tester->RayTracerOwner, &ShapeHit);

    if (Status == ISTATUS_NO_MORE_DATA)
    {
        *Visible = TRUE;
        return ISTATUS_SUCCESS;
    }

    if (Tester->Epsilon < DistanceToObject &&
        IsFiniteFloat(DistanceToObject) != FALSE)
    {
        DistanceToObject -= Tester->Epsilon;
    }

    do
    {
        if (Tester->Epsilon < ShapeHit->Distance &&
            ShapeHit->Distance < DistanceToObject)
        {
            *Visible = FALSE;
            return ISTATUS_SUCCESS;
        }

        Status = RayTracerOwnerGetNextShapeHit(Tester->RayTracerOwner, &ShapeHit);
    } while (Status == ISTATUS_SUCCESS);

    *Visible = TRUE;
    return ISTATUS_SUCCESS;
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
    PRAYTRACER RayTracer;
    PCSHAPE_HIT ShapeHit;
    ISTATUS Status;

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

    Status = RayTracerOwnerSetRay(Tester->RayTracerOwner, WorldRay, TRUE);

    switch (Status)
    {
        case ISTATUS_SUCCESS:
            break;
        case ISTATUS_INVALID_ARGUMENT_01:
            return ISTATUS_INVALID_ARGUMENT_01;
        default:
            ASSERT(FALSE);
            return Status;
    }

    RayTracer = RayTracerOwnerGetRayTracer(Tester->RayTracerOwner);

    Status = SceneTrace(Tester->Scene,
                        RayTracer);

    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }

    Status = RayTracerOwnerGetNextShapeHit(Tester->RayTracerOwner, &ShapeHit);

    if (Status == ISTATUS_NO_MORE_DATA)
    {
        *Visible = TRUE;
        return ISTATUS_SUCCESS;
    }

    do
    {
        if (Tester->Epsilon < ShapeHit->Distance)
        {
            *Visible = FALSE;
            return ISTATUS_SUCCESS;
        }

        Status = RayTracerOwnerGetNextShapeHit(Tester->RayTracerOwner, &ShapeHit);
    } while (Status == ISTATUS_SUCCESS);

    *Visible = TRUE;
    return ISTATUS_SUCCESS;
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

    RayTracerOwnerFree(Tester->RayTracerOwner);
    SceneDereference(Tester->Scene);
    free(Tester);
}