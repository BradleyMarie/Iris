/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    irisadvanced_visibilitytester.h

Abstract:

    This file contains the definitions for the VISIBILITY_TESTER type.

--*/

#ifndef _VISIBILITY_TESTER_IRIS_ADVANCED_
#define _VISIBILITY_TESTER_IRIS_ADVANCED_

#include <irisadvanced.h>

//
// Types
//

typedef struct _VISIBILITY_TESTER VISIBILITY_TESTER, *PVISIBILITY_TESTER;
typedef CONST VISIBILITY_TESTER *PCVISIBILITY_TESTER;

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISADVANCEDAPI
ISTATUS
VisibilityTesterAllocate(
    _In_ PSCENE Scene,
    _In_ FLOAT Epsilon,
    _Out_ PVISIBILITY_TESTER *VisibilityTester
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISADVANCEDAPI
ISTATUS
VisibilityTesterTestVisibility(
    _In_ PVISIBILITY_TESTER Tester,
    _In_ RAY WorldRay,
    _In_ FLOAT DistanceToObject,
    _Out_ PBOOL Visible
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISADVANCEDAPI
ISTATUS
VisibilityTesterTestVisibilityAnyDistance(
    _In_ PVISIBILITY_TESTER Tester,
    _In_ RAY WorldRay,
    _Out_ PBOOL Visible
    );

IRISADVANCEDAPI
VOID
VisibilityTesterFree(
    _In_opt_ _Post_invalid_ PVISIBILITY_TESTER Tester
    );

#endif // _RANDOM_IRIS_ADVANCED_