/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    irisphysx_hitallocator.h

Abstract:

    This file contains the definitions for the PHYSX_HIT_TESTER type.

--*/

#ifndef _PHYSX_HIT_TESTER_IRIS_PHYSX_INTERNAL_
#define _PHYSX_HIT_TESTER_IRIS_PHYSX_INTERNAL_

#include <irisphysxp.h>

//
// Types
//

struct _PHYSX_HIT_TESTER {
    PHIT_TESTER Tester;
};

//
// Function
//

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

#endif // _PHYSX_HIT_TESTER_IRIS_PHYSX_INTERNAL_