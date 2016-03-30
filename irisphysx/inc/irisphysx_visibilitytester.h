/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    irisphysx_visibilitytester.h

Abstract:

    This file contains the definitions for the PBR_VISIBILITY_TESTER type.

--*/

#ifndef _PBR_VISIBILITY_TESTER_IRIS_PHYSX_
#define _PBR_VISIBILITY_TESTER_IRIS_PHYSX_

#include <irisphysx.h>

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISPHYSXAPI
ISTATUS
PBRVisibilityTesterTestVisibility(
    _In_ PPBR_VISIBILITY_TESTER PBRVisibilityTester,
    _In_ RAY WorldRay,
    _In_ FLOAT DistanceToObject,
    _Out_ PBOOL Visible
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISPHYSXAPI
ISTATUS
PBRVisibilityTesterTestVisibilityAnyDistance(
    _In_ PPBR_VISIBILITY_TESTER PBRVisibilityTester,
    _In_ RAY WorldRay,
    _Out_ PBOOL Visible
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISPHYSXAPI
ISTATUS
PBRVisibilityTesterTestLightVisibility(
    _In_ PPBR_VISIBILITY_TESTER PBRVisibilityTester,
    _In_ RAY WorldRay,
    _In_ PCLIGHT Light,
    _Out_ PBOOL Visible
    );

#endif // _PBR_VISIBILITY_TESTER_IRIS_PHYSX_