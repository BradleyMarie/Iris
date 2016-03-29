/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    irisphysx_raytraceradapter.h

Abstract:

    This file contains the prototypes for the RayTracer adapter functions.

--*/

#ifndef _RAYTRACER_ADAPTER_IRIS_PHYSX_INTERNAL_
#define _RAYTRACER_ADAPTER_IRIS_PHYSX_INTERNAL_

#include <irisphysxp.h>

//
// Types
//

typedef struct _PBR_HIT_TESTER_ADAPTER_CONTEXT {
    PPBR_INTEGRATOR_TEST_GEOMETRY_ROUTINE TestGeometryRoutine;
    PCVOID TestGeometryRoutineContext;
} PBR_HIT_TESTER_ADAPTER_CONTEXT, *PPBR_HIT_TESTER_ADAPTER_CONTEXT;

typedef CONST PBR_HIT_TESTER_ADAPTER_CONTEXT *PCPBR_HIT_TESTER_ADAPTER_CONTEXT;

//
// Prototypes
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
PBRHitTesterAdapter(
    _In_opt_ PCVOID Context, 
    _Inout_ PHIT_TESTER HitTester,
    _In_ RAY Ray
    );

//
// Functions
//

SFORCEINLINE
VOID
PBRHitTesterAdapterContextInitialize(
    _Out_ PPBR_HIT_TESTER_ADAPTER_CONTEXT PBRHitTesterAdapterContext,
    _In_ PPBR_INTEGRATOR_TEST_GEOMETRY_ROUTINE TestGeometryRoutine,
    _In_ PCVOID TestGeometryRoutineContext
    )
{
    ASSERT(PBRHitTesterAdapterContext != NULL);
    ASSERT(TestGeometryRoutine != NULL);
    ASSERT(TestGeometryRoutineContext != NULL);
    
    PBRHitTesterAdapterContext->TestGeometryRoutine = TestGeometryRoutine;
    PBRHitTesterAdapterContext->TestGeometryRoutineContext = TestGeometryRoutineContext;
}

SFORCEINLINE
VOID
PBRHitTesterInitialize(
    _Out_ PPBR_HIT_TESTER PBRHitTester,
    _In_ PHIT_TESTER Tester
    )
{
    ASSERT(PBRHitTester != NULL);
    ASSERT(Tester != NULL);
    
    PBRHitTester->Tester = Tester;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS
RayTracerAdapterTraceSceneProcessClosestHit(
    _Inout_ PRAYTRACER RayTracer,
    _In_ PPBR_INTEGRATOR_TEST_GEOMETRY_ROUTINE TestGeometryRoutine,
    _In_opt_ PCVOID TestGeometryContext,
    _In_ RAY Ray,
    _In_ FLOAT MinimumDistance,
    _In_ PRAYTRACER_PROCESS_HIT_ROUTINE ProcessHitRoutine,
    _Inout_opt_ PVOID ProcessHitContext
    )
{
    PBR_HIT_TESTER_ADAPTER_CONTEXT PBRHitTesterAdapterContext;
    ISTATUS Status;
    
    ASSERT(RayTracer != NULL);
    ASSERT(TestGeometryRoutine != NULL);
    ASSERT(RayValidate(Ray) != FALSE);
    ASSERT(IsGreaterThanOrEqualToZeroFloat(MinimumDistance) != FALSE);
    ASSERT(ProcessHitRoutine != NULL);
    
    PBRHitTesterAdapterContextInitialize(&PBRHitTesterAdapterContext,
                                         TestGeometryRoutine,
                                         TestGeometryContext);

    Status = RayTracerTraceSceneProcessClosestHit(RayTracer,
                                                  PBRHitTesterAdapter,
                                                  &PBRHitTesterAdapterContext,
                                                  Ray,
                                                  MinimumDistance,
                                                  ProcessHitRoutine,
                                                  ProcessHitContext);

    return Status;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS
RayTracerAdapterTraceSceneProcessClosestHitWithCoordinates(
    _Inout_ PRAYTRACER RayTracer,
    _In_ PPBR_INTEGRATOR_TEST_GEOMETRY_ROUTINE TestGeometryRoutine,
    _In_opt_ PCVOID TestGeometryContext,
    _In_ RAY Ray,
    _In_ FLOAT MinimumDistance,
    _In_ PRAYTRACER_PROCESS_HIT_WITH_COORDINATES_ROUTINE ProcessHitRoutine,
    _Inout_opt_ PVOID ProcessHitContext
    )
{
    PBR_HIT_TESTER_ADAPTER_CONTEXT PBRHitTesterAdapterContext;
    ISTATUS Status;
    
    ASSERT(RayTracer != NULL);
    ASSERT(TestGeometryRoutine != NULL);
    ASSERT(RayValidate(Ray) != FALSE);
    ASSERT(IsGreaterThanOrEqualToZeroFloat(MinimumDistance) != FALSE);
    ASSERT(ProcessHitRoutine != NULL);
    
    PBRHitTesterAdapterContextInitialize(&PBRHitTesterAdapterContext,
                                         TestGeometryRoutine,
                                         TestGeometryContext);

    Status = RayTracerTraceSceneProcessClosestHitWithCoordinates(RayTracer,
                                                                 PBRHitTesterAdapter,
                                                                 &PBRHitTesterAdapterContext,
                                                                 Ray,
                                                                 MinimumDistance,
                                                                 ProcessHitRoutine,
                                                                 ProcessHitContext);

    return Status;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS
RayTracerAdapterTraceSceneProcessAllHitsOutOfOrder(
    _Inout_ PRAYTRACER RayTracer,
    _In_ PPBR_INTEGRATOR_TEST_GEOMETRY_ROUTINE TestGeometryRoutine,
    _In_opt_ PCVOID TestGeometryContext,
    _In_ RAY Ray,
    _In_ PRAYTRACER_PROCESS_HIT_ROUTINE ProcessHitRoutine,
    _Inout_opt_ PVOID ProcessHitContext
    )
{
    PBR_HIT_TESTER_ADAPTER_CONTEXT PBRHitTesterAdapterContext;
    ISTATUS Status;
    
    ASSERT(RayTracer != NULL);
    ASSERT(TestGeometryRoutine != NULL);
    ASSERT(RayValidate(Ray) != FALSE);
    ASSERT(ProcessHitRoutine != NULL);
    
    PBRHitTesterAdapterContextInitialize(&PBRHitTesterAdapterContext,
                                         TestGeometryRoutine,
                                         TestGeometryContext);

    Status = RayTracerTraceSceneProcessAllHitsOutOfOrder(RayTracer,
                                                         PBRHitTesterAdapter,
                                                         &PBRHitTesterAdapterContext,
                                                         Ray,
                                                         ProcessHitRoutine,
                                                         ProcessHitContext);

    return Status;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS
RayTracerAdapterTraceSceneProcessAllHitsInOrderWithCoordinates(
    _Inout_ PRAYTRACER RayTracer,
    _In_ PPBR_INTEGRATOR_TEST_GEOMETRY_ROUTINE TestGeometryRoutine,
    _In_opt_ PCVOID TestGeometryContext,
    _In_ RAY Ray,
    _In_ PRAYTRACER_PROCESS_HIT_WITH_COORDINATES_ROUTINE ProcessHitRoutine,
    _Inout_opt_ PVOID ProcessHitContext
    )
{
    PBR_HIT_TESTER_ADAPTER_CONTEXT PBRHitTesterAdapterContext;
    ISTATUS Status;
    
    ASSERT(RayTracer != NULL);
    ASSERT(TestGeometryRoutine != NULL);
    ASSERT(RayValidate(Ray) != FALSE);
    ASSERT(ProcessHitRoutine != NULL);
    
    PBRHitTesterAdapterContextInitialize(&PBRHitTesterAdapterContext,
                                         TestGeometryRoutine,
                                         TestGeometryContext);

    Status = RayTracerTraceSceneProcessAllHitsInOrderWithCoordinates(RayTracer,
                                                                     PBRHitTesterAdapter,
                                                                     &PBRHitTesterAdapterContext,
                                                                     Ray,
                                                                     ProcessHitRoutine,
                                                                     ProcessHitContext);

    return Status;
}

#endif // _RAYTRACER_ADAPTER_IRIS_PHYSX_INTERNAL_