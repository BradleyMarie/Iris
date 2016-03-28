/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    irisphysx_hitallocator.h

Abstract:

    This file contains the definitions for the PBR_HIT_ALLOCATOR type.

--*/

#ifndef _PBR_HIT_ALLOCATOR_IRIS_PHYSX_INTERNAL_
#define _PBR_HIT_ALLOCATOR_IRIS_PHYSX_INTERNAL_

#include <irisphysxp.h>

//
// Types
//

struct _PBR_HIT_TESTER {
    PHIT_TESTER Tester;
};

//
// Functions
//

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

#endif // _PBR_HIT_ALLOCATOR_IRIS_PHYSX_INTERNAL_