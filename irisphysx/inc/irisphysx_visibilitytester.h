/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    irisphysx_visibilitytester.h

Abstract:

    This file contains the definitions for the PPHYSX_VISIBILITY_TESTER type.

--*/

#ifndef _PHYSX_VISIBILITY_TESTER_IRIS_PHYSX_
#define _PHYSX_VISIBILITY_TESTER_IRIS_PHYSX_

#include <irisphysx.h>

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISPHYSXAPI
ISTATUS
PhysxVisibilityTesterTestVisibility(
    _In_ PPHYSX_VISIBILITY_TESTER VisibilityTester,
    _In_ RAY WorldRay,
    _In_ FLOAT DistanceToObject,
    _Out_ PBOOL Visible
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISPHYSXAPI
ISTATUS
PhysxVisibilityTesterTestVisibilityAnyDistance(
    _In_ PPHYSX_VISIBILITY_TESTER VisibilityTester,
    _In_ RAY WorldRay,
    _Out_ PBOOL Visible
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISPHYSXAPI
ISTATUS
PhysxVisibilityTesterTestLightVisibility(
    _In_ PPHYSX_VISIBILITY_TESTER VisibilityTester,
    _In_ RAY WorldRay,
    _In_ PCPHYSX_LIGHT Light,
    _Out_ PBOOL Visible
    );

#endif // _PHYSX_VISIBILITY_TESTER_IRIS_PHYSX_
