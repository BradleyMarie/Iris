/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    iriscommon_dynamicallocator.h

Abstract:

    This module implements the internal dynamic memory allocator used by Iris. 
    This allocator is not intended to be efficient nor is it intend to be a 
    general purpose allocator. It's just intended to be fast and provide 
    allocations of a dynamic size.

    The allocator works by simply creating a linked list of free regions of
    memory. When a region of memory is allocated, it just returns the first
    element off the list, expanding the region if necessary. Elements can only
    be freed in bulk.

--*/

#ifndef _IRIS_COMMON_DYNAMIC_MEMORY_ALLOCATOR_
#define _IRIS_COMMON_DYNAMIC_MEMORY_ALLOCATOR_

#include <iris.h>

//
// Types
//

struct _DYNAMIC_MEMORY_ALLOCATION;

_Struct_size_bytes_(SizeInBytes)
typedef struct _DYNAMIC_MEMORY_ALLOCATION {
    SIZE_T SizeInBytes;
    struct _DYNAMIC_MEMORY_ALLOCATION *OlderAllocation;
    struct _DYNAMIC_MEMORY_ALLOCATION *NewerAllocation;
} DYNAMIC_MEMORY_ALLOCATION, *PDYNAMIC_MEMORY_ALLOCATION;

typedef struct _DYNAMIC_MEMORY_ALLOCATOR {
    PDYNAMIC_MEMORY_ALLOCATION AllocationListTail;
    PDYNAMIC_MEMORY_ALLOCATION NextAllocation;
} DYNAMIC_MEMORY_ALLOCATOR, *PDYNAMIC_MEMORY_ALLOCATOR;

//
// Functions
//

SFORCEINLINE
VOID
DynamicMemoryAllocatorInitialize(
    _Out_ PDYNAMIC_MEMORY_ALLOCATOR Allocator
    )
{
    ASSERT(Allocator != NULL);

    Allocator->AllocationListTail = NULL;
    Allocator->NextAllocation = NULL;
}

_Check_return_ 
_Ret_maybenull_ 
_Post_writable_byte_size_(SizeInBytes)
SFORCEINLINE
PVOID
DynamicMemoryAllocatorAllocate(
    _Inout_ PDYNAMIC_MEMORY_ALLOCATOR Allocator,
    _Pre_satisfies_(SizeInBytes != 0) SIZE_T SizeInBytes,
    _In_ SIZE_T Alignment
    )
{
    PDYNAMIC_MEMORY_ALLOCATION NewerAllocation;
    PDYNAMIC_MEMORY_ALLOCATION OlderAllocation;
    PDYNAMIC_MEMORY_ALLOCATION DynamicAllocation;
    UINT_PTR WritableMemoryStartAddress;
    SIZE_T AlignmentMinusOne;
    SIZE_T AllocationSize;
    SIZE_T HeaderPadding;
    SIZE_T RequiredSpace;
    PVOID Allocation;
    ISTATUS Status;
    PVOID Resized;

    ASSERT(Allocator != NULL);
    
    if (SizeInBytes == 0 ||
        Alignment == 0)
    {
        return NULL;
    }

    AlignmentMinusOne = Alignment - 1;
    
    if (Alignment & AlignmentMinusOne ||
        SizeInBytes % Alignment != 0)
    {
        return NULL;
    }

    Status = CheckedAddSizeT(SizeInBytes, 
                             sizeof(DYNAMIC_MEMORY_ALLOCATION),
                             &AllocationSize);

    if (Status != ISTATUS_SUCCESS)
    {
        return NULL;
    }

    if (Allocator->NextAllocation != NULL)
    {
        DynamicAllocation = Allocator->NextAllocation;

        WritableMemoryStartAddress = (UINT_PTR) DynamicAllocation + sizeof(DYNAMIC_MEMORY_ALLOCATION);

        HeaderPadding = WritableMemoryStartAddress % Alignment;

        if (HeaderPadding != 0)
        {
            HeaderPadding = Alignment - HeaderPadding;
        }

        Status = CheckedAddSizeT(SizeInBytes,
                                 HeaderPadding,
                                 &RequiredSpace);
                                 
        if (Status != ISTATUS_SUCCESS)
        {
            return NULL;
        }

        if (DynamicAllocation->SizeInBytes < RequiredSpace)
        {
            if (Alignment > sizeof(PVOID))
            {
                Status = CheckedAddSizeT(SizeInBytes, 
                                         AlignmentMinusOne,
                                         &AllocationSize);
                                        
                if (Status != ISTATUS_SUCCESS)
                {
                    return NULL;
                }
            }

            Resized = realloc(DynamicAllocation, AllocationSize);

            if (Resized == NULL)
            {
                return NULL;
            }

            ASSERT((UINT_PTR) Resized % sizeof(PVOID) == 0);

            DynamicAllocation = (PDYNAMIC_MEMORY_ALLOCATION) Resized;

            OlderAllocation = DynamicAllocation->OlderAllocation;
            NewerAllocation = DynamicAllocation->NewerAllocation;

            if (OlderAllocation != NULL)
            {
                OlderAllocation->NewerAllocation = DynamicAllocation;
            }

            if (NewerAllocation != NULL)
            {
                NewerAllocation->OlderAllocation = DynamicAllocation;
            }

            DynamicAllocation->SizeInBytes = SizeInBytes;

            WritableMemoryStartAddress = (UINT_PTR) DynamicAllocation + sizeof(DYNAMIC_MEMORY_ALLOCATION);

            HeaderPadding = WritableMemoryStartAddress % Alignment;

            if (HeaderPadding != 0)
            {
                HeaderPadding = Alignment - HeaderPadding;
            }
        }

        Allocator->NextAllocation = DynamicAllocation->OlderAllocation;
    }
    else
    {
        if (Alignment > sizeof(PVOID))
        {
            Status = CheckedAddSizeT(SizeInBytes, 
                                     AlignmentMinusOne,
                                     &AllocationSize);
                                    
            if (Status != ISTATUS_SUCCESS)
            {
                return NULL;
            }
        }

        Allocation = malloc(AllocationSize);

        if (Allocation == NULL)
        {
            return NULL;
        }

        ASSERT((UINT_PTR) Allocation % sizeof(PVOID) == 0);

        DynamicAllocation = (PDYNAMIC_MEMORY_ALLOCATION) Allocation;

        OlderAllocation = Allocator->AllocationListTail;

        if (OlderAllocation != NULL)
        {
            OlderAllocation->NewerAllocation = DynamicAllocation;
        }

        DynamicAllocation->OlderAllocation = OlderAllocation;
        DynamicAllocation->NewerAllocation = NULL;
        DynamicAllocation->SizeInBytes = SizeInBytes;

        Allocator->AllocationListTail = DynamicAllocation;

        WritableMemoryStartAddress = (UINT_PTR) DynamicAllocation + sizeof(DYNAMIC_MEMORY_ALLOCATION);

        HeaderPadding = WritableMemoryStartAddress % Alignment;

        if (HeaderPadding != 0)
        {
            HeaderPadding = Alignment - HeaderPadding;
        }
    }

    return (PVOID) ((PUINT8) DynamicAllocation + sizeof(DYNAMIC_MEMORY_ALLOCATION) + HeaderPadding);
}

_Check_return_ 
_Ret_maybenull_ 
_Post_writable_byte_size_(HeaderSizeInBytes)
SFORCEINLINE
PVOID
DynamicMemoryAllocatorAllocateWithHeader(
    _Inout_ PDYNAMIC_MEMORY_ALLOCATOR Allocator,
    _In_ SIZE_T HeaderSizeInBytes,
    _In_ SIZE_T HeaderAlignment,
    _In_ SIZE_T DataSizeInBytes,
    _In_ SIZE_T DataAlignment,
    _When_(DataSizeInBytes == 0, _Deref_post_null_) _When_(DataSizeInBytes != 0, _Outptr_result_bytebuffer_(DataSizeInBytes)) PVOID *DataPointer
    )
{
    SIZE_T AllocationSize;
    SIZE_T HeaderPadding;
    ISTATUS Status;
    PVOID Header;

    ASSERT(Allocator != NULL);

    if (DataSizeInBytes == 0)
    {
        Header = DynamicMemoryAllocatorAllocate(Allocator,
                                                HeaderSizeInBytes,
                                                HeaderAlignment);
        
        if (DataPointer != NULL)
        {
            *DataPointer = NULL;
        }
                              
        return Header;    
    }
    
    if (HeaderSizeInBytes == 0 ||
        HeaderAlignment == 0 ||
        HeaderAlignment & (HeaderAlignment - 1) ||
        HeaderSizeInBytes % HeaderAlignment != 0 ||
        DataAlignment & (DataAlignment - 1) ||
        DataSizeInBytes % DataAlignment != 0 ||
        DataPointer == NULL)
    {
        return NULL;
    }

    Status = CheckedAddSizeT(HeaderSizeInBytes,
                             DataSizeInBytes,
                             &AllocationSize);
                             
    if (Status != ISTATUS_SUCCESS)
    {
        return NULL;
    }

    if (DataAlignment <= HeaderAlignment)
    { 
        HeaderPadding = 0;
        
        Header = DynamicMemoryAllocatorAllocate(Allocator,
                                                AllocationSize,
                                                HeaderAlignment);
    }
    else
    {
        HeaderPadding = HeaderSizeInBytes % DataAlignment;
        
        if (HeaderPadding != 0)
        {
            AllocationSize += DataAlignment - (HeaderPadding);
        }
        
        Header = DynamicMemoryAllocatorAllocate(Allocator,
                                                AllocationSize,
                                                DataAlignment);
    }

    if (Header == NULL)
    {
        return NULL;
    }

    *DataPointer = (PVOID) ((PUINT8) Header + HeaderSizeInBytes + HeaderPadding);

    return Header;
}

SFORCEINLINE
VOID
DynamicMemoryAllocatorFreeAll(
    _Inout_ PDYNAMIC_MEMORY_ALLOCATOR Allocator
    )
{
    ASSERT(Allocator != NULL);

    Allocator->NextAllocation = Allocator->AllocationListTail;
}

SFORCEINLINE
VOID
DynamicMemoryAllocatorDestroy(
    _Inout_ PDYNAMIC_MEMORY_ALLOCATOR Allocator
    )
{
    PDYNAMIC_MEMORY_ALLOCATION NextAllocation;
    PDYNAMIC_MEMORY_ALLOCATION Temp;

    ASSERT(Allocator != NULL);

    Allocator->NextAllocation = NULL;

    NextAllocation = Allocator->AllocationListTail;
    Allocator->AllocationListTail = NULL;

    while (NextAllocation != NULL)
    {
        Temp = NextAllocation->OlderAllocation;

        free(NextAllocation);

        NextAllocation = Temp;
    }
}

#endif // _IRIS_COMMON_DYNAMIC_MEMORY_ALLOCATOR_