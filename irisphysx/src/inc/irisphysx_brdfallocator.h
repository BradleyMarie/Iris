/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    irisphysx_brdfallocator.h

Abstract:

    This file contains the definitions for the PBR_BRDF_ALLOCATOR type.

--*/

#ifndef _PBR_BRDF_ALLOCATOR_IRIS_PHYSX_INTERNAL_
#define _PBR_BRDF_ALLOCATOR_IRIS_PHYSX_INTERNAL_

#include <irisphysxp.h>

//
// Types
//

struct _PBR_BRDF_ALLOCATOR {
    DYNAMIC_MEMORY_ALLOCATOR Allocator;
};

//
// Functions
//

_Check_return_
_Ret_maybenull_
SFORCEINLINE
VOID
PbrBrdfAllocatorInitialize(
    _Out_ PPBR_BRDF_ALLOCATOR PbrBrdfAllocator
    )
{
    ASSERT(PbrBrdfAllocator != NULL);
    
    DynamicMemoryAllocatorInitialize(&PbrBrdfAllocator->Allocator);
}

SFORCEINLINE
VOID
PbrBrdfAllocatorFreeAll(
    _Inout_ PPBR_BRDF_ALLOCATOR PbrBrdfAllocator
    )
{
    ASSERT(PbrBrdfAllocator != NULL);
    
    DynamicMemoryAllocatorFreeAll(&PbrBrdfAllocator->Allocator);
}

SFORCEINLINE
VOID
PbrBrdfAllocatorDestroy(
    _In_ _Post_invalid_ PPBR_BRDF_ALLOCATOR PbrBrdfAllocator
    )
{
    ASSERT(PbrBrdfAllocator != NULL);
    
    DynamicMemoryAllocatorDestroy(&PbrBrdfAllocator->Allocator);
}

#endif // _PBR_BRDF_ALLOCATOR_IRIS_PHYSX_INTERNAL_