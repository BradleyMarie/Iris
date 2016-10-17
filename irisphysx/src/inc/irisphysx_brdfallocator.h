/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    irisphysx_brdfallocator.h

Abstract:

    This file contains the definitions for the PBR_BRDF_ALLOCATOR type.

--*/

#ifndef _PHYSX_BRDF_ALLOCATOR_IRIS_PHYSX_INTERNAL_
#define _PHYSX_BRDF_ALLOCATOR_IRIS_PHYSX_INTERNAL_

#include <irisphysxp.h>

//
// Types
//

struct _PHYSX_BRDF_ALLOCATOR {
    DYNAMIC_MEMORY_ALLOCATOR Allocator;
};

//
// Functions
//

SFORCEINLINE
VOID
PhysxBrdfAllocatorInitialize(
    _Out_ PPHYSX_BRDF_ALLOCATOR Allocator
    )
{
    ASSERT(Allocator != NULL);
    
    DynamicMemoryAllocatorInitialize(&Allocator->Allocator);
}

SFORCEINLINE
VOID
PhysxBrdfAllocatorFreeAll(
    _Inout_ PPHYSX_BRDF_ALLOCATOR Allocator
    )
{
    ASSERT(Allocator != NULL);
    
    DynamicMemoryAllocatorFreeAll(&Allocator->Allocator);
}

SFORCEINLINE
VOID
PhysxBrdfAllocatorDestroy(
    _In_ _Post_invalid_ PPHYSX_BRDF_ALLOCATOR Allocator
    )
{
    ASSERT(Allocator != NULL);
    
    DynamicMemoryAllocatorDestroy(&Allocator->Allocator);
}

#endif // _PHYSX_BRDF_ALLOCATOR_IRIS_PHYSX_INTERNAL_
