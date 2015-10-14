/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    iriscommon_malloc.h

Abstract:

    This file contains the prototypes for the aligned memory allocation.

--*/

#include <iris.h>

#ifndef _MALLOC_IRIS_COMMON_
#define _MALLOC_IRIS_COMMON_

#ifndef _MSC_VER
#include <stdalign.h>
#endif

//
// Prototypes
//

SFORCEINLINE
VOID
IrisAlignedFree(
    _Pre_maybenull_ _Post_invalid_ PVOID Header
    )
{
#ifndef _MSVC_VER
    _aligned_free(Header);
#else
    free(Header);
#endif
}

_Check_return_
_Ret_maybenull_ 
_Post_writable_byte_size_(SizeInBytes)
SFORCEINLINE
PVOID
IrisAlignedAlloc(
    _Pre_satisfies_(SizeInBytes != 0 && SizeInBytes % Alignment == 0)  SIZE_T SizeInBytes,
    _Pre_satisfies_(Alignment != 0 && (Alignment & (Alignment - 1)) == 0) SIZE_T Alignment
    )
{
    PVOID Allocation;

    if (SizeInBytes == 0 ||
        Alignment == 0 ||
        Alignment & (Alignment - 1))
    {
        return NULL;
    }

#ifndef _MSVC_VER
    Allocation = _aligned_malloc(SizeInBytes, Alignment);
#else
    Allocation = aligned_alloc(SizeInBytes, Alignment);
#endif

    return Allocation;
}

_Check_return_
_Success_(return != FALSE)
SFORCEINLINE
BOOL
IrisAlignedAllocWithHeader(
    _Pre_satisfies_(HeaderSizeInBytes != 0 && HeaderSizeInBytes % HeaderAlignment == 0)  SIZE_T HeaderSizeInBytes,
    _Pre_satisfies_(HeaderAlignment != 0 && (HeaderAlignment & (HeaderAlignment - 1)) == 0) SIZE_T HeaderAlignment,
    _Deref_post_bytecap_(HeaderSizeInBytes) PVOID *Header,
    _When_(DataSizeInBytes != 0, _Pre_satisfies_(DataSizeInBytes % DataAlignment == 0)) SIZE_T DataSizeInBytes,
    _When_(DataSizeInBytes != 0, _Pre_satisfies_((DataAlignment & (DataAlignment - 1)) == 0)) SIZE_T DataAlignment,
    _When_(DataSizeInBytes == 0 && Data != NULL, _Deref_post_null_) _When_(DataSizeInBytes != 0, _Deref_post_bytecap_(DataSizeInBytes)) PVOID *Data,
    _Out_opt_ PSIZE_T ActualAllocationSize
    )
{
    SIZE_T AllocateWithHeaderAlignmentPadding;
    SIZE_T AllocateWithDataAlignmentPadding;
    SIZE_T AllocationAlignment;
    SIZE_T AllocationPadding;
    SIZE_T AllocationSize;
    PUINT8 Allocation;
    SIZE_T Status;

    ASSERT(HeaderSizeInBytes != 0);
    ASSERT(HeaderAlignment != 0);
    ASSERT(Header != NULL);
    ASSERT((HeaderAlignment & (HeaderAlignment - 1)) == 0);
    ASSERT(HeaderSizeInBytes % HeaderAlignment == 0);
    ASSERT(DataSizeInBytes == 0 || DataAlignment != 0);
    ASSERT(DataSizeInBytes == 0 || Data != NULL);
    ASSERT(DataSizeInBytes == 0 || (DataAlignment & (DataAlignment - 1)) == 0);
    ASSERT(DataSizeInBytes == 0 || DataSizeInBytes % DataAlignment == 0);

    AllocationSize = HeaderSizeInBytes;

    if (DataSizeInBytes != 0)
    {
        if (HeaderAlignment < DataAlignment)
        {
            AllocateWithHeaderAlignmentPadding = DataAlignment - HeaderAlignment;

            AllocateWithDataAlignmentPadding = HeaderSizeInBytes % DataAlignment;

            if (AllocateWithDataAlignmentPadding != 0)
            {
                AllocateWithDataAlignmentPadding = DataAlignment - AllocateWithDataAlignmentPadding;
            }

            if (AllocateWithDataAlignmentPadding < AllocateWithHeaderAlignmentPadding)
            {
                AllocationPadding = AllocateWithDataAlignmentPadding;
                AllocationAlignment = DataAlignment;
            }
            else
            {
                AllocationPadding = AllocateWithHeaderAlignmentPadding;
                AllocationAlignment = HeaderAlignment;
            }

            Status = CheckedAddSizeT(AllocationPadding,
                                     AllocationPadding,
                                     &AllocationSize);

            if (Status != ISTATUS_SUCCESS)
            {
                return FALSE;
            }
        }

        Status = CheckedAddSizeT(AllocationSize,
                                 DataSizeInBytes,
                                 &AllocationSize);

        if (Status != ISTATUS_SUCCESS)
        {
            return FALSE;
        }
    }
    else
    {
        AllocationAlignment = HeaderAlignment;
    }

    Allocation = (PUINT8) IrisAlignedAlloc(AllocationSize, AllocationAlignment);

    if (Allocation == NULL)
    {
        return FALSE;
    }

    *Header = Allocation;

    if (DataSizeInBytes == 0)
    {
        if (Data != NULL)
        {
            *Data = NULL;
        }
    }
    else
    {
        *Data = (PVOID) (Allocation + HeaderSizeInBytes + AllocationPadding);
    }

    if (ActualAllocationSize != NULL)
    {
        *ActualAllocationSize = AllocationSize;
    }

    return TRUE;
}

_Check_return_
_Success_(return != FALSE)
SFORCEINLINE
BOOL
IrisAlignedResizeWithHeader(
    _Pre_opt_bytecap_(OriginalAllocationSize) _Post_invalid_ PVOID OriginalHeader,
    _When_(OriginalHeader != NULL, _In_) SIZE_T OriginalAllocationSize,
    _Pre_satisfies_(HeaderSizeInBytes != 0 && HeaderSizeInBytes % HeaderAlignment == 0) SIZE_T HeaderSizeInBytes,
    _Pre_satisfies_(HeaderAlignment != 0 && (HeaderAlignment & (HeaderAlignment - 1)) == 0) SIZE_T HeaderAlignment,
    _Deref_post_bytecap_(HeaderSizeInBytes) PVOID *Header,
    _When_(DataSizeInBytes != 0, _Pre_satisfies_(DataSizeInBytes % DataAlignment == 0)) SIZE_T DataSizeInBytes,
    _When_(DataSizeInBytes != 0, _Pre_satisfies_((DataAlignment & (DataAlignment - 1)) == 0)) SIZE_T DataAlignment,
    _When_(DataSizeInBytes == 0 && Data != NULL, _Deref_post_null_) _When_(DataSizeInBytes != 0, _Deref_post_bytecap_(DataSizeInBytes)) PVOID *Data,
    _Out_opt_ PSIZE_T ActualAllocationSize
    )
{
    SIZE_T AllocationSize;
    SIZE_T HeaderPadding;
    UINT_PTR HeaderStart;
    UINT_PTR HeaderEnd;
    UINT_PTR DataStart;
    UINT_PTR DataEnd;
    ISTATUS Status;
    BOOL Result;

    ASSERT(OriginalAllocationSize != 0 || OriginalHeader == NULL);
    ASSERT(HeaderSizeInBytes != 0);
    ASSERT(HeaderAlignment != 0);
    ASSERT(Header != NULL);
    ASSERT((HeaderAlignment & (HeaderAlignment - 1)) == 0);
    ASSERT(HeaderSizeInBytes % HeaderAlignment == 0);
    ASSERT(DataSizeInBytes == 0 || DataAlignment != 0);
    ASSERT(DataSizeInBytes == 0 || Data != NULL);
    ASSERT(DataSizeInBytes == 0 || (DataAlignment & (DataAlignment - 1)) == 0);
    ASSERT(DataSizeInBytes == 0 || DataSizeInBytes % DataAlignment == 0);

    if (OriginalHeader == NULL || (HeaderAlignment - 1) & (UINT_PTR) OriginalHeader)
    {
        Result = IrisAlignedAllocWithHeader(HeaderSizeInBytes,
                                            HeaderAlignment,
                                            Header,
                                            DataSizeInBytes,
                                            DataAlignment,
                                            Data,
                                            ActualAllocationSize);

        if (Result != FALSE)
        {
            IrisAlignedFree(OriginalHeader);
        }

        return Result;
    }

    HeaderStart = (UINT_PTR) OriginalHeader;

    Status = CheckedAddUIntPtrAndSizeT(HeaderStart,
                                       HeaderSizeInBytes,
                                       &HeaderEnd);

    if (Status != ISTATUS_SUCCESS)
    {
        return FALSE;
    }

    if (DataSizeInBytes != 0)
    {
        if (HeaderAlignment < DataAlignment)
        {
            HeaderPadding = HeaderEnd & (DataAlignment - 1);

            if (HeaderPadding != 0)
            {
                HeaderPadding = DataAlignment - HeaderPadding;
            }

            Status = CheckedAddUIntPtrAndSizeT(HeaderEnd,
                                               HeaderPadding,
                                               &DataStart);

            if (Status != ISTATUS_SUCCESS)
            {
                return FALSE;
            }
        }
        else
        {
            DataStart = HeaderEnd;
        }

        Status = CheckedAddUIntPtrAndSizeT(DataStart,
                                           DataSizeInBytes,
                                           &DataEnd);

        if (Status != ISTATUS_SUCCESS)
        {
            return FALSE;
        }

        AllocationSize = DataEnd - HeaderStart;
    }
    else
    {
        AllocationSize = HeaderSizeInBytes;
    }

    if (AllocationSize <= OriginalAllocationSize)
    {
        *Header = OriginalHeader;

        if (DataSizeInBytes == 0)
        {
            if (Data != NULL)
            {
                *Data = NULL;
            }            
        }
        else
        {
            *Data = (PVOID) DataStart;
        }

        if (ActualAllocationSize != NULL)
        {
            *ActualAllocationSize = OriginalAllocationSize;
        }

        return TRUE;
    }

    Result = IrisAlignedAllocWithHeader(HeaderSizeInBytes,
                                        HeaderAlignment,
                                        Header,
                                        DataSizeInBytes,
                                        DataAlignment,
                                        Data,
                                        ActualAllocationSize);

    if (Result != FALSE)
    {
        IrisAlignedFree(OriginalHeader);
    }

    return Result;
}

#endif // _MALLOC_IRIS_COMMON_