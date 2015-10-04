/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    brdfallocator.c

Abstract:

    This file contains the definitions for the BRDF_ALLOCATOR type.

--*/

#include <irisphysxp.h>

//
// Types
//

typedef struct _RUNTIME_ALLOCATED_BRDF_HEADER {
    BRDF_VTABLE BrdfVTable;
    BRDF BrdfHeader;
} RUNTIME_ALLOCATED_BRDF_HEADER, *PRUNTIME_ALLOCATED_BRDF_HEADER;

typedef CONST RUNTIME_ALLOCATED_BRDF_HEADER *PCRUNTIME_ALLOCATED_BRDF_HEADER;

struct _BRDF_ALLOCATOR {
    DYNAMIC_MEMORY_ALLOCATOR Allocator;
};

//
// Functions
//

_Check_return_
_Ret_maybenull_
PBRDF_ALLOCATOR
BrdfAllocatorCreate(
    VOID
    )
{
    PBRDF_ALLOCATOR Allocator;
    
    Allocator = (PBRDF_ALLOCATOR) malloc(sizeof(BRDF_ALLOCATOR));
    
    if (Allocator == NULL)
    {
        return NULL;
    }
    
    DynamicMemoryAllocatorInitialize(&Allocator->Allocator);
    
    return Allocator;
}

_Check_return_
_Ret_maybenull_
PCBRDF
BrdfAllocatorAllocate(
    _Inout_ PBRDF_ALLOCATOR BrdfAllocator,
    _In_ PBRDF_SAMPLE SampleRoutine,
    _In_ PBRDF_COMPUTE_REFLECTANCE ComputeReflectanceRoutine,
    _In_reads_bytes_(DataSizeInBytes) PCVOID Data,
    _In_ SIZE_T DataSizeInBytes,
    _In_ SIZE_T DataAlignment
    )
{
    return NULL;
}

VOID
BrdfAllocatorFreeAll(
    _Inout_opt_ PBRDF_ALLOCATOR BrdfAllocator
    )
{
    if (BrdfAllocator == NULL)
    {
        return;
    }
    
    DynamicMemoryAllocatorFreeAll(&BrdfAllocator->Allocator);
}

VOID
BrdfAllocatorDestroy(
    _Pre_maybenull_ _Post_invalid_ PBRDF_ALLOCATOR BrdfAllocator
    )
{
    if (BrdfAllocator == NULL)
    {
        return;
    }
    
    DynamicMemoryAllocatorDestroy(&BrdfAllocator->Allocator);
    
    free(BrdfAllocator);
}