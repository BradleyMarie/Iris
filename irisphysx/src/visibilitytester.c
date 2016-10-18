/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    visibilitytester.c

Abstract:

    This file contains the definitions for the PHYSX_VISIBILITY_TESTER type.

--*/

#include <irisphysxp.h>

//
// Types
//

typedef struct _PHYSX_VISIBILITY_TESTER_TEST_VISIBILITY_ANY_DISTANCE_PROCESS_HIT_CONTEXT {
    FLOAT MinimumDistance;
    BOOL Visible;
} PHYSX_VISIBILITY_TESTER_TEST_VISIBILITY_ANY_DISTANCE_PROCESS_HIT_CONTEXT, *PPHYSX_VISIBILITY_TESTER_TEST_VISIBILITY_ANY_DISTANCE_PROCESS_HIT_CONTEXT;

typedef struct _PHYSX_VISIBILITY_TESTER_TEST_VISIBILITY_PROCESS_HIT_CONTEXT {
    FLOAT DistanceToObject;
    FLOAT Epsilon;
    BOOL Visible;
} PHYSX_VISIBILITY_TESTER_TEST_VISIBILITY_PROCESS_HIT_CONTEXT, *PPHYSX_VISIBILITY_TESTER_TEST_VISIBILITY_PROCESS_HIT_CONTEXT;

typedef struct _PHYSX_VISIBILITY_TESTER_TEST_PHYSX_LIGHT_VISIBILITY_PROCESS_HIT_CONTEXT {
    PCPHYSX_LIGHT TargetLight;
    BOOL Visible;
} PHYSX_VISIBILITY_TESTER_TEST_PHYSX_LIGHT_VISIBILITY_PROCESS_HIT_CONTEXT, *PPHYSX_VISIBILITY_TESTER_TEST_LIGHT_VISIBILITY_PROCESS_HIT_CONTEXT;

typedef struct _PHYSX_VISIBILITY_TESTER_FIND_DISTANCE_TO_LIGHT_PROCESS_HIT_CONTEXT {
    PCPHYSX_LIGHT TargetLight;
    FLOAT DistanceToLight;
    BOOL Visible;
} PHYSX_VISIBILITY_TESTER_FIND_DISTANCE_TO_LIGHT_PROCESS_HIT_CONTEXT, *PPHYSX_VISIBILITY_TESTER_FIND_DISTANCE_TO_LIGHT_PROCESS_HIT_CONTEXT;

//
// Context Creation Functions
//

SFORCEINLINE
PHYSX_VISIBILITY_TESTER_TEST_VISIBILITY_PROCESS_HIT_CONTEXT
PhysxVisibilityTesterTestVisibilityProcessHitCreateContext(
    _In_ FLOAT DistanceToObject,
    _In_ FLOAT Epsilon,
    _In_ BOOL Visible
    )
{
    PHYSX_VISIBILITY_TESTER_TEST_VISIBILITY_PROCESS_HIT_CONTEXT Context;

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
PHYSX_VISIBILITY_TESTER_TEST_VISIBILITY_ANY_DISTANCE_PROCESS_HIT_CONTEXT
PhysxVisibilityTesterTestVisibilityAnyDistanceProcessHitCreateContext(
    _In_ FLOAT MinimumDistance,
    _In_ BOOL Visible
    )
{
    PHYSX_VISIBILITY_TESTER_TEST_VISIBILITY_ANY_DISTANCE_PROCESS_HIT_CONTEXT Context;

    ASSERT(IsFiniteFloat(MinimumDistance) != FALSE);
    ASSERT(IsGreaterThanOrEqualToZeroFloat(MinimumDistance) != FALSE);

    Context.MinimumDistance = MinimumDistance;
    Context.Visible = Visible;

    return Context;
}

SFORCEINLINE
PHYSX_VISIBILITY_TESTER_TEST_PHYSX_LIGHT_VISIBILITY_PROCESS_HIT_CONTEXT
PhysxVisibilityTesterTestLightVisibilityProcessHitCreateContext(
    _In_ PCPHYSX_LIGHT TargetLight,
    _In_ BOOL Visible
    )
{
    PHYSX_VISIBILITY_TESTER_TEST_PHYSX_LIGHT_VISIBILITY_PROCESS_HIT_CONTEXT Context;

    ASSERT(TargetLight != NULL);

    Context.TargetLight = TargetLight;
    Context.Visible = Visible;

    return Context;
}

SFORCEINLINE
PHYSX_VISIBILITY_TESTER_FIND_DISTANCE_TO_LIGHT_PROCESS_HIT_CONTEXT
PhysxVisibilityTesterFindDistanceToLightProcessHitCreateContext(
    _In_ PCPHYSX_LIGHT TargetLight,
    _In_ FLOAT DistanceToLight,
    _In_ BOOL Visible
    )
{
    PHYSX_VISIBILITY_TESTER_FIND_DISTANCE_TO_LIGHT_PROCESS_HIT_CONTEXT Context;

    ASSERT(TargetLight != NULL);

    Context.TargetLight = TargetLight;
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
PhysxVisibilityTesterTestLightVisibilityProcessHit(
    _Inout_ PVOID Context,
    _In_ PCHIT Hit
    )
{
    PPHYSX_VISIBILITY_TESTER_TEST_LIGHT_VISIBILITY_PROCESS_HIT_CONTEXT TestContext;
    PCPHYSX_GEOMETRY Geometry;
    PCPHYSX_LIGHT ClosestLight;
    
    TestContext = (PPHYSX_VISIBILITY_TESTER_TEST_LIGHT_VISIBILITY_PROCESS_HIT_CONTEXT) Context;
    Geometry = (PCPHYSX_GEOMETRY) Hit->Data;

    PhysxGeometryGetLight(Geometry, Hit->FrontFace, &ClosestLight);
    
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
PhysxVisibilityTesterTestVisibilityAnyDistanceProcessHit(
    _Inout_ PVOID Context,
    _In_ PCHIT Hit
    )
{
    PPHYSX_VISIBILITY_TESTER_TEST_VISIBILITY_ANY_DISTANCE_PROCESS_HIT_CONTEXT TestContext;
    
    TestContext = (PPHYSX_VISIBILITY_TESTER_TEST_VISIBILITY_ANY_DISTANCE_PROCESS_HIT_CONTEXT) Context;
    
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
PhysxVisibilityTesterTestVisibilityProcessHit(
    _Inout_ PVOID Context,
    _In_ PCHIT Hit
    )
{
    PPHYSX_VISIBILITY_TESTER_TEST_VISIBILITY_PROCESS_HIT_CONTEXT TestContext;
    
    TestContext = (PPHYSX_VISIBILITY_TESTER_TEST_VISIBILITY_PROCESS_HIT_CONTEXT) Context;
    
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
PhysxVisibilityTesterFindDistaneToLightProcessHit(
    _Inout_ PVOID Context,
    _In_ PCHIT Hit
    )
{
    PPHYSX_VISIBILITY_TESTER_FIND_DISTANCE_TO_LIGHT_PROCESS_HIT_CONTEXT TestContext;
    PCPHYSX_GEOMETRY Geometry;
    PCPHYSX_LIGHT ClosestLight;
    
    TestContext = (PPHYSX_VISIBILITY_TESTER_FIND_DISTANCE_TO_LIGHT_PROCESS_HIT_CONTEXT) Context;
    Geometry = (PCPHYSX_GEOMETRY) Hit->Data;

    PhysxGeometryGetLight(Geometry, Hit->FrontFace, &ClosestLight);
    
    if (ClosestLight == TestContext->TargetLight)
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
PhysxVisibilityTesterFindDistanceToLight(
    _In_ PPHYSX_VISIBILITY_TESTER VisibilityTester,
    _In_ RAY WorldRay,
    _In_ PCPHYSX_LIGHT Light,
    _Out_ PBOOL Visible,
    _Out_ PFLOAT DistanceToLight
    )
{
    PHYSX_VISIBILITY_TESTER_FIND_DISTANCE_TO_LIGHT_PROCESS_HIT_CONTEXT ProcessHitContext;
    ISTATUS Status;

    ASSERT(VisibilityTester != NULL);
    ASSERT(RayValidate(WorldRay) != FALSE);
    ASSERT(Light != NULL);
    ASSERT(Visible != NULL);
    ASSERT(DistanceToLight != NULL);

    ProcessHitContext = PhysxVisibilityTesterFindDistanceToLightProcessHitCreateContext(Light, 
                                                                                      (FLOAT) 0.0f,
                                                                                      FALSE);

    Status = RayTracerAdapterTraceSceneProcessClosestHit(VisibilityTester->RayTracer,
                                                         WorldRay,
                                                         VisibilityTester->Epsilon,
                                                         VisibilityTester->TestGeometryRoutine,
                                                         VisibilityTester->TestGeometryRoutineContext,
                                                         PhysxVisibilityTesterFindDistaneToLightProcessHit,
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
PhysxVisibilityTesterTestVisibility(
    _In_ PPHYSX_VISIBILITY_TESTER VisibilityTester,
    _In_ RAY WorldRay,
    _In_ FLOAT DistanceToObject,
    _Out_ PBOOL Visible
    )
{
    PHYSX_VISIBILITY_TESTER_TEST_VISIBILITY_PROCESS_HIT_CONTEXT Context;
    ISTATUS Status;
    
    if (VisibilityTester == NULL)
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

    Context = PhysxVisibilityTesterTestVisibilityProcessHitCreateContext(DistanceToObject,
                                                                         VisibilityTester->Epsilon,
                                                                         TRUE);
    
    Status = RayTracerAdapterTraceSceneProcessAllHitsOutOfOrder(VisibilityTester->RayTracer,
                                                                WorldRay,
                                                                VisibilityTester->TestGeometryRoutine,
                                                                VisibilityTester->TestGeometryRoutineContext,
                                                                PhysxVisibilityTesterTestVisibilityProcessHit,
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
PhysxVisibilityTesterTestVisibilityAnyDistance(
    _In_ PPHYSX_VISIBILITY_TESTER VisibilityTester,
    _In_ RAY WorldRay,
    _Out_ PBOOL Visible
    )
{
    PHYSX_VISIBILITY_TESTER_TEST_VISIBILITY_ANY_DISTANCE_PROCESS_HIT_CONTEXT Context;
    ISTATUS Status;

    if (VisibilityTester == NULL)
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
    
    Context = PhysxVisibilityTesterTestVisibilityAnyDistanceProcessHitCreateContext(VisibilityTester->Epsilon, TRUE);

    Status = RayTracerAdapterTraceSceneProcessAllHitsOutOfOrder(VisibilityTester->RayTracer,
                                                                WorldRay,
                                                                VisibilityTester->TestGeometryRoutine,
                                                                VisibilityTester->TestGeometryRoutineContext,
                                                                PhysxVisibilityTesterTestVisibilityAnyDistanceProcessHit,
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
PhysxVisibilityTesterTestLightVisibility(
    _In_ PPHYSX_VISIBILITY_TESTER VisibilityTester,
    _In_ RAY WorldRay,
    _In_ PCPHYSX_LIGHT Light,
    _Out_ PBOOL Visible
    )
{
    PHYSX_VISIBILITY_TESTER_TEST_PHYSX_LIGHT_VISIBILITY_PROCESS_HIT_CONTEXT ProcessHitContext;
    ISTATUS Status;
    
    if (VisibilityTester == NULL)
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

    ProcessHitContext = PhysxVisibilityTesterTestLightVisibilityProcessHitCreateContext(Light, FALSE);

    Status = RayTracerAdapterTraceSceneProcessClosestHit(VisibilityTester->RayTracer,
                                                         WorldRay,
                                                         VisibilityTester->Epsilon,
                                                         VisibilityTester->TestGeometryRoutine,
                                                         VisibilityTester->TestGeometryRoutineContext,
                                                         PhysxVisibilityTesterTestLightVisibilityProcessHit,
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
