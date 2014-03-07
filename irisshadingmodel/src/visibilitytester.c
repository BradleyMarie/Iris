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
    PVISIBILITY_TESTER Tester;
    PRAYTRACER RayTracer;

    ASSERT(Scene != NULL);
    ASSERT(IsNormalFloat(Epsilon));
    ASSERT(IsFiniteFloat(Epsilon));

    Tester = (PVISIBILITY_TESTER) malloc(sizeof(VISIBILITY_TESTER));

    if (Tester == NULL)
    {
        return NULL;
    }

    RayTracer = RayTracerAllocate();

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
    PCGEOMETRY_HIT *HitList;
    RAY NormalizedWorldRay;
	PRAYTRACER RayTracer;
    SIZE_T NumberOfHits;
	ISTATUS Status;
    SIZE_T Index;

    ASSERT(Tester != NULL);
    ASSERT(WorldRay != NULL);
    ASSERT(IsNormalFloat(DistanceToObject));
    ASSERT(IsFiniteFloat(DistanceToObject));
    ASSERT((FLOAT) 0.0 <= DistanceToObject);

    VectorNormalize(&WorldRay->Direction, &NormalizedWorldRay.Direction);
    NormalizedWorldRay.Origin = WorldRay->Origin;

    RayTracer = Tester->RayTracer;

    RayTracerClearResults(RayTracer);

    Status = SceneTrace(Tester->Scene,
                        &NormalizedWorldRay,
                        RayTracer);

    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }

    RayTracerGetResults(RayTracer,
                        FALSE,
                        &HitList,
                        &NumberOfHits);

    DistanceToObject += Tester->Epsilon;

    for (Index = 0; Index < NumberOfHits; Index++)
    {
        if (HitList[Index]->Distance < DistanceToObject)
        {
            *Visible = FALSE;
            return ISTATUS_SUCCESS;
        }
    }

    *Visible = TRUE;
    return ISTATUS_SUCCESS;
}

VOID
VisibilityTesterFree(
    _Pre_maybenull_ _Post_invalid_ PVISIBILITY_TESTER Tester
    )
{
    if (Tester == NULL)
    {
        return;
    }

    RayTracerFree(Tester->RayTracer);
    free(Tester);
}