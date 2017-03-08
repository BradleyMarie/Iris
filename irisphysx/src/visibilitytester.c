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

typedef struct _PHYSX_VISIBILITY_TESTER_COMPUTE_PDF_PROCESS_HIT_CONTEXT {
    PCPHYSX_LIGHT Light;
    RAY ToLight;
    FLOAT InverseTotalSurfaceArea;
    FLOAT Pdf;
    BOOL FirstHit;
    POINT3 ClosestWorldPointOnLight;
} PHYSX_VISIBILITY_TESTER_COMPUTE_PDF_PROCESS_HIT_CONTEXT, *PPHYSX_VISIBILITY_TESTER_COMPUTE_PDF_PROCESS_HIT_CONTEXT;

typedef struct _PHYSX_VISIBILITY_TESTER_FIND_DISTANCE_TO_LIGHT_PROCESS_HIT_CONTEXT {
    PCPHYSX_LIGHT Light;
    FLOAT DistanceToLight;
    BOOL Visible;
} PHYSX_VISIBILITY_TESTER_FIND_DISTANCE_TO_LIGHT_PROCESS_HIT_CONTEXT, *PPHYSX_VISIBILITY_TESTER_FIND_DISTANCE_TO_LIGHT_PROCESS_HIT_CONTEXT;

typedef struct _PHYSX_VISIBILITY_TESTER_TEST_ANY_DISTANCE_PROCESS_HIT_CONTEXT {
    BOOL Visible;
} PHYSX_VISIBILITY_TESTER_TEST_ANY_DISTANCE_PROCESS_HIT_CONTEXT, *PPHYSX_VISIBILITY_TESTER_TEST_ANY_DISTANCE_PROCESS_HIT_CONTEXT;

typedef struct _PHYSX_VISIBILITY_TESTER_TEST_PROCESS_HIT_CONTEXT {
    FLOAT Distance;
    BOOL Visible;
} PHYSX_VISIBILITY_TESTER_TEST_PROCESS_HIT_CONTEXT, *PPHYSX_VISIBILITY_TESTER_TEST_PROCESS_HIT_CONTEXT;

typedef struct _PHYSX_VISIBILITY_TESTER_TEST_GEOMETRY_CONTEXT {
    PPHYSX_RAYTRACER_TEST_GEOMETRY_ROUTINE Routine;
    PCVOID Context;
} PHYSX_VISIBILITY_TESTER_TEST_GEOMETRY_CONTEXT, *PPHYSX_VISIBILITY_TESTER_TEST_GEOMETRY_CONTEXT;

typedef CONST PHYSX_VISIBILITY_TESTER_TEST_GEOMETRY_CONTEXT *PCPHYSX_VISIBILITY_TESTER_TEST_GEOMETRY_CONTEXT;

struct _PHYSX_VISIBILITY_TESTER {
    PRAYTRACER RayTracer;
    PHYSX_VISIBILITY_TESTER_TEST_GEOMETRY_CONTEXT TestGeometryContext;
    FLOAT Epsilon;
};

//
// Static Test Geometry Functions
//

SFORCEINLINE
VOID
PhysxVisibilityTesterTestGeometryContextInitialize(
    _Out_ PPHYSX_VISIBILITY_TESTER_TEST_GEOMETRY_CONTEXT Context,
    _In_ PPHYSX_RAYTRACER_TEST_GEOMETRY_ROUTINE TestGeometryRoutine,
    _In_ PCVOID TestGeometryRoutineContext
    )
{
    ASSERT(Context != NULL);
    ASSERT(TestGeometryRoutine != NULL);
    ASSERT(TestGeometryRoutineContext != NULL);
    
    Context->Routine = TestGeometryRoutine;
    Context->Context = TestGeometryRoutineContext;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
STATIC
ISTATUS
PhysxVisibilityTesterTestGeometryCallback(
    _In_opt_ PCVOID Context, 
    _Inout_ PHIT_TESTER HitTester,
    _In_ RAY Ray
    )
{
    PCPHYSX_VISIBILITY_TESTER_TEST_GEOMETRY_CONTEXT TestGeometryContext;
    PHYSX_HIT_TESTER PhysxHitTester;
    ISTATUS Status;
    
    ASSERT(Context != NULL);
    ASSERT(HitTester != NULL);
    ASSERT(RayValidate(Ray) != FALSE);
    
    PhysxHitTesterInitialize(&PhysxHitTester,
                             HitTester);
    
    TestGeometryContext = (PCPHYSX_VISIBILITY_TESTER_TEST_GEOMETRY_CONTEXT) Context;
    
    Status = TestGeometryContext->Routine(TestGeometryContext->Context,
                                          &PhysxHitTester,
                                          Ray);

    return Status;
}

//
// Static Compute Pdf Functions
//

SFORCEINLINE
VOID
PhysxVisibilityTesterComputePdfProcessHitContextInitialize(
    _Out_ PPHYSX_VISIBILITY_TESTER_COMPUTE_PDF_PROCESS_HIT_CONTEXT Context,
    _In_ RAY ToLight,
    _In_ PCPHYSX_LIGHT Light,
    _In_ FLOAT InverseTotalSurfaceArea
    )
{
    ASSERT(Context != NULL);
    ASSERT(RayValidate(ToLight) != FALSE);
    ASSERT(Light != NULL);
    ASSERT(IsGreaterThanZeroFloat(InverseTotalSurfaceArea) != FALSE);
    ASSERT(IsFiniteFloat(InverseTotalSurfaceArea) != FALSE);

    Context->Light = Light;
    Context->ToLight = ToLight;
    Context->InverseTotalSurfaceArea = InverseTotalSurfaceArea;
    Context->FirstHit = TRUE;
    Context->Pdf = (FLOAT) 0.0f;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
STATIC
ISTATUS
PhysxVisibilityTesterComputePdfProcessHitCallback(
    _Inout_ PVOID Context,
    _In_ PCHIT Hit,
    _In_ PCMATRIX ModelToWorld,
    _In_ VECTOR3 ModelViewer,
    _In_ POINT3 ModelHitPoint,
    _In_ POINT3 WorldHitPoint
    )
{
    PCPHYSX_LIGHT BackLight;
    PCPHYSX_GEOMETRY Geometry;
    VECTOR3 ModelSurfaceNormal;
    VECTOR3 NormalizedWorldSurfaceNormal;
    PCPHYSX_LIGHT FrontLight;
    PCPHYSX_LIGHTED_GEOMETRY LightedGeometry;
    FLOAT Pdf;
    PPHYSX_VISIBILITY_TESTER_COMPUTE_PDF_PROCESS_HIT_CONTEXT ProcessHitContext;
    ISTATUS Status;
    FLOAT SurfaceArea;
    VECTOR3 WorldSurfaceNormal;
    VECTOR3 WorldToLight;

    ProcessHitContext = (PPHYSX_VISIBILITY_TESTER_COMPUTE_PDF_PROCESS_HIT_CONTEXT) Context;

    Geometry = (PCPHYSX_GEOMETRY) Hit->Data;
    
    PhysxGeometryGetLight(Geometry,
                          Hit->FrontFace,
                          &FrontLight);

    if (FrontLight != ProcessHitContext->Light)
    {
        if (ProcessHitContext->FirstHit != FALSE)
        {
            return ISTATUS_NO_MORE_DATA;
        }

        return ISTATUS_SUCCESS;
    }
    
    Status = PhysxGeometryComputeNormal(Geometry,
                                        Hit->FrontFace,
                                        ModelHitPoint,
                                        &ModelSurfaceNormal);

    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }

    PhysxLightedGeometryAdapterGetLightedGeometry(Geometry, &LightedGeometry);

    PhysxLightedGeometryComputeSurfaceArea(LightedGeometry,
                                           Hit->FrontFace,
                                           &SurfaceArea);

    WorldSurfaceNormal = VectorMatrixInverseTransposedMultiply(ModelToWorld,
                                                               ModelSurfaceNormal);

    NormalizedWorldSurfaceNormal = VectorNormalize(WorldSurfaceNormal, NULL, NULL);

    WorldToLight = PointSubtract(WorldHitPoint, ProcessHitContext->ToLight.Origin);

    Pdf = SurfaceArea *
          VectorDotProduct(WorldToLight, WorldToLight) /
          VectorDotProduct(ProcessHitContext->ToLight.Direction, NormalizedWorldSurfaceNormal);

    PhysxGeometryGetLight(Geometry,
                          Hit->BackFace,
                          &BackLight);
    
    if (BackLight == ProcessHitContext->Light)
    {
        Pdf *= (FLOAT) 2.0f;
    }
    
    ProcessHitContext->Pdf += Pdf * SurfaceArea * ProcessHitContext->InverseTotalSurfaceArea;

    if (ProcessHitContext->FirstHit != FALSE)
    {
        ProcessHitContext->ClosestWorldPointOnLight = WorldHitPoint;
        ProcessHitContext->FirstHit = FALSE;
    }

    return ISTATUS_SUCCESS;
}

//
// Static Find Distance To Light Functions
//

SFORCEINLINE
VOID
PhysxVisibilityTesterFindDistanceToLightProcessHitContextInitialize(
    _Out_ PPHYSX_VISIBILITY_TESTER_FIND_DISTANCE_TO_LIGHT_PROCESS_HIT_CONTEXT Context,
    _In_ PCPHYSX_LIGHT Light
    )
{
    ASSERT(Context != NULL);
    ASSERT(Light != NULL);

    Context->Light = Light;
    Context->DistanceToLight = (FLOAT) 0.0f;
    Context->Visible = FALSE;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
STATIC
ISTATUS
PhysxVisibilityTesterFindDistanceToLightProcessHitCallback(
    _Inout_ PVOID Context,
    _In_ PCHIT Hit
    )
{
    PPHYSX_VISIBILITY_TESTER_FIND_DISTANCE_TO_LIGHT_PROCESS_HIT_CONTEXT CallbackContext;
    PCPHYSX_GEOMETRY Geometry;
    PCPHYSX_LIGHT ClosestLight;
    
    ASSERT(Context != NULL);
    ASSERT(Hit != NULL);

    CallbackContext = (PPHYSX_VISIBILITY_TESTER_FIND_DISTANCE_TO_LIGHT_PROCESS_HIT_CONTEXT) Context;
    Geometry = (PCPHYSX_GEOMETRY) Hit->Data;

    PhysxGeometryGetLight(Geometry, Hit->FrontFace, &ClosestLight);
    
    if (ClosestLight == CallbackContext->Light)
    {
        CallbackContext->Visible = TRUE;
        CallbackContext->DistanceToLight = Hit->Distance;
        return ISTATUS_NO_MORE_DATA;
    }
    
    return ISTATUS_SUCCESS;
}

//
// Static Test Any Distance Functions
//

SFORCEINLINE
VOID
PhysxVisibilityTesterTestProcessHitContextInitialize(
    _Out_ PPHYSX_VISIBILITY_TESTER_TEST_PROCESS_HIT_CONTEXT Context,
    _In_ FLOAT Distance
    )
{
    ASSERT(Context != NULL);
    ASSERT(IsFiniteFloat(Distance) != FALSE);
    ASSERT(IsGreaterThanOrEqualToZeroFloat(Distance) != FALSE);

    Context->Distance = Distance;
    Context->Visible = TRUE;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
STATIC
ISTATUS
PhysxVisibilityTesterTestProcessHitCallback(
    _Inout_ PVOID Context,
    _In_ PCHIT Hit
    )
{
    PPHYSX_VISIBILITY_TESTER_TEST_PROCESS_HIT_CONTEXT CallbackContext;
    
    ASSERT(Context != NULL);
    ASSERT(Hit != NULL);
    
    CallbackContext = (PPHYSX_VISIBILITY_TESTER_TEST_PROCESS_HIT_CONTEXT) Context;

    if (Hit->Distance < CallbackContext->Distance)
    {
        CallbackContext->Visible = FALSE;
    }
    
    return ISTATUS_NO_MORE_DATA;
}

//
// Static Test Functions
//

SFORCEINLINE
VOID
PhysxVisibilityTesterTestAnyDistanceProcessHitContextInitialize(
    _Out_ PPHYSX_VISIBILITY_TESTER_TEST_ANY_DISTANCE_PROCESS_HIT_CONTEXT Context
    )
{
    ASSERT(Context != NULL);
    Context->Visible = TRUE;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
STATIC
ISTATUS
PhysxVisibilityTesterTestAnyDistanceProcessHitCallback(
    _Inout_ PVOID Context,
    _In_ PCHIT Hit
    )
{
    PPHYSX_VISIBILITY_TESTER_TEST_ANY_DISTANCE_PROCESS_HIT_CONTEXT CallbackContext;
    
    ASSERT(Context != NULL);
    ASSERT(Hit != NULL);

    CallbackContext = (PPHYSX_VISIBILITY_TESTER_TEST_ANY_DISTANCE_PROCESS_HIT_CONTEXT) Context;
    
    CallbackContext->Visible = FALSE;
    
    return ISTATUS_NO_MORE_DATA;
}

//
// Internal Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
PhysxVisibilityTesterAllocate(
    _Out_ PPHYSX_VISIBILITY_TESTER *VisibilityTester
    )
{
    PVOID Allocation;
    PPHYSX_VISIBILITY_TESTER AllocatedVisibilityTester;
    PRAYTRACER RayTracer;
    ISTATUS Status;

    ASSERT(VisibilityTester != NULL);

    Allocation = malloc(sizeof(PHYSX_VISIBILITY_TESTER));
    AllocatedVisibilityTester = (PPHYSX_VISIBILITY_TESTER) Allocation;

    if (AllocatedVisibilityTester == NULL)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    Status = RayTracerAllocate(&RayTracer);

    if (Status != ISTATUS_SUCCESS)
    {
        free(AllocatedVisibilityTester);
        return Status;
    }

    AllocatedVisibilityTester->RayTracer = RayTracer;

    *VisibilityTester = AllocatedVisibilityTester;

    return ISTATUS_SUCCESS;
}

VOID
PhysxVisibilityTesterSetSceneAndEpsilon(
    _Inout_ PPHYSX_VISIBILITY_TESTER VisibilityTester,
    _In_ PPHYSX_RAYTRACER_TEST_GEOMETRY_ROUTINE TestGeometryRoutine,
    _In_ PCVOID TestGeometryRoutineContext,
    _In_ FLOAT Epsilon
    )
{
    ASSERT(VisibilityTester != NULL);
    ASSERT(TestGeometryRoutine != NULL);
    ASSERT(TestGeometryRoutineContext != NULL);
    ASSERT(IsFiniteFloat(Epsilon) != FALSE);
    ASSERT(IsGreaterThanZeroFloat(Epsilon) != FALSE);
    
    PhysxVisibilityTesterTestGeometryContextInitialize(&VisibilityTester->TestGeometryContext,
                                                       TestGeometryRoutine,
                                                       TestGeometryRoutineContext);
    
    VisibilityTester->Epsilon = Epsilon;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
PhysxVisibilityTesterComputePdf(
    _In_ PPHYSX_VISIBILITY_TESTER VisibilityTester,
    _In_ RAY WorldRay,
    _In_ PCPHYSX_LIGHT Light,
    _In_ FLOAT InverseLightSurfaceArea,
    _Out_ PPOINT3 ClosestPointOnLight,
    _Out_ PFLOAT Pdf
    )
{
    PHYSX_VISIBILITY_TESTER_COMPUTE_PDF_PROCESS_HIT_CONTEXT Context;
    ISTATUS Status;

    ASSERT(VisibilityTester != NULL);
    ASSERT(RayValidate(WorldRay) != FALSE);
    ASSERT(Light != NULL);
    ASSERT(IsFiniteFloat(InverseLightSurfaceArea) != FALSE);
    ASSERT(IsGreaterThanZeroFloat(InverseLightSurfaceArea) != FALSE); 
    ASSERT(Pdf != NULL);

    PhysxVisibilityTesterComputePdfProcessHitContextInitialize(&Context,
                                                               WorldRay,
                                                               Light,
                                                               InverseLightSurfaceArea);
    
    Status = RayTracerTraceSceneProcessHitsInOrderWithCoordinates(VisibilityTester->RayTracer,
                                                                  WorldRay,
                                                                  VisibilityTester->Epsilon,
                                                                  PhysxVisibilityTesterTestGeometryCallback,
                                                                  &VisibilityTester->TestGeometryContext,
                                                                  PhysxVisibilityTesterComputePdfProcessHitCallback,
                                                                  &Context);

    *ClosestPointOnLight = Context.ClosestWorldPointOnLight;
    *Pdf = Context.Pdf;

    return Status;
}

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

    PhysxVisibilityTesterFindDistanceToLightProcessHitContextInitialize(&ProcessHitContext,
                                                                        Light);
    
    Status = RayTracerTraceSceneProcessClosestHit(VisibilityTester->RayTracer,
                                                  WorldRay,
                                                  VisibilityTester->Epsilon,
                                                  PhysxVisibilityTesterTestGeometryCallback,
                                                  &VisibilityTester->TestGeometryContext,
                                                  PhysxVisibilityTesterFindDistanceToLightProcessHitCallback,
                                                  &ProcessHitContext);
    
    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }
    
    *Visible = ProcessHitContext.Visible;
    *DistanceToLight = ProcessHitContext.DistanceToLight;
    
    return ISTATUS_SUCCESS;
}

VOID
PhysxVisibilityTesterFree(
    _In_opt_ _Post_invalid_ PPHYSX_VISIBILITY_TESTER VisibilityTester
    )
{
    ASSERT(VisibilityTester != NULL);

    RayTracerFree(VisibilityTester->RayTracer);
    free(VisibilityTester);
}

//
// Public Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
PhysxVisibilityTesterTest(
    _In_ PPHYSX_VISIBILITY_TESTER VisibilityTester,
    _In_ RAY WorldRay,
    _In_ FLOAT DistanceToObject,
    _Out_ PBOOL Visible
    )
{
    PHYSX_VISIBILITY_TESTER_TEST_PROCESS_HIT_CONTEXT Context;
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

    if (VisibilityTester->Epsilon * (FLOAT) 2.0f > DistanceToObject)
    {
        *Visible = TRUE;
        return ISTATUS_SUCCESS;
    }

    DistanceToObject = DistanceToObject - VisibilityTester->Epsilon;

    PhysxVisibilityTesterTestProcessHitContextInitialize(&Context,
                                                         DistanceToObject);
    
    Status = RayTracerTraceSceneProcessClosestHit(VisibilityTester->RayTracer,
                                                  WorldRay,
                                                  VisibilityTester->Epsilon,
                                                  PhysxVisibilityTesterTestGeometryCallback,
                                                  &VisibilityTester->TestGeometryContext,
                                                  PhysxVisibilityTesterTestProcessHitCallback,
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
PhysxVisibilityTesterTestAnyDistance(
    _In_ PPHYSX_VISIBILITY_TESTER VisibilityTester,
    _In_ RAY WorldRay,
    _Out_ PBOOL Visible
    )
{
    PHYSX_VISIBILITY_TESTER_TEST_ANY_DISTANCE_PROCESS_HIT_CONTEXT Context;
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

    PhysxVisibilityTesterTestAnyDistanceProcessHitContextInitialize(&Context);
    
    Status = RayTracerTraceSceneProcessClosestHit(VisibilityTester->RayTracer,
                                                  WorldRay,
                                                  VisibilityTester->Epsilon,
                                                  PhysxVisibilityTesterTestGeometryCallback,
                                                  &VisibilityTester->TestGeometryContext,
                                                  PhysxVisibilityTesterTestAnyDistanceProcessHitCallback,
                                                  &Context);

    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }

    *Visible = Context.Visible;
    return ISTATUS_SUCCESS;
}
