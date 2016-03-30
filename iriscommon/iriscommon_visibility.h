/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    iriscommon_visibility.h

Abstract:

    This file contains the function definitions for the visibility routines.

--*/

#ifndef _IRIS_COMMON_VISIBILITY_
#define _IRIS_COMMON_VISIBILITY_

#include <iris.h>

//
// Types
//

typedef struct _RAYTRACER_TEST_VISIBILITY_ANY_DISTANCE_PROCESS_HIT_CONTEXT {
    FLOAT MinimumDistance;
    BOOL Visible;
} RAYTRACER_TEST_VISIBILITY_ANY_DISTANCE_PROCESS_HIT_CONTEXT, *PRAYTRACER_TEST_VISIBILITY_ANY_DISTANCE_PROCESS_HIT_CONTEXT;

typedef struct _RAYTRACER_TEST_VISIBILITY_PROCESS_HIT_CONTEXT {
    FLOAT DistanceToObject;
    FLOAT Epsilon;
    BOOL Visible;
} RAYTRACER_TEST_VISIBILITY_PROCESS_HIT_CONTEXT, *PRAYTRACER_TEST_VISIBILITY_PROCESS_HIT_CONTEXT;

//
// Functions
//

SFORCEINLINE
RAYTRACER_TEST_VISIBILITY_PROCESS_HIT_CONTEXT
RayTracerTestVisibilityProcessHitCreateContext(
    _In_ FLOAT DistanceToObject,
    _In_ FLOAT Epsilon,
    _In_ BOOL Visible
    )
{
    RAYTRACER_TEST_VISIBILITY_PROCESS_HIT_CONTEXT Context;

    ASSERT(IsFiniteFloat(DistanceToObject) != FALSE);
    ASSERT(IsGreaterThanOrEqualToZeroFloat(DistanceToObject) != FALSE);
    ASSERT(IsFiniteFloat(Epsilon) != FALSE);
    ASSERT(IsGreaterThanOrEqualToZeroFloat(Epsilon) != FALSE);

    Context.DistanceToObject = DistanceToObject;
    Context.Epsilon = Epsilon;
    Context.Visible = Visible;

    return Context;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
STATIC
ISTATUS
RayTracerTestVisibilityProcessHit(
    _Inout_ PVOID Context,
    _In_ PCHIT Hit
    )
{
    PRAYTRACER_TEST_VISIBILITY_PROCESS_HIT_CONTEXT TestContext;
    
    TestContext = (PRAYTRACER_TEST_VISIBILITY_PROCESS_HIT_CONTEXT) Context;
    
    if (TestContext->Epsilon < Hit->Distance &&
        Hit->Distance < TestContext->DistanceToObject)
    {
        TestContext->Visible = FALSE;
        return ISTATUS_NO_MORE_DATA;
    }
    
    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS
RayTracerTestVisibility(
    _In_ PRAYTRACER RayTracer,
    _In_ PRAYTRACER_TEST_GEOMETRY_ROUTINE TestShapesRoutine,
    _In_opt_ PCVOID TestShapesContext,
    _In_ RAY WorldRay,
    _In_ FLOAT DistanceToObject,
    _In_ FLOAT Epsilon,
    _Out_ PBOOL Visible
    )
{
    RAYTRACER_TEST_VISIBILITY_PROCESS_HIT_CONTEXT Context;
    ISTATUS Status;

    if (TestShapesRoutine == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (IsFiniteFloat(DistanceToObject) == FALSE ||
        IsGreaterThanOrEqualToZeroFloat(DistanceToObject) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    if (IsFiniteFloat(Epsilon) == FALSE ||
        IsGreaterThanOrEqualToZeroFloat(Epsilon) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_05;
    }

    if (Visible == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_06;
    }

    if (Epsilon < DistanceToObject &&
        IsFiniteFloat(DistanceToObject) != FALSE)
    {
        DistanceToObject -= Epsilon;
    }

    Context = RayTracerTestVisibilityProcessHitCreateContext(DistanceToObject,
                                                             Epsilon,
                                                             TRUE);

    Status = RayTracerTraceSceneProcessAllHitsOutOfOrder(RayTracer,
                                                         WorldRay,
                                                         TestShapesRoutine,
                                                         TestShapesContext,
                                                         RayTracerTestVisibilityProcessHit,
                                                         &Context);

    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }

    *Visible = Context.Visible;
    return ISTATUS_SUCCESS;
}

//
// This is needed since not all C environments support the INFINITY macro
//

SFORCEINLINE
RAYTRACER_TEST_VISIBILITY_ANY_DISTANCE_PROCESS_HIT_CONTEXT
RayTracerTestVisibilityAnyDistanceProcessHitCreateContext(
    _In_ FLOAT MinimumDistance,
    _In_ BOOL Visible
    )
{
    RAYTRACER_TEST_VISIBILITY_ANY_DISTANCE_PROCESS_HIT_CONTEXT Context;

    ASSERT(IsFiniteFloat(MinimumDistance) != FALSE);
    ASSERT(IsGreaterThanOrEqualToZeroFloat(MinimumDistance) != FALSE);

    Context.MinimumDistance = MinimumDistance;
    Context.Visible = Visible;

    return Context;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
STATIC
ISTATUS
RayTracerTestVisibilityAnyDistanceProcessHit(
    _Inout_ PVOID Context,
    _In_ PCHIT Hit
    )
{
    PRAYTRACER_TEST_VISIBILITY_ANY_DISTANCE_PROCESS_HIT_CONTEXT TestContext;
    
    TestContext = (PRAYTRACER_TEST_VISIBILITY_ANY_DISTANCE_PROCESS_HIT_CONTEXT) Context;
    
    if (TestContext->MinimumDistance < Hit->Distance &&
        IsInfiniteFloat(Hit->Distance) != FALSE)
    {
        TestContext->Visible = FALSE;
        return ISTATUS_NO_MORE_DATA;
    }
    
    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS
RayTracerTestVisibilityAnyDistance(
    _In_ PRAYTRACER RayTracer,
    _In_ PRAYTRACER_TEST_GEOMETRY_ROUTINE TestShapesRoutine,
    _In_opt_ PCVOID TestShapesContext,
    _In_ RAY WorldRay,
    _In_ FLOAT MinimumDistance,
    _Out_ PBOOL Visible
    )
{
    RAYTRACER_TEST_VISIBILITY_ANY_DISTANCE_PROCESS_HIT_CONTEXT Context;
    ISTATUS Status;

    if (TestShapesRoutine == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (IsFiniteFloat(MinimumDistance) == FALSE ||
        IsGreaterThanOrEqualToZeroFloat(MinimumDistance) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    if (Visible == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_05;
    }

    Context = RayTracerTestVisibilityAnyDistanceProcessHitCreateContext(MinimumDistance, TRUE);

    Status = RayTracerTraceSceneProcessAllHitsOutOfOrder(RayTracer,
                                                         WorldRay,
                                                         TestShapesRoutine,
                                                         TestShapesContext,
                                                         RayTracerTestVisibilityAnyDistanceProcessHit,
                                                         &Context);

    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }

    *Visible = Context.Visible;
    return ISTATUS_SUCCESS;
}

#endif // _IRIS_COMMON_VISIBILITY_