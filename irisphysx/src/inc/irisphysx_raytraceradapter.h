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

typedef struct _PHYSX_HIT_TESTER_ADAPTER_CONTEXT {
    PPHYSX_INTEGRATOR_TEST_GEOMETRY_ROUTINE TestGeometryRoutine;
    PCVOID TestGeometryRoutineContext;
} PHYSX_HIT_TESTER_ADAPTER_CONTEXT, *PPHYSX_HIT_TESTER_ADAPTER_CONTEXT;

typedef CONST PHYSX_HIT_TESTER_ADAPTER_CONTEXT *PCPHYSX_HIT_TESTER_ADAPTER_CONTEXT;

//
// Prototypes
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
PhysxHitTesterAdapter(
    _In_opt_ PCVOID Context, 
    _Inout_ PHIT_TESTER HitTester,
    _In_ RAY Ray
    );

//
// Functions
//

SFORCEINLINE
VOID
PhysxHitTesterAdapterContextInitialize(
    _Out_ PPHYSX_HIT_TESTER_ADAPTER_CONTEXT HitTesterAdapterContext,
    _In_ PPHYSX_INTEGRATOR_TEST_GEOMETRY_ROUTINE TestGeometryRoutine,
    _In_ PCVOID TestGeometryRoutineContext
    )
{
    ASSERT(HitTesterAdapterContext != NULL);
    ASSERT(TestGeometryRoutine != NULL);
    ASSERT(TestGeometryRoutineContext != NULL);
    
    HitTesterAdapterContext->TestGeometryRoutine = TestGeometryRoutine;
    HitTesterAdapterContext->TestGeometryRoutineContext = TestGeometryRoutineContext;
}

SFORCEINLINE
VOID
PhysxHitTesterInitialize(
    _Out_ PPHYSX_HIT_TESTER HitTester,
    _In_ PHIT_TESTER Tester
    )
{
    ASSERT(HitTester != NULL);
    ASSERT(Tester != NULL);
    
    HitTester->Tester = Tester;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS
RayTracerAdapterTraceSceneProcessClosestHit(
    _Inout_ PRAYTRACER RayTracer,
    _In_ RAY Ray,
    _In_ FLOAT MinimumDistance,
    _In_ PPHYSX_INTEGRATOR_TEST_GEOMETRY_ROUTINE TestGeometryRoutine,
    _In_opt_ PCVOID TestGeometryContext,
    _In_ PRAYTRACER_PROCESS_HIT_ROUTINE ProcessHitRoutine,
    _Inout_opt_ PVOID ProcessHitContext
    )
{
    PHYSX_HIT_TESTER_ADAPTER_CONTEXT HitTesterAdapterContext;
    ISTATUS Status;
    
    ASSERT(RayTracer != NULL);

    if (TestGeometryRoutine == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }
    
    if (ProcessHitRoutine == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_05;
    }

    PhysxHitTesterAdapterContextInitialize(&HitTesterAdapterContext,
                                           TestGeometryRoutine,
                                           TestGeometryContext);

    Status = RayTracerTraceSceneProcessClosestHit(RayTracer,
                                                  Ray,
                                                  MinimumDistance,
                                                  PhysxHitTesterAdapter,
                                                  &HitTesterAdapterContext,
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
    _In_ RAY Ray,
    _In_ FLOAT MinimumDistance,
    _In_ PPHYSX_INTEGRATOR_TEST_GEOMETRY_ROUTINE TestGeometryRoutine,
    _In_opt_ PCVOID TestGeometryContext,
    _In_ PRAYTRACER_PROCESS_HIT_WITH_COORDINATES_ROUTINE ProcessHitRoutine,
    _Inout_opt_ PVOID ProcessHitContext
    )
{
    PHYSX_HIT_TESTER_ADAPTER_CONTEXT HitTesterAdapterContext;
    ISTATUS Status;
    
    ASSERT(RayTracer != NULL);

    if (TestGeometryRoutine == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    if (ProcessHitRoutine == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_05;
    }
    
    PhysxHitTesterAdapterContextInitialize(&HitTesterAdapterContext,
                                           TestGeometryRoutine,
                                           TestGeometryContext);

    Status = RayTracerTraceSceneProcessClosestHitWithCoordinates(RayTracer,
                                                                 Ray,
                                                                 MinimumDistance,
                                                                 PhysxHitTesterAdapter,
                                                                 &HitTesterAdapterContext,
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
    _In_ RAY Ray,
    _In_ PPHYSX_INTEGRATOR_TEST_GEOMETRY_ROUTINE TestGeometryRoutine,
    _In_opt_ PCVOID TestGeometryContext,
    _In_ PRAYTRACER_PROCESS_HIT_ROUTINE ProcessHitRoutine,
    _Inout_opt_ PVOID ProcessHitContext
    )
{
    PHYSX_HIT_TESTER_ADAPTER_CONTEXT HitTesterAdapterContext;
    ISTATUS Status;
    
    ASSERT(RayTracer != NULL);

    if (TestGeometryRoutine == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (ProcessHitRoutine == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }
    
    PhysxHitTesterAdapterContextInitialize(&HitTesterAdapterContext,
                                           TestGeometryRoutine,
                                           TestGeometryContext);

    Status = RayTracerTraceSceneProcessAllHitsOutOfOrder(RayTracer,
                                                         Ray,
                                                         PhysxHitTesterAdapter,
                                                         &HitTesterAdapterContext,
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
    _In_ RAY Ray,
    _In_ PPHYSX_INTEGRATOR_TEST_GEOMETRY_ROUTINE TestGeometryRoutine,
    _In_opt_ PCVOID TestGeometryContext,
    _In_ PRAYTRACER_PROCESS_HIT_WITH_COORDINATES_ROUTINE ProcessHitRoutine,
    _Inout_opt_ PVOID ProcessHitContext
    )
{
    PHYSX_HIT_TESTER_ADAPTER_CONTEXT HitTesterAdapterContext;
    ISTATUS Status;
    
    ASSERT(RayTracer != NULL);

    if (TestGeometryRoutine == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (ProcessHitRoutine == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }
    
    PhysxHitTesterAdapterContextInitialize(&HitTesterAdapterContext,
                                           TestGeometryRoutine,
                                           TestGeometryContext);

    Status = RayTracerTraceSceneProcessAllHitsInOrderWithCoordinates(RayTracer,
                                                                     Ray,
                                                                     PhysxHitTesterAdapter,
                                                                     &HitTesterAdapterContext,
                                                                     ProcessHitRoutine,
                                                                     ProcessHitContext);

    return Status;
}

#endif // _RAYTRACER_ADAPTER_IRIS_PHYSX_INTERNAL_
