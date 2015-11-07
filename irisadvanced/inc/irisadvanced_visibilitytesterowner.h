/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    irisadvanced_visibilitytesterowner.h

Abstract:

    This file contains the definitions for the VISIBILITY_TESTER_OWNER type.

--*/

#ifndef _VISIBILITY_TESTER_OWNER_IRIS_ADVANCED_
#define _VISIBILITY_TESTER_OWNER_IRIS_ADVANCED_

#include <irisadvanced.h>

//
// Types
//

typedef struct _VISIBILITY_TESTER_OWNER VISIBILITY_TESTER_OWNER, *PVISIBILITY_TESTER_OWNER;
typedef CONST VISIBILITY_TESTER_OWNER *PCVISIBILITY_TESTER_OWNER;

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISADVANCEDAPI
ISTATUS
VisibilityTesterOwnerAllocate(
    _Out_ PVISIBILITY_TESTER_OWNER *VisibilityTesterOwner
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISADVANCEDAPI
ISTATUS
VisibilityTesterOwnerGetVisibilityTester(
    _In_ PVISIBILITY_TESTER_OWNER VisibilityTesterOwner,
    _In_ PCSCENE Scene,
    _In_ FLOAT Epsilon,
    _Out_ PVISIBILITY_TESTER *VisibilityTester
    );

IRISADVANCEDAPI
VOID
VisibilityTesterOwnerFree(
    _In_opt_ _Post_invalid_ PVISIBILITY_TESTER_OWNER Tester
    );

#endif // _VISIBILITY_TESTER_OWNER_IRIS_ADVANCED_