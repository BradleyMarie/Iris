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
#include "iriscommon_malloc.h"

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
    _Pre_invalid_ _Post_valid_ PDYNAMIC_MEMORY_ALLOCATOR Allocator
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
    _In_ _Pre_satisfies_(_Curr_ != 0) SIZE_T SizeInBytes,
    _In_ _Pre_satisfies_(_Curr_ != 0 && (_Curr_ & (_Curr_ -1)) == 0 && SizeInBytes % _Curr_ == 0) SIZE_T Alignment
    )
{
    PDYNAMIC_MEMORY_ALLOCATION NewerAllocation;
    PDYNAMIC_MEMORY_ALLOCATION OlderAllocation;
    PDYNAMIC_MEMORY_ALLOCATION NextAllocation;
    PVOID Header;
    PVOID Data;
    SIZE_T AllocationSize;
    BOOL Success;

    ASSERT(Allocator != NULL);
    ASSERT(SizeInBytes != 0);
    ASSERT(Alignment != 0);
    ASSERT((Alignment & (Alignment - 1)) == 0);
    ASSERT(SizeInBytes % Alignment == 0);

    if (Allocator->NextAllocation != NULL)
    {
        Success = IrisAlignedResizeWithHeader(Allocator->NextAllocation,
                                              Allocator->NextAllocation->SizeInBytes,
                                              sizeof(DYNAMIC_MEMORY_ALLOCATION),
                                              _Alignof(DYNAMIC_MEMORY_ALLOCATION),
                                              &Header,
                                              SizeInBytes,
                                              Alignment,
                                              &Data,
                                              &AllocationSize);

        if (Success == FALSE)
        {
            return NULL;
        }

        NextAllocation = (PDYNAMIC_MEMORY_ALLOCATION) Header;

        if (Allocator->NextAllocation != NextAllocation)
        {
            OlderAllocation = NextAllocation->OlderAllocation;
            NewerAllocation = NextAllocation->NewerAllocation;

            if (OlderAllocation != NULL)
            {
                OlderAllocation->NewerAllocation = NextAllocation;
            }

            if (NewerAllocation != NULL)
            {
                NewerAllocation->OlderAllocation = NextAllocation;
            }

            NextAllocation->SizeInBytes = AllocationSize;
        }

        Allocator->NextAllocation = NextAllocation->OlderAllocation;
    }
    else
    {
        Success = IrisAlignedAllocWithHeader(sizeof(DYNAMIC_MEMORY_ALLOCATION),
                                             _Alignof(DYNAMIC_MEMORY_ALLOCATION),
                                             &Header,
                                             SizeInBytes,
                                             Alignment,
                                             &Data,
                                             &AllocationSize);

        if (Success == FALSE)
        {
            return NULL;
        }

        NextAllocation = (PDYNAMIC_MEMORY_ALLOCATION) Header;

        OlderAllocation = Allocator->AllocationListTail;

        if (OlderAllocation != NULL)
        {
            OlderAllocation->NewerAllocation = NextAllocation;
        }

        NextAllocation->OlderAllocation = OlderAllocation;
        NextAllocation->NewerAllocation = NULL;
        NextAllocation->SizeInBytes = AllocationSize;

        Allocator->AllocationListTail = NextAllocation;
    }

    return Data;
}

_Check_return_ 
_Success_(return != NULL)
_Ret_maybenull_
_Post_writable_byte_size_(HeaderSizeInBytes)
SFORCEINLINE
PVOID
DynamicMemoryAllocatorAllocateWithHeader(
    _Inout_ PDYNAMIC_MEMORY_ALLOCATOR Allocator,
    _In_ _Pre_satisfies_(_Curr_ != 0) SIZE_T HeaderSizeInBytes,
    _In_ _Pre_satisfies_(_Curr_ != 0 && (_Curr_ & (_Curr_ -1)) == 0 && HeaderSizeInBytes % _Curr_ == 0) SIZE_T HeaderAlignment,
    _In_ SIZE_T DataSizeInBytes,
    _When_(DataSizeInBytes != 0, _In_ _Pre_satisfies_(_Curr_ != 0 && (_Curr_ & (_Curr_ - 1)) == 0 && DataSizeInBytes % _Curr_ == 0)) SIZE_T DataAlignment,
    _When_(DataSizeInBytes != 0, _Out_ _Deref_post_bytecap_(DataSizeInBytes)) _When_(DataSizeInBytes == 0, _Out_opt_ _When_(DataPointer != NULL, _Deref_post_null_)) PVOID *DataPointer
    )
{
    PDYNAMIC_MEMORY_ALLOCATION NewerAllocation;
    PDYNAMIC_MEMORY_ALLOCATION OlderAllocation;
    PDYNAMIC_MEMORY_ALLOCATION NextAllocation;
    SIZE_T AllocationSize;
    PVOID AllocationHeader;
    PVOID Header;
    PVOID Data;
    BOOL Success;

    ASSERT(Allocator != NULL);
    ASSERT(HeaderSizeInBytes != 0);
    ASSERT(HeaderAlignment != 0);
    ASSERT((HeaderAlignment & (HeaderAlignment - 1)) == 0);
    ASSERT(HeaderSizeInBytes % HeaderAlignment == 0);
    ASSERT(DataSizeInBytes == 0 || DataAlignment != 0);
    ASSERT(DataSizeInBytes == 0 || DataPointer != NULL);
    ASSERT(DataSizeInBytes == 0 || (DataAlignment & (DataAlignment - 1)) == 0);
    ASSERT(DataSizeInBytes == 0 || DataSizeInBytes % DataAlignment == 0);

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

    if (Allocator->NextAllocation != NULL)
    {
        Success = IrisAlignedResizeWithTwoHeader(Allocator->NextAllocation,
                                                 Allocator->NextAllocation->SizeInBytes,
                                                 sizeof(DYNAMIC_MEMORY_ALLOCATION),
                                                 _Alignof(DYNAMIC_MEMORY_ALLOCATION),
                                                 &AllocationHeader,
                                                 HeaderSizeInBytes,
                                                 HeaderAlignment,
                                                 &Header,
                                                 DataSizeInBytes,
                                                 DataAlignment,
                                                 &Data,
                                                 &AllocationSize);

        if (Success == FALSE)
        {
            return NULL;
        }

        NextAllocation = (PDYNAMIC_MEMORY_ALLOCATION) AllocationHeader;

        if (Allocator->NextAllocation != NextAllocation)
        {
            OlderAllocation = NextAllocation->OlderAllocation;
            NewerAllocation = NextAllocation->NewerAllocation;

            if (OlderAllocation != NULL)
            {
                OlderAllocation->NewerAllocation = NextAllocation;
            }

            if (NewerAllocation != NULL)
            {
                NewerAllocation->OlderAllocation = NextAllocation;
            }

            NextAllocation->SizeInBytes = AllocationSize;
        }

        Allocator->NextAllocation = NextAllocation->OlderAllocation;
    }
    else
    {
        Success = IrisAlignedAllocWithTwoHeaders(sizeof(DYNAMIC_MEMORY_ALLOCATION),
                                                 _Alignof(DYNAMIC_MEMORY_ALLOCATION),
                                                 &AllocationHeader,
                                                 HeaderSizeInBytes,
                                                 HeaderAlignment,
                                                 &Header,
                                                 DataSizeInBytes,
                                                 DataAlignment,
                                                 &Data,
                                                 &AllocationSize);

        if (Success == FALSE)
        {
            return NULL;
        }

        NextAllocation = (PDYNAMIC_MEMORY_ALLOCATION) AllocationHeader;

        OlderAllocation = Allocator->AllocationListTail;

        if (OlderAllocation != NULL)
        {
            OlderAllocation->NewerAllocation = NextAllocation;
        }

        NextAllocation->OlderAllocation = OlderAllocation;
        NextAllocation->NewerAllocation = NULL;
        NextAllocation->SizeInBytes = AllocationSize;

        Allocator->AllocationListTail = NextAllocation;
    }

    *DataPointer = Data;

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
    _Post_invalid_ PDYNAMIC_MEMORY_ALLOCATOR Allocator
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

        IrisAlignedFree(NextAllocation);

        NextAllocation = Temp;
    }
}

#endif // _IRIS_COMMON_DYNAMIC_MEMORY_ALLOCATOR_