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
_Success_(Return == ISTATUS_SUCCESS)
ISTATUS
BrdfAllocatorAllocate(
    _Inout_ PBRDF_ALLOCATOR BrdfAllocator,
    _In_ PBRDF_SAMPLE SampleRoutine,
    _In_ PBRDF_COMPUTE_REFLECTANCE ComputeReflectanceRoutine,
    _In_reads_bytes_(DataSizeInBytes) PCVOID Data,
    _In_ SIZE_T DataSizeInBytes,
    _In_ SIZE_T DataAlignment,
    _Out_ PCBRDF *Brdf
    )
{
    PRUNTIME_ALLOCATED_BRDF_HEADER AllocatedBrdf;
    PVOID DataDestination;
    PVOID Header;
    
    if (BrdfAllocator == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }
    
    if (SampleRoutine == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }
    
    if (ComputeReflectanceRoutine == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }
    
    if (DataSizeInBytes != 0)
    {
        if (Data == NULL)
        {
            return ISTATUS_INVALID_ARGUMENT_COMBINATION_00;
        }
        
        if (DataAlignment & DataAlignment - 1 ||
            DataSizeInBytes % DataAlignment != 0)
        {
            return ISTATUS_INVALID_ARGUMENT_COMBINATION_01;
        }
    }
    
    if (Brdf == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_06;
    }

    Header = DynamicMemoryAllocatorAllocateWithHeader(&BrdfAllocator->Allocator,
                                                      sizeof(RUNTIME_ALLOCATED_BRDF_HEADER),
                                                      sizeof(PVOID),
                                                      DataSizeInBytes,
                                                      DataAlignment,
                                                      &DataDestination);
                                                      
    if (Header == NULL)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }
    
    AllocatedBrdf = (PRUNTIME_ALLOCATED_BRDF_HEADER) Header;
    
    AllocatedBrdf->BrdfVTable.SampleRoutine = SampleRoutine;
    AllocatedBrdf->BrdfVTable.ComputeReflectanceRoutine = ComputeReflectanceRoutine;
    AllocatedBrdf->BrdfVTable.FreeRoutine = NULL;
    
    AllocatedBrdf->BrdfHeader.VTable = &AllocatedBrdf->BrdfVTable;
    AllocatedBrdf->BrdfHeader.ReferenceCount = 0;
    AllocatedBrdf->BrdfHeader.Data = DataDestination;
    
    memcpy(DataDestination, Data, DataSizeInBytes);
    
    *Brdf = &AllocatedBrdf->BrdfHeader;

    return ISTATUS_SUCCESS;
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