/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    visibilitytester.c

Abstract:

    This file contains the function definitions for the VISIBILITY_TESTER type.

--*/

#include <irisshadingmodelp.h>

struct _VISIBILITY_TESTER {
    PRAYTRACER RayTracer;
    FLOAT Epsilon;
    PCSCENE Scene;
};

_Check_return_
_Ret_maybenull_
PVISIBILITY_TESTER
VisibilityTesterAllocate(
    _In_ PCSCENE Scene,
    _In_ FLOAT Epsilon
    )
{
    VECTOR3 TemporaryDirection;
    PVISIBILITY_TESTER Tester;
    POINT3 TemporaryOrigin;
    PRAYTRACER RayTracer;
    RAY TemporaryRay;

    if (Scene == NULL ||
        IsNormalFloat(Epsilon) == FALSE ||
        IsFiniteFloat(Epsilon) == FALSE ||
        Epsilon < (FLOAT) 0.0)
    {
        return NULL;
    }

    Tester = (PVISIBILITY_TESTER) malloc(sizeof(VISIBILITY_TESTER));

    if (Tester == NULL)
    {
        return NULL;
    }

    TemporaryDirection = VectorCreate((FLOAT) 0.0, (FLOAT) 0.0, (FLOAT) 1.0);
    TemporaryOrigin = PointCreate((FLOAT) 0.0, (FLOAT) 0.0, (FLOAT) 0.0);
    TemporaryRay = RayCreate(TemporaryOrigin, TemporaryDirection);

    RayTracer = RayTracerAllocate(TemporaryRay);

    if (RayTracer == NULL)
    {
        free(Tester);
        return NULL;
    }

    Tester->Scene = Scene;
    Tester->RayTracer = RayTracer;
    Tester->Epsilon = MaxFloat(Epsilon, (FLOAT) 0.0);

    return Tester;
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

    if (Tester == NULL ||
        IsNormalFloat(DistanceToObject) == FALSE ||
        IsFiniteFloat(DistanceToObject) == FALSE ||
        DistanceToObject < (FLOAT) 0.0)
    {
        return ISTATUS_INVALID_ARGUMENT;
    }

    RayTracer = Tester->RayTracer;

    Status = RayTracerSetRay(RayTracer, WorldRay, TRUE);

    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }

    Status = SceneTrace(Tester->Scene,
                        WorldRay,
                        RayTracer);

    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }

    Status = RayTracerGetNextShapeHit(RayTracer, &ShapeHit);

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

        Status = RayTracerGetNextShapeHit(RayTracer, &ShapeHit);
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
        return ISTATUS_INVALID_ARGUMENT;
    }

    RayTracer = Tester->RayTracer;

    Status = RayTracerSetRay(RayTracer, WorldRay, TRUE);

    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }

    Status = SceneTrace(Tester->Scene,
                        WorldRay,
                        RayTracer);

    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }

    Status = RayTracerGetNextShapeHit(RayTracer, &ShapeHit);

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

        Status = RayTracerGetNextShapeHit(RayTracer, &ShapeHit);
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

    RayTracerFree(Tester->RayTracer);
    free(Tester);
}