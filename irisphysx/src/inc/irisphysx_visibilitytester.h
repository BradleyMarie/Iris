/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    irisphysx_visibilitytester.h

Abstract:

    This file contains the internal definitions for the 
    PHYSX_VISIBILITY_TESTER type.

--*/

#ifndef _PHYSX_VISIBILITY_TESTER_IRIS_PHYSX_INTERNAL_
#define _PHYSX_VISIBILITY_TESTER_IRIS_PHYSX_INTERNAL_

#include <irisphysxp.h>

//
// Types
//

struct _PHYSX_VISIBILITY_TESTER {
    PRAYTRACER RayTracer;
    PPHYSX_INTEGRATOR_TEST_GEOMETRY_ROUTINE TestGeometryRoutine;
    PCVOID TestGeometryRoutineContext;
    FLOAT Epsilon;
};

//
// PBRVisibilityTester Static Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS
PhysxVisibilityTesterInitialize(
    _Out_ PPHYSX_VISIBILITY_TESTER VisibilityTester
    )
{
    PRAYTRACER RayTracer;
    ISTATUS Status;

    ASSERT(VisibilityTester != NULL);

    Status = RayTracerAllocate(&RayTracer);

    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }

    VisibilityTester->RayTracer = RayTracer;
    VisibilityTester->TestGeometryRoutine = NULL;
    VisibilityTester->TestGeometryRoutineContext = NULL;
    VisibilityTester->Epsilon = (FLOAT) 0.0;

    return ISTATUS_SUCCESS;
}

SFORCEINLINE
VOID
PhysxVisibilityTesterSetSceneAndEpsilon(
    _Inout_ PPHYSX_VISIBILITY_TESTER VisibilityTester,
    _In_ PPHYSX_INTEGRATOR_TEST_GEOMETRY_ROUTINE TestGeometryRoutine,
    _In_ PCVOID TestGeometryRoutineContext,
    _In_ FLOAT Epsilon
    )
{
    ASSERT(VisibilityTester != NULL);
    ASSERT(TestGeometryRoutine != NULL);
    ASSERT(TestGeometryRoutineContext != NULL);
    ASSERT(IsFiniteFloat(Epsilon) != FALSE);
    ASSERT(IsGreaterThanZeroFloat(Epsilon) != FALSE);
    
    VisibilityTester->TestGeometryRoutine = TestGeometryRoutine;
    VisibilityTester->TestGeometryRoutineContext = TestGeometryRoutineContext;
    VisibilityTester->Epsilon = Epsilon;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS
PhysxVisibilityTesterTestCustom(
    _In_ PPHYSX_VISIBILITY_TESTER VisibilityTester,
    _In_ RAY WorldRay,
    _In_ PRAYTRACER_PROCESS_HIT_WITH_COORDINATES_ROUTINE ProcessHitRoutine,
    _Inout_opt_ PVOID ProcessHitContext
    )
{
    ISTATUS Status;

    ASSERT(VisibilityTester != NULL);
    ASSERT(RayValidate(WorldRay) != FALSE);
    ASSERT(ProcessHitRoutine != NULL);

    Status = RayTracerAdapterTraceSceneProcessAllHitsInOrderWithCoordinates(VisibilityTester->RayTracer,
                                                                            WorldRay,
                                                                            VisibilityTester->TestGeometryRoutine,
                                                                            VisibilityTester->TestGeometryRoutineContext,
                                                                            ProcessHitRoutine,
                                                                            ProcessHitContext);

    return Status;
}

SFORCEINLINE
VOID
PhysxVisibilityTesterDestroy(
    _In_opt_ _Post_invalid_ PPHYSX_VISIBILITY_TESTER VisibilityTester
    )
{
    ASSERT(VisibilityTester != NULL);

    RayTracerFree(VisibilityTester->RayTracer);
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
    );

#endif // _PHYSX_VISIBILITY_TESTER_IRIS_PHYSX_INTERNAL_
