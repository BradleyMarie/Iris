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

    ASSERT(Scene != NULL);
    ASSERT(IsNormalFloat(Epsilon));
    ASSERT(IsFiniteFloat(Epsilon));

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
    _In_ PCRAY WorldRay,
    _In_ FLOAT DistanceToObject,
    _Out_ PBOOL Visible
    )
{
    RAY NormalizedWorldRay;
    PRAYTRACER RayTracer;
    PCSHAPE_HIT ShapeHit;
    ISTATUS Status;

    ASSERT(Tester != NULL);
    ASSERT(WorldRay != NULL);
    ASSERT(IsNormalFloat(DistanceToObject));
    ASSERT(IsFiniteFloat(DistanceToObject));
    ASSERT((FLOAT) 0.0 <= DistanceToObject);

    NormalizedWorldRay = RayNormalize(*WorldRay);

    RayTracer = Tester->RayTracer;

    Status = RayTracerSetRay(RayTracer, NormalizedWorldRay);

    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }

    Status = SceneTrace(Tester->Scene,
                        &NormalizedWorldRay,
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

    if (Tester->Epsilon < DistanceToObject)
    {
        DistanceToObject -= Tester->Epsilon;
    }

    while (Status == ISTATUS_SUCCESS)
    {
        if (Tester->Epsilon < ShapeHit->Distance &&
            ShapeHit->Distance < DistanceToObject)
        {
            *Visible = FALSE;
            return ISTATUS_SUCCESS;
        }

        Status = RayTracerGetNextShapeHit(RayTracer, &ShapeHit);
    }

    *Visible = TRUE;
    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
VisibilityTesterTestVisibilityAnyDistance(
    _In_ PVISIBILITY_TESTER Tester,
    _In_ PCRAY WorldRay,
    _Out_ PBOOL Visible
    )
{
    RAY NormalizedWorldRay;
    PRAYTRACER RayTracer;
    PCSHAPE_HIT ShapeHit;
    ISTATUS Status;

    ASSERT(Tester != NULL);
    ASSERT(WorldRay != NULL);

    NormalizedWorldRay = RayNormalize(*WorldRay);

    RayTracer = Tester->RayTracer;

    Status = RayTracerSetRay(RayTracer, NormalizedWorldRay);

    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }

    Status = SceneTrace(Tester->Scene,
        &NormalizedWorldRay,
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

    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }

    *Visible = FALSE;
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