/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    visibilitytester.c

Abstract:

    This file contains the definitions for the PBR_VISIBILITY_TESTER type.

--*/

#include <irisphysxp.h>

//
// Types
//

typedef struct _PBR_VISIBILITY_TESTER_TEST_VISIBILITY_ANY_DISTANCE_PROCESS_HIT_CONTEXT {
    FLOAT MinimumDistance;
    BOOL Visible;
} PBR_VISIBILITY_TESTER_TEST_VISIBILITY_ANY_DISTANCE_PROCESS_HIT_CONTEXT, *PPBR_VISIBILITY_TESTER_TEST_VISIBILITY_ANY_DISTANCE_PROCESS_HIT_CONTEXT;

typedef struct _PBR_VISIBILITY_TESTER_TEST_VISIBILITY_PROCESS_HIT_CONTEXT {
    FLOAT DistanceToObject;
    FLOAT Epsilon;
    BOOL Visible;
} PBR_VISIBILITY_TESTER_TEST_VISIBILITY_PROCESS_HIT_CONTEXT, *PPBR_VISIBILITY_TESTER_TEST_VISIBILITY_PROCESS_HIT_CONTEXT;

typedef struct _PBR_VISIBILITY_TESTER_TEST_LIGHT_VISIBILITY_PROCESS_HIT_CONTEXT {
    PCLIGHT TargetLight;
    BOOL Visible;
} PBR_VISIBILITY_TESTER_TEST_LIGHT_VISIBILITY_PROCESS_HIT_CONTEXT, *PPBR_VISIBILITY_TESTER_TEST_LIGHT_VISIBILITY_PROCESS_HIT_CONTEXT;

//
// Static Functions
//

SFORCEINLINE
PBR_VISIBILITY_TESTER_TEST_VISIBILITY_PROCESS_HIT_CONTEXT
PBRVisibilityTesterTestVisibilityProcessHitCreateContext(
    _In_ FLOAT DistanceToObject,
    _In_ FLOAT Epsilon,
    _In_ BOOL Visible
    )
{
    PBR_VISIBILITY_TESTER_TEST_VISIBILITY_PROCESS_HIT_CONTEXT Context;

    ASSERT(IsFiniteFloat(DistanceToObject) != FALSE);
    ASSERT(IsGreaterThanOrEqualToZeroFloat(DistanceToObject) != FALSE);
    ASSERT(IsFiniteFloat(Epsilon) != FALSE);
    ASSERT(IsGreaterThanOrEqualToZeroFloat(Epsilon) != FALSE);

    Context.DistanceToObject = DistanceToObject;
    Context.Epsilon = Epsilon;
    Context.Visible = Visible;

    return Context;
}

SFORCEINLINE
PBR_VISIBILITY_TESTER_TEST_VISIBILITY_ANY_DISTANCE_PROCESS_HIT_CONTEXT
PBRVisibilityTesterTestVisibilityAnyDistanceProcessHitCreateContext(
    _In_ FLOAT MinimumDistance,
    _In_ BOOL Visible
    )
{
    PBR_VISIBILITY_TESTER_TEST_VISIBILITY_ANY_DISTANCE_PROCESS_HIT_CONTEXT Context;

    ASSERT(IsFiniteFloat(MinimumDistance) != FALSE);
    ASSERT(IsGreaterThanOrEqualToZeroFloat(MinimumDistance) != FALSE);

    Context.MinimumDistance = MinimumDistance;
    Context.Visible = Visible;

    return Context;
}

SFORCEINLINE
PBR_VISIBILITY_TESTER_TEST_LIGHT_VISIBILITY_PROCESS_HIT_CONTEXT
PBRVisibilityTesterTestLightVisibilityProcessHitCreateContext(
    _In_ PCLIGHT TargetLight,
    _In_ BOOL Visible
    )
{
    PBR_VISIBILITY_TESTER_TEST_LIGHT_VISIBILITY_PROCESS_HIT_CONTEXT Context;

    ASSERT(TargetLight != NULL);

    Context.TargetLight = TargetLight;
    Context.Visible = Visible;

    return Context;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
STATIC
ISTATUS
PBRVisibilityTesterTestLightVisibilityProcessHit(
    _Inout_ PVOID Context,
    _In_ PCHIT Hit
    )
{
    PPBR_VISIBILITY_TESTER_TEST_LIGHT_VISIBILITY_PROCESS_HIT_CONTEXT TestContext;
    PCPBR_GEOMETRY PBRGeometry;
    PCLIGHT ClosestLight;
    
    TestContext = (PPBR_VISIBILITY_TESTER_TEST_LIGHT_VISIBILITY_PROCESS_HIT_CONTEXT) Context;
    PBRGeometry = (PCPBR_GEOMETRY) Hit->Data;

    PBRGeometryGetLight(PBRGeometry, Hit->FaceHit, &ClosestLight);
    
    if (ClosestLight == TestContext->TargetLight)
    {
        TestContext->Visible = TRUE;
    }
    
    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
STATIC
ISTATUS
PBRVisibilityTesterTestVisibilityAnyDistanceProcessHit(
    _Inout_ PVOID Context,
    _In_ PCHIT Hit
    )
{
    PPBR_VISIBILITY_TESTER_TEST_VISIBILITY_ANY_DISTANCE_PROCESS_HIT_CONTEXT TestContext;
    
    TestContext = (PPBR_VISIBILITY_TESTER_TEST_VISIBILITY_ANY_DISTANCE_PROCESS_HIT_CONTEXT) Context;
    
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
STATIC
ISTATUS
PBRVisibilityTesterTestVisibilityProcessHit(
    _Inout_ PVOID Context,
    _In_ PCHIT Hit
    )
{
    PPBR_VISIBILITY_TESTER_TEST_VISIBILITY_PROCESS_HIT_CONTEXT TestContext;
    
    TestContext = (PPBR_VISIBILITY_TESTER_TEST_VISIBILITY_PROCESS_HIT_CONTEXT) Context;
    
    if (TestContext->Epsilon < Hit->Distance &&
        Hit->Distance < TestContext->DistanceToObject)
    {
        TestContext->Visible = FALSE;
        return ISTATUS_NO_MORE_DATA;
    }
    
    return ISTATUS_SUCCESS;
}

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
PBRVisibilityTesterTestVisibility(
    _In_ PPBR_VISIBILITY_TESTER PBRVisibilityTester,
    _In_ RAY WorldRay,
    _In_ FLOAT DistanceToObject,
    _Out_ PBOOL Visible
    )
{
    PBR_VISIBILITY_TESTER_TEST_VISIBILITY_PROCESS_HIT_CONTEXT Context;
    ISTATUS Status;
    
    if (PBRVisibilityTester == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }
    
    if (RayValidate(WorldRay) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
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

    Context = PBRVisibilityTesterTestVisibilityProcessHitCreateContext(DistanceToObject,
                                                                       PBRVisibilityTester->Epsilon,
                                                                       TRUE);
    


    Status = RayTracerAdapterTraceSceneProcessAllHitsOutOfOrder(PBRVisibilityTester->RayTracer,
                                                                PBRVisibilityTester->TestGeometryRoutine,
                                                                PBRVisibilityTester->TestGeometryRoutineContext,
                                                                WorldRay,
                                                                PBRVisibilityTesterTestVisibilityProcessHit,
                                                                &Context);

    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }

    *Visible = Context.Visible;
    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
PBRVisibilityTesterTestVisibilityAnyDistance(
    _In_ PPBR_VISIBILITY_TESTER PBRVisibilityTester,
    _In_ RAY WorldRay,
    _Out_ PBOOL Visible
    )
{
    PBR_VISIBILITY_TESTER_TEST_VISIBILITY_ANY_DISTANCE_PROCESS_HIT_CONTEXT Context;
    ISTATUS Status;

    if (PBRVisibilityTester == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }
    
    if (RayValidate(WorldRay) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }
    
    if (Visible == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }
    
    Context = PBRVisibilityTesterTestVisibilityAnyDistanceProcessHitCreateContext(PBRVisibilityTester->Epsilon, TRUE);

    Status = RayTracerAdapterTraceSceneProcessAllHitsOutOfOrder(PBRVisibilityTester->RayTracer,
                                                                PBRVisibilityTester->TestGeometryRoutine,
                                                                PBRVisibilityTester->TestGeometryRoutineContext,
                                                                WorldRay,
                                                                RayTracerTestVisibilityAnyDistanceProcessHit,
                                                                &Context);

    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }

    *Visible = Context.Visible;
    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
PBRVisibilityTesterTestLightVisibility(
    _In_ PPBR_VISIBILITY_TESTER PBRVisibilityTester,
    _In_ RAY WorldRay,
    _In_ PCLIGHT Light,
    _Out_ PBOOL Visible
    )
{
    PBR_VISIBILITY_TESTER_TEST_LIGHT_VISIBILITY_PROCESS_HIT_CONTEXT ProcessHitContext;
    ISTATUS Status;
    
    if (PBRVisibilityTester == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }
    
    if (RayValidate(WorldRay) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }
    
    if (Light == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }
    
    if (Visible == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    ProcessHitContext = PBRVisibilityTesterTestLightVisibilityProcessHitCreateContext(Light, FALSE);

    Status = RayTracerAdapterTraceSceneProcessClosestHit(PBRVisibilityTester->RayTracer,
                                                         PBRVisibilityTester->TestGeometryRoutine,
                                                         PBRVisibilityTester->TestGeometryRoutineContext,
                                                         WorldRay,
                                                         PBRVisibilityTester->Epsilon,
                                                         PBRVisibilityTesterTestLightVisibilityProcessHit,
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