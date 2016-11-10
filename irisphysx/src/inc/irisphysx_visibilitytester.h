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
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
PhysxVisibilityTesterAllocate(
    _Out_ PPHYSX_VISIBILITY_TESTER *VisibilityTester
    );

VOID
PhysxVisibilityTesterSetSceneAndEpsilon(
    _Inout_ PPHYSX_VISIBILITY_TESTER VisibilityTester,
    _In_ PPHYSX_INTEGRATOR_TEST_GEOMETRY_ROUTINE TestGeometryRoutine,
    _In_ PCVOID TestGeometryRoutineContext,
    _In_ FLOAT Epsilon
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
PhysxVisibilityTesterComputePdf(
    _In_ PPHYSX_VISIBILITY_TESTER VisibilityTester,
    _In_ RAY WorldRay,
    _In_ PCPHYSX_LIGHT Light,
    _In_ FLOAT InverseLightSurfaceArea,
    _In_ PPOINT3 ClosestPointOnLight,
    _Out_ PFLOAT Pdf
    );

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

VOID
PhysxVisibilityTesterFree(
    _In_opt_ _Post_invalid_ PPHYSX_VISIBILITY_TESTER VisibilityTester
    );

#endif // _PHYSX_VISIBILITY_TESTER_IRIS_PHYSX_INTERNAL_
