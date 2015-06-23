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
    SIZE_T HeaderAlignmentMinusOne;
    SIZE_T DataAlignmentMinusOne;
    SIZE_T AlignHeaderWithDataPadding;
    SIZE_T AlignHeaderWithHeaderPadding;
    SIZE_T AllocationSize;
    SIZE_T HeaderPadding;
    PUINT8 Allocation;
    SIZE_T Status;

    if (HeaderSizeInBytes == 0 ||
        HeaderAlignment < sizeof(PVOID) ||
        DataPointer == NULL)
    {
        return NULL;
    }

    HeaderAlignmentMinusOne = HeaderAlignment - 1;

    if (HeaderAlignment & HeaderAlignmentMinusOne)
    {
        return NULL;
    }

    AllocationSize = HeaderSizeInBytes;

    if (DataSizeInBytes != 0)
    {
        DataAlignmentMinusOne = DataAlignment - 1;

        if (DataAlignment & DataAlignmentMinusOne)
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
    
    if (SizeInBytes == 0 || Alignment < sizeof(PVOID))
    {
        return NULL;
    }

    AlignmentMinusOne = Alignment - 1;

    if (Alignment & AlignmentMinusOne)
    {
        return NULL;
    }

    Allocation = aligned_alloc(Alignment, SizeInBytes);

#elif _MSC_VER

    if (SizeInBytes == 0 || Alignment < sizeof(PVOID))
    {
        return NULL;
    }

    AlignmentMinusOne = Alignment - 1;

    if (Alignment & AlignmentMinusOne)
    {
        return NULL;
    }

    Allocation = _aligned_malloc(SizeInBytes, Alignment);

#else

    PVOID OriginalAllocation;
    PUINT8 AllocationAsByteArray;
    PVOID *OriginalAllocationPointer;
    SIZE_T AllocationAlignment;
    SIZE_T AllocationSize;
    ISTATUS Status;

    if (SizeInBytes == 0 || Alignment < sizeof(PVOID))
    {
        return NULL;
    }

    AlignmentMinusOne = Alignment - 1;

    if (Alignment & AlignmentMinusOne)
    {
        return NULL;
    }

    Status = CheckedAddSizeT(SizeInBytes,
                             AlignmentMinusOne,
                             &AllocationSize);

    if (Status != ISTATUS_SUCCESS)
    {
        return NULL;
    }

    Status = CheckedAddSizeT(AllocationSize,
                             sizeof(PVOID),
                             &AllocationSize);

    if (Status != ISTATUS_SUCCESS)
    {
        return NULL;
    }

    OriginalAllocation = malloc(AllocationSize);

    if (OriginalAllocation == NULL)
    {
        return NULL;
    }

    AllocationAsByteArray = (PUINT8) OriginalAllocation + sizeof(PVOID);
    AllocationAlignment = (UINT_PTR) AllocationAsByteArray % Alignment;

    if (AllocationAlignment != 0)
    {
        AllocationAsByteArray += Alignment - AllocationAlignment;
    }

    OriginalAllocationPointer = (PVOID*) AllocationAsByteArray - 1;

    *OriginalAllocationPointer = OriginalAllocation;

    Allocation = (PVOID) AllocationAsByteArray;

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

    if (Data == NULL)
    {
        return;
    }

    OriginalAllocationPointer = (PVOID*) Data - 1;

    free(*OriginalAllocationPointer);

#endif
}