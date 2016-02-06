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

typedef struct _VISIBILITY_TEST_ANY_DISTANCE_CONTEXT {
    FLOAT MinimumDistance;
    BOOL Visible;
} VISIBILITY_TEST_ANY_DISTANCE_CONTEXT, *PVISIBILITY_TEST_ANY_DISTANCE_CONTEXT;

typedef struct _VISIBILITY_TEST_CONTEXT {
    FLOAT DistanceToObject;
    FLOAT Epsilon;
    BOOL Visible;
} VISIBILITY_TEXT_CONTEXT, *PVISIBILITY_TEST_CONTEXT;

//
// Functions
//

SFORCEINLINE
VISIBILITY_TEXT_CONTEXT
VisibilityTesterTestCreateContext(
    _In_ FLOAT DistanceToObject,
    _In_ FLOAT Epsilon,
    _In_ BOOL Visible
    )
{
    VISIBILITY_TEXT_CONTEXT Context;

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
VisibilityCheckerRoutine(
    _Inout_ PVOID Context,
    _In_ PCSHAPE_HIT ShapeHit
    )
{
    PVISIBILITY_TEST_CONTEXT TestContext;
    
    TestContext = (PVISIBILITY_TEST_CONTEXT) Context;
    
    if (TestContext->Epsilon < ShapeHit->Distance &&
        ShapeHit->Distance < TestContext->DistanceToObject)
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
RayTracerOwnerTestVisibility(
    _In_ PRAYTRACER_OWNER RayTracerOwner,
    _In_ PCSCENE Scene,
    _In_ RAY WorldRay,
    _In_ FLOAT DistanceToObject,
    _In_ FLOAT Epsilon,
    _Out_ PBOOL Visible
    )
{
    VISIBILITY_TEXT_CONTEXT Context;
    ISTATUS Status;

    if (IsFiniteFloat(DistanceToObject) == FALSE ||
        IsGreaterThanOrEqualToZeroFloat(DistanceToObject) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    if (IsFiniteFloat(Epsilon) == FALSE ||
        IsGreaterThanOrEqualToZeroFloat(Epsilon) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    if (Visible == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_05;
    }

    if (Epsilon < DistanceToObject &&
        IsFiniteFloat(DistanceToObject) != FALSE)
    {
        DistanceToObject -= Epsilon;
    }

    Context = VisibilityTesterTestCreateContext(DistanceToObject,
                                                Epsilon,
                                                TRUE);

    Status = RayTracerOwnerTraceSceneFindAllHitsUnsorted(RayTracerOwner,
                                                         Scene,
                                                         WorldRay,
                                                         VisibilityCheckerRoutine,
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
VISIBILITY_TEST_ANY_DISTANCE_CONTEXT
VisibilityTesterTestAnyDistanceCreateContext(
    _In_ FLOAT MinimumDistance,
    _In_ BOOL Visible
    )
{
    VISIBILITY_TEST_ANY_DISTANCE_CONTEXT Context;

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
VisibilityCheckerLessThanInfinityRoutine(
    _Inout_ PVOID Context,
    _In_ PCSHAPE_HIT ShapeHit
    )
{
    PVISIBILITY_TEST_ANY_DISTANCE_CONTEXT TestContext;
    
    TestContext = (PVISIBILITY_TEST_ANY_DISTANCE_CONTEXT) Context;
    
    if (TestContext->MinimumDistance < ShapeHit->Distance &&
        IsInfiniteFloat(ShapeHit->Distance) != FALSE)
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
RayTracerOwnerTestVisibilityAnyDistance(
    _In_ PRAYTRACER_OWNER RayTracerOwner,
    _In_ PCSCENE Scene,
    _In_ RAY WorldRay,
    _In_ FLOAT MinimumDistance,
    _Out_ PBOOL Visible
    )
{
    VISIBILITY_TEST_ANY_DISTANCE_CONTEXT Context;
    ISTATUS Status;

    if (IsFiniteFloat(MinimumDistance) == FALSE ||
        IsGreaterThanOrEqualToZeroFloat(MinimumDistance) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    if (Visible == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    Context = VisibilityTesterTestAnyDistanceCreateContext(MinimumDistance, TRUE);

    Status = RayTracerOwnerTraceSceneFindAllHitsUnsorted(RayTracerOwner,
                                                         Scene,
                                                         WorldRay,
                                                         VisibilityCheckerLessThanInfinityRoutine,
                                                         &Context);

    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }

    *Visible = Context.Visible;
    return ISTATUS_SUCCESS;
}

#endif // _IRIS_COMMON_VISIBILITY_