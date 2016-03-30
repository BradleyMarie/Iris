/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    irisphysx_visibilitytester.h

Abstract:

    This file contains the internal definitions for the 
    PBR_VISIBILITY_TESTER type.

--*/

#ifndef _PBR_VISIBILITY_TESTER_IRIS_PHYSX_INTERNAL_
#define _PBR_VISIBILITY_TESTER_IRIS_PHYSX_INTERNAL_

#include <irisphysxp.h>

//
// Types
//

struct _PBR_VISIBILITY_TESTER {
    PRAYTRACER RayTracer;
    PPBR_INTEGRATOR_TEST_GEOMETRY_ROUTINE TestGeometryRoutine;
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
PBRVisibilityTesterInitialize(
    _Out_ PPBR_VISIBILITY_TESTER PBRVisibilityTester
    )
{
    PRAYTRACER RayTracer;
    ISTATUS Status;

    ASSERT(PBRVisibilityTester != NULL);

    Status = RayTracerAllocate(&RayTracer);

    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }

    PBRVisibilityTester->RayTracer = RayTracer;
    PBRVisibilityTester->TestGeometryRoutine = NULL;
    PBRVisibilityTester->TestGeometryRoutineContext = NULL;
    PBRVisibilityTester->Epsilon = (FLOAT) 0.0;

    return ISTATUS_SUCCESS;
}

SFORCEINLINE
VOID
PBRVisibilityTesterSetSceneAndEpsilon(
    _Inout_ PPBR_VISIBILITY_TESTER PBRVisibilityTester,
    _In_ PPBR_INTEGRATOR_TEST_GEOMETRY_ROUTINE TestGeometryRoutine,
    _In_ PCVOID TestGeometryRoutineContext,
    _In_ FLOAT Epsilon
    )
{
    ASSERT(PBRVisibilityTester != NULL);
    ASSERT(TestGeometryRoutine != NULL);
    ASSERT(TestGeometryRoutineContext != NULL);
    ASSERT(IsFiniteFloat(Epsilon) != FALSE);
    ASSERT(IsGreaterThanZeroFloat(Epsilon) != FALSE);
    
    PBRVisibilityTester->TestGeometryRoutine = TestGeometryRoutine;
    PBRVisibilityTester->TestGeometryRoutineContext = TestGeometryRoutineContext;
    PBRVisibilityTester->Epsilon = Epsilon;
}

SFORCEINLINE
VOID
PBRVisibilityTesterDestroy(
    _In_opt_ _Post_invalid_ PPBR_VISIBILITY_TESTER PBRVisibilityTester
    )
{
    ASSERT(PBRVisibilityTester != NULL);

    RayTracerFree(PBRVisibilityTester->RayTracer);
}

#endif // _PBR_VISIBILITY_TESTER_IRIS_PHYSX_INTERNAL_