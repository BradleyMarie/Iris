/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    irisadvanced_randomallocator.h

Abstract:

    Internal definitions and declarations for RANDOM_ALLOCATOR type.

--*/

#ifndef _IRIS_ADVANCED_RANDOM_ALLOCATOR_INTERNAL_
#define _IRIS_ADVANCED_RANDOM_ALLOCATOR_INTERNAL_

#include <irisadvancedp.h>

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
RandomAllocatorAllocate(
    _Out_ PRANDOM_ALLOCATOR *RandomAllocator
    );

VOID
RandomAllocatorClear(
    _Inout_ PRANDOM_ALLOCATOR RandomAllocator
    );

VOID
RandomAllocatorFree(
    _In_opt_ _Post_invalid_ PRANDOM_ALLOCATOR RandomAllocator
    );

#endif // _IRIS_ADVANCED_RANDOM_ALLOCATOR_INTERNAL_
