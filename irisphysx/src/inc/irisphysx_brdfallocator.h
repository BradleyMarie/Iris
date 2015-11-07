/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    irisphysx_brdfallocator.h

Abstract:

    This file contains the definitions for the BRDF_ALLOCATOR type.

--*/

#ifndef _BRDF_ALLOCATOR_IRIS_PHYSX_INTERNAL_
#define _BRDF_ALLOCATOR_IRIS_PHYSX_INTERNAL_

#include <irisphysxp.h>

//
// Types
//

struct _BRDF_ALLOCATOR {
    DYNAMIC_MEMORY_ALLOCATOR Allocator;
};

//
// Functions
//

_Check_return_
_Ret_maybenull_
SFORCEINLINE
VOID
BrdfAllocatorInitialize(
    _Out_ PBRDF_ALLOCATOR BrdfAllocator
    )
{
    ASSERT(BrdfAllocator != NULL);
    
    DynamicMemoryAllocatorInitialize(&BrdfAllocator->Allocator);
}

SFORCEINLINE
VOID
BrdfAllocatorFreeAll(
    _Inout_ PBRDF_ALLOCATOR BrdfAllocator
    )
{
    ASSERT(BrdfAllocator != NULL);
    
    DynamicMemoryAllocatorFreeAll(&BrdfAllocator->Allocator);
}

SFORCEINLINE
VOID
BrdfAllocatorDestroy(
    _In_ _Post_invalid_ PBRDF_ALLOCATOR BrdfAllocator
    )
{
    ASSERT(BrdfAllocator != NULL);
    
    DynamicMemoryAllocatorDestroy(&BrdfAllocator->Allocator);
}

#endif // _BRDF_ALLOCATOR_IRIS_PHYSX_INTERNAL_