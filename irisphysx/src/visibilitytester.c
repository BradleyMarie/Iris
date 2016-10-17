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

typedef struct _PBR_VISIBILITY_TESTER_TEST_PBR_LIGHT_VISIBILITY_PROCESS_HIT_CONTEXT {
    PCPBR_LIGHT TargetPbrLight;
    BOOL Visible;
} PBR_VISIBILITY_TESTER_TEST_PBR_LIGHT_VISIBILITY_PROCESS_HIT_CONTEXT, *PPBR_VISIBILITY_TESTER_TEST_LIGHT_VISIBILITY_PROCESS_HIT_CONTEXT;

typedef struct _PBR_VISIBILITY_TESTER_FIND_DISTANCE_TO_LIGHT_PROCESS_HIT_CONTEXT {
    PCPBR_LIGHT TargetPbrLight;
    FLOAT DistanceToLight;
    BOOL Visible;
} PBR_VISIBILITY_TESTER_FIND_DISTANCE_TO_LIGHT_PROCESS_HIT_CONTEXT, *PPBR_VISIBILITY_TESTER_FIND_DISTANCE_TO_LIGHT_PROCESS_HIT_CONTEXT;

//
// Context Creation Functions
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
PBR_VISIBILITY_TESTER_TEST_PBR_LIGHT_VISIBILITY_PROCESS_HIT_CONTEXT
PBRVisibilityTesterTestLightVisibilityProcessHitCreateContext(
    _In_ PCPBR_LIGHT TargetPbrLight,
    _In_ BOOL Visible
    )
{
    PBR_VISIBILITY_TESTER_TEST_PBR_LIGHT_VISIBILITY_PROCESS_HIT_CONTEXT Context;

    ASSERT(TargetPbrLight != NULL);

    Context.TargetPbrLight = TargetPbrLight;
    Context.Visible = Visible;

    return Context;
}

SFORCEINLINE
PBR_VISIBILITY_TESTER_FIND_DISTANCE_TO_LIGHT_PROCESS_HIT_CONTEXT
PBRVisibilityTesterFindDistanceToLightProcessHitCreateContext(
    _In_ PCPBR_LIGHT TargetPbrLight,
    _In_ FLOAT DistanceToLight,
    _In_ BOOL Visible
    )
{
    PBR_VISIBILITY_TESTER_FIND_DISTANCE_TO_LIGHT_PROCESS_HIT_CONTEXT Context;

    ASSERT(TargetPbrLight != NULL);

    Context.TargetPbrLight = TargetPbrLight;
    Context.DistanceToLight = DistanceToLight;
    Context.Visible = Visible;

    return Context;
}

//
// Process Hit Callback Functions
//

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
    PCPBR_LIGHT ClosestPbrLight;
    
    TestContext = (PPBR_VISIBILITY_TESTER_TEST_LIGHT_VISIBILITY_PROCESS_HIT_CONTEXT) Context;
    PBRGeometry = (PCPBR_GEOMETRY) Hit->Data;

    PBRGeometryGetLight(PBRGeometry, Hit->FrontFace, &ClosestPbrLight);
    
    if (ClosestPbrLight == TestContext->TargetPbrLight)
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

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
STATIC
ISTATUS
PBRVisibilityTesterFindDistaneToLightProcessHit(
    _Inout_ PVOID Context,
    _In_ PCHIT Hit
    )
{
    PPBR_VISIBILITY_TESTER_FIND_DISTANCE_TO_LIGHT_PROCESS_HIT_CONTEXT TestContext;
    PCPBR_GEOMETRY PBRGeometry;
    PCPBR_LIGHT ClosestPbrLight;
    
    TestContext = (PPBR_VISIBILITY_TESTER_FIND_DISTANCE_TO_LIGHT_PROCESS_HIT_CONTEXT) Context;
    PBRGeometry = (PCPBR_GEOMETRY) Hit->Data;

    PBRGeometryGetLight(PBRGeometry, Hit->FrontFace, &ClosestPbrLight);
    
    if (ClosestPbrLight == TestContext->TargetPbrLight)
    {
        TestContext->Visible = TRUE;
        TestContext->DistanceToLight = Hit->Distance;
    }
    
    return ISTATUS_SUCCESS;
}

//
// Internal Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
PBRVisibilityTesterFindDistanceToLight(
    _In_ PPBR_VISIBILITY_TESTER PBRVisibilityTester,
    _In_ RAY WorldRay,
    _In_ PCPBR_LIGHT Light,
    _Out_ PBOOL Visible,
    _Out_ PFLOAT DistanceToLight
    )
{
    PBR_VISIBILITY_TESTER_FIND_DISTANCE_TO_LIGHT_PROCESS_HIT_CONTEXT ProcessHitContext;
    ISTATUS Status;

    ASSERT(PBRVisibilityTester != NULL);
    ASSERT(RayValidate(WorldRay) != FALSE);
    ASSERT(Light != NULL);
    ASSERT(Visible != NULL);
    ASSERT(DistanceToLight != NULL);

    ProcessHitContext = PBRVisibilityTesterFindDistanceToLightProcessHitCreateContext(Light, 
                                                                                      (FLOAT) 0.0f,
                                                                                      FALSE);

    Status = RayTracerAdapterTraceSceneProcessClosestHit(PBRVisibilityTester->RayTracer,
                                                         WorldRay,
                                                         PBRVisibilityTester->Epsilon,
                                                         PBRVisibilityTester->TestGeometryRoutine,
                                                         PBRVisibilityTester->TestGeometryRoutineContext,
                                                         PBRVisibilityTesterFindDistaneToLightProcessHit,
                                                         &ProcessHitContext);
    
    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }
    
    *Visible = ProcessHitContext.Visible;
    *DistanceToLight = ProcessHitContext.DistanceToLight;
    
    return ISTATUS_SUCCESS;
}

//
// Public Functions
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
                                                                WorldRay,
                                                                PBRVisibilityTester->TestGeometryRoutine,
                                                                PBRVisibilityTester->TestGeometryRoutineContext,
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
                                                                WorldRay,
                                                                PBRVisibilityTester->TestGeometryRoutine,
                                                                PBRVisibilityTester->TestGeometryRoutineContext,
                                                                PBRVisibilityTesterTestVisibilityAnyDistanceProcessHit,
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
    _In_ PCPBR_LIGHT PbrLight,
    _Out_ PBOOL Visible
    )
{
    PBR_VISIBILITY_TESTER_TEST_PBR_LIGHT_VISIBILITY_PROCESS_HIT_CONTEXT ProcessHitContext;
    ISTATUS Status;
    
    if (PBRVisibilityTester == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }
    
    if (RayValidate(WorldRay) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }
    
    if (PbrLight == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }
    
    if (Visible == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    ProcessHitContext = PBRVisibilityTesterTestLightVisibilityProcessHitCreateContext(PbrLight, FALSE);

    Status = RayTracerAdapterTraceSceneProcessClosestHit(PBRVisibilityTester->RayTracer,
                                                         WorldRay,
                                                         PBRVisibilityTester->Epsilon,
                                                         PBRVisibilityTester->TestGeometryRoutine,
                                                         PBRVisibilityTester->TestGeometryRoutineContext,
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
