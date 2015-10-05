/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    iris_malloc.c

Abstract:

    This file contains the definitions for the aligned Iris malloc/free.

--*/

#include <irisp.h>

#if __STDC_VERSION__ >= 201112L
#include <stdalign.h>
#endif

_Check_return_
_Success_(return != NULL)
_Ret_maybenull_ 
_Post_writable_byte_size_(HeaderSizeInBytes)
IRISAPI
PVOID
IrisAlignedMallocWithHeader(
    _In_ SIZE_T HeaderSizeInBytes,
    _In_ SIZE_T HeaderAlignment,
    _In_ SIZE_T DataSizeInBytes,
    _In_ SIZE_T DataAlignment,
    _When_(DataSizeInBytes == 0, _Deref_post_null_) _When_(DataSizeInBytes != 0, _Outptr_result_bytebuffer_(DataSizeInBytes)) PVOID *DataPointer
    )
{
    SIZE_T AlignHeaderWithDataPadding;
    SIZE_T AlignHeaderWithHeaderPadding;
    SIZE_T HeaderAlignmentMinusOne;
    SIZE_T DataAlignmentMinusOne;
    SIZE_T AllocationSize;
    SIZE_T HeaderPadding;
    PUINT8 Allocation;
    SIZE_T Status;

    if (HeaderSizeInBytes == 0 ||
        HeaderAlignment == 0 ||
        DataPointer == NULL)
    {
        return NULL;
    }

    HeaderAlignmentMinusOne = HeaderAlignment - 1;

    if (HeaderAlignment & HeaderAlignmentMinusOne ||
        HeaderAlignmentMinusOne & HeaderSizeInBytes)
    {
        return NULL;
    }

    AllocationSize = HeaderSizeInBytes;

    if (DataSizeInBytes != 0)
    {
        if (DataAlignment == 0)
        {
            return NULL;
        }
        
        DataAlignmentMinusOne = DataAlignment - 1;

        if (DataAlignment & DataAlignmentMinusOne ||
            DataAlignmentMinusOne & DataSizeInBytes)
        {
            return NULL;
        }

        if (HeaderAlignment < DataAlignment)
        {
            AlignHeaderWithHeaderPadding = HeaderSizeInBytes % HeaderAlignment;

            if (AlignHeaderWithHeaderPadding == 0)
            {
                AlignHeaderWithHeaderPadding = DataAlignment - HeaderAlignment;
            }
            else
            {
                AlignHeaderWithHeaderPadding = DataAlignment - AlignHeaderWithHeaderPadding;
            }

            AlignHeaderWithDataPadding = HeaderSizeInBytes % DataAlignment;

            if (AlignHeaderWithHeaderPadding != 0)
            {
                AlignHeaderWithHeaderPadding = DataAlignment - AlignHeaderWithDataPadding;
            }

            if (AlignHeaderWithDataPadding < AlignHeaderWithHeaderPadding)
            {
                HeaderPadding = AlignHeaderWithDataPadding;
                HeaderAlignment = DataAlignment;
            }
            else
            {
                HeaderPadding = AlignHeaderWithHeaderPadding;
            }
        }
        else
        {
            HeaderPadding = HeaderSizeInBytes % DataAlignment;

            if (HeaderPadding != 0)
            {
                HeaderPadding = DataAlignment - HeaderPadding;
            }
        }

        Status = CheckedAddSizeT(AllocationSize,
                                 HeaderPadding,
                                 &AllocationSize);

        if (Status != ISTATUS_SUCCESS)
        {
            return NULL;
        }

        Status = CheckedAddSizeT(AllocationSize,
                                 DataSizeInBytes,
                                 &AllocationSize);

        if (Status != ISTATUS_SUCCESS)
        {
            return NULL;
        }
    }

    Allocation = (PUINT8) IrisAlignedMalloc(AllocationSize, HeaderAlignment);

    if (Allocation == NULL)
    {
        return NULL;
    }

    if (DataSizeInBytes == 0)
    {
        *DataPointer = NULL;
    }
    else
    {
        *DataPointer = (PVOID) (Allocation + HeaderSizeInBytes + HeaderPadding);
    }

    return (PVOID) Allocation;
}

_Check_return_
_Ret_maybenull_ 
_Post_writable_byte_size_(SizeInBytes)
PVOID
IrisAlignedMalloc(
    _In_ SIZE_T SizeInBytes,
    _In_ SIZE_T Alignment
    )
{
    SIZE_T AlignmentMinusOne;
    PVOID Allocation;

#if __STDC_VERSION__ >= 201112L
    
    if (SizeInBytes == 0 || Alignment == 0)
    {
        return NULL;
    }

    AlignmentMinusOne = Alignment - 1;

    if (Alignment & AlignmentMinusOne ||
        SizeInBytes & AlignmentMinusOne)
    {
        return NULL;
    }

    Allocation = aligned_alloc(Alignment, SizeInBytes);

#elif _MSC_VER

    if (SizeInBytes == 0 || Alignment == 0)
    {
        return NULL;
    }

    AlignmentMinusOne = Alignment - 1;

    if (Alignment & AlignmentMinusOne ||
        SizeInBytes & AlignmentMinusOne)
    {
        return NULL;
    }

    Allocation = _aligned_malloc(SizeInBytes, Alignment);

#else

    PVOID OriginalAllocation;
    PVOID *OriginalAllocationPointer;
    UINT_PTR AllocationAsUIntPtr;
    UINT_PTR HeaderStartOffset;
    UINT_PTR DataAlignmentOffset;
    UINT_PTR DataOffset;
    UINT_PTR DataStart;
    SIZE_T AllocationSize;
    ISTATUS Status;

    if (SizeInBytes == 0 || Alignment == 0)
    {
        return NULL;
    }

    AlignmentMinusOne = Alignment - 1;

    if (Alignment & AlignmentMinusOne ||
        SizeInBytes & AlignmentMinusOne)
    {
        return NULL;
    }

    Status = CheckedAddSizeT(SizeInBytes,
                             sizeof(PVOID),
                             &AllocationSize);

    if (Status != ISTATUS_SUCCESS)
    {
        return NULL;
    }

    if (Alignment > sizeof(PVOID))
    {
        Status = CheckedAddSizeT(AllocationSize,
                                 AlignmentMinusOne,
                                 &AllocationSize);

        if (Status != ISTATUS_SUCCESS)
        {
            return NULL;
        }
    }

    OriginalAllocation = malloc(AllocationSize);

    if (OriginalAllocation == NULL)
    {
        return NULL;
    }

    ASSERT((UINT_PTR) OriginalAllocation % sizeof(PVOID) == 0);

    AllocationAsUIntPtr = (UINT_PTR) OriginalAllocation;

    HeaderStartOffset = AllocationAsUIntPtr % sizeof(PVOID);

    if (HeaderStartOffset != 0)
    {
        HeaderStartOffset = sizeof(PVOID) - HeaderStartOffset;
    }

    DataStart = AllocationAsUIntPtr + sizeof(PVOID) + HeaderStartOffset;

    DataAlignmentOffset = DataStart % Alignment;

    if (DataAlignmentOffset != 0)
    {
        DataAlignmentOffset = Alignment - DataAlignmentOffset;
    }

    DataStart += DataAlignmentOffset;

    DataOffset = DataStart - AllocationAsUIntPtr;

    Allocation = (PVOID) ((PUINT8) OriginalAllocation + DataOffset);

    OriginalAllocationPointer = (PVOID*) ((PUINT8) Allocation + (DataOffset & (sizeof(PVOID) - 1)));
    OriginalAllocationPointer -= 1;

    *OriginalAllocationPointer = OriginalAllocation;

#endif

    return Allocation;
}

VOID
IrisAlignedFree(
    _Pre_maybenull_ _Post_invalid_ PVOID Data
    )
{
#if __STDC_VERSION__ >= 201112L

    free(Data);

#elif _MSC_VER

    _aligned_free(Data);

#else

    PVOID *OriginalAllocationPointer;
    UINT_PTR Header;

    if (Data == NULL)
    {
        return;
    }

    Header = (sizeof(PVOID) - 1) & (UINT_PTR) Data;

    OriginalAllocationPointer = (PVOID*) Header - 1;

    free(*OriginalAllocationPointer);

#endif
}