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
    SIZE_T AllocationAlignment;
    SIZE_T AllocationSize;
    SIZE_T HeaderPadding;
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

    if (DataSizeInBytes == 0)
    {
        Allocation = (PUINT8) IrisAlignedAlloc(HeaderSizeInBytes, HeaderAlignment);

        if (Allocation == NULL)
        {
            return FALSE;
        }

        *Header = (PVOID) Allocation;

        if (ActualAllocationSize != NULL)
        {
            *ActualAllocationSize = HeaderSizeInBytes;
        }

        if (Data != NULL)
        {
            *Data = NULL;
        }

        return TRUE;
    }

    AllocationSize = HeaderSizeInBytes;

    if (HeaderAlignment < DataAlignment)
    {
        HeaderPadding = HeaderSizeInBytes & (DataAlignment - 1);

        if (HeaderPadding != 0)
        {
            HeaderPadding = DataAlignment - HeaderPadding;

            Status = CheckedAddSizeT(AllocationSize,
                                     HeaderPadding,
                                     &AllocationSize);

            if (Status != ISTATUS_SUCCESS)
            {
                return FALSE;
            }
        }

        AllocationAlignment = DataAlignment;
    }
    else
    {
        AllocationAlignment = HeaderAlignment;
        HeaderPadding = 0;
    }

    Status = CheckedAddSizeT(AllocationSize,
                             DataSizeInBytes,
                             &AllocationSize);

    if (Status != ISTATUS_SUCCESS)
    {
        return FALSE;
    }

    Allocation = (PUINT8) IrisAlignedAlloc(AllocationSize, AllocationAlignment);

    if (Allocation == NULL)
    {
        return FALSE;
    }

    *Header = Allocation;
    *Data = (PVOID) (Allocation + HeaderSizeInBytes + HeaderPadding);

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

    if (OriginalHeader == NULL || 
        (HeaderAlignment - 1) & (UINT_PTR) OriginalHeader ||
        OriginalAllocationSize < HeaderSizeInBytes)
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

    if (DataSizeInBytes == 0)
    {
        *Header = OriginalHeader;

        if (Data != NULL)
        {
            *Data = NULL;
        }

        if (ActualAllocationSize != NULL)
        {
            *ActualAllocationSize = OriginalAllocationSize;
        }

        return TRUE;
    }

    HeaderStart = (UINT_PTR) OriginalHeader;

    Status = CheckedAddUIntPtrAndSizeT(HeaderStart,
                                       HeaderSizeInBytes,
                                       &HeaderEnd);

    if (Status != ISTATUS_SUCCESS)
    {
        return FALSE;
    }

    if (HeaderAlignment < DataAlignment)
    {
        HeaderPadding = HeaderEnd & (DataAlignment - 1);

        if (HeaderPadding != 0)
        {
            HeaderPadding = DataAlignment - HeaderPadding;

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

    if (AllocationSize <= OriginalAllocationSize)
    {
        *Header = OriginalHeader;
        *Data = (PVOID) DataStart;

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

_Check_return_
_Success_(return != FALSE)
SFORCEINLINE
BOOL
IrisAlignedAllocWithTwoHeaders(
    _Pre_satisfies_(Header0SizeInBytes != 0 && Header0SizeInBytes % Header0Alignment == 0)  SIZE_T Header0SizeInBytes,
    _Pre_satisfies_(Header0Alignment != 0 && (Header0Alignment & (Header0Alignment - 1)) == 0) SIZE_T Header0Alignment,
    _Deref_post_bytecap_(Header0SizeInBytes) PVOID *Header0,
    _Pre_satisfies_(Header1SizeInBytes != 0 && Header1SizeInBytes % Header1Alignment == 0)  SIZE_T Header1SizeInBytes,
    _Pre_satisfies_(Header1Alignment != 0 && (Header1Alignment & (Header1Alignment - 1)) == 0) SIZE_T Header1Alignment,
    _Deref_post_bytecap_(Header1SizeInBytes) PVOID *Header1,
    _When_(DataSizeInBytes != 0, _Pre_satisfies_(DataSizeInBytes % DataAlignment == 0)) SIZE_T DataSizeInBytes,
    _When_(DataSizeInBytes != 0, _Pre_satisfies_((DataAlignment & (DataAlignment - 1)) == 0)) SIZE_T DataAlignment,
    _When_(DataSizeInBytes == 0 && Data != NULL, _Deref_post_null_) _When_(DataSizeInBytes != 0, _Deref_post_bytecap_(DataSizeInBytes)) PVOID *Data,
    _Out_opt_ PSIZE_T ActualAllocationSize
    )
{
    SIZE_T AllocationAlignment;
    SIZE_T AllocationSize;
    SIZE_T Header0Padding;
    SIZE_T Header1Padding;
    PUINT8 Allocation;
    SIZE_T Status;
    BOOL Success;

    ASSERT(Header0SizeInBytes != 0);
    ASSERT(Header0Alignment != 0);
    ASSERT(Header0 != NULL);
    ASSERT((Header0Alignment & (Header0Alignment - 1)) == 0);
    ASSERT(Header0SizeInBytes % Header0Alignment == 0);
    ASSERT(Header1SizeInBytes != 0);
    ASSERT(Header1Alignment != 0);
    ASSERT(Header1 != NULL);
    ASSERT((Header1Alignment & (Header1Alignment - 1)) == 0);
    ASSERT(Header1SizeInBytes % Header1Alignment == 0);
    ASSERT(DataSizeInBytes == 0 || DataAlignment != 0);
    ASSERT(DataSizeInBytes == 0 || Data != NULL);
    ASSERT(DataSizeInBytes == 0 || (DataAlignment & (DataAlignment - 1)) == 0);
    ASSERT(DataSizeInBytes == 0 || DataSizeInBytes % DataAlignment == 0);

    if (DataSizeInBytes == 0)
    {
        Success = IrisAlignedAllocWithHeader(Header0SizeInBytes,
                                             Header0Alignment,
                                             Header0,
                                             Header1SizeInBytes,
                                             Header1Alignment,
                                             Header1,
                                             ActualAllocationSize);

        return Success;
    }

    Status = CheckedAddSizeT(Header0SizeInBytes,
                             Header1SizeInBytes,
                             &AllocationSize);

    if (Status != ISTATUS_SUCCESS)
    {
        return FALSE;
    }

    if (Header0Alignment < Header1Alignment)
    {
        Header0Padding = Header0SizeInBytes & (Header1Alignment - 1);

        if (Header0Padding != 0)
        {
            Header0Padding = Header1Alignment - Header0Padding;
        }

        Status = CheckedAddSizeT(AllocationSize,
                                 Header0Padding,
                                 &AllocationSize);

        if (Status != ISTATUS_SUCCESS)
        {
            return FALSE;
        }

        AllocationAlignment = Header1Alignment;
    }
    else
    {
        AllocationAlignment = Header0Alignment;
        Header0Padding = 0;
    }

    if (Header1Alignment < DataAlignment)
    {
        Header1Padding = AllocationSize & (DataAlignment - 1);

        if (Header1Padding != 0)
        {
            Header1Padding = DataAlignment - Header1Padding;
        }

        Status = CheckedAddSizeT(AllocationSize,
                                 Header1Padding,
                                 &AllocationSize);

        if (Status != ISTATUS_SUCCESS)
        {
            return FALSE;
        }

        if (AllocationAlignment < DataAlignment)
        {
            AllocationAlignment = DataAlignment;
        }
    }
    else
    {
        if (AllocationAlignment < Header1Alignment)
        {
            AllocationAlignment = Header1Alignment;
        }

        Header1Padding = 0;
    }

    Status = CheckedAddSizeT(AllocationSize,
                             DataSizeInBytes,
                             &AllocationSize);

    if (Status != ISTATUS_SUCCESS)
    {
        return FALSE;
    }

    Allocation = (PUINT8) IrisAlignedAlloc(AllocationSize, AllocationAlignment);

    if (Allocation == NULL)
    {
        return FALSE;
    }

    *Header0 = Allocation;
    *Header1 = (PVOID) (Allocation + Header0SizeInBytes + Header0Padding);
    *Data = (PVOID) (Allocation + 
                     Header0SizeInBytes + 
                     Header0Padding + 
                     Header1SizeInBytes + 
                     Header1Padding);

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
IrisAlignedResizeWithTwoHeader(
    _Pre_opt_bytecap_(OriginalAllocationSize) _Post_invalid_ PVOID OriginalHeader,
    _When_(OriginalHeader != NULL, _In_) SIZE_T OriginalAllocationSize,
    _Pre_satisfies_(Header0SizeInBytes != 0 && Header0SizeInBytes % Header0Alignment == 0)  SIZE_T Header0SizeInBytes,
    _Pre_satisfies_(Header0Alignment != 0 && (Header0Alignment & (Header0Alignment - 1)) == 0) SIZE_T Header0Alignment,
    _Deref_post_bytecap_(Header0SizeInBytes) PVOID *Header0,
    _Pre_satisfies_(Header1SizeInBytes != 0 && Header1SizeInBytes % Header1Alignment == 0)  SIZE_T Header1SizeInBytes,
    _Pre_satisfies_(Header1Alignment != 0 && (Header1Alignment & (Header1Alignment - 1)) == 0) SIZE_T Header1Alignment,
    _Deref_post_bytecap_(Header1SizeInBytes) PVOID *Header1,
    _When_(DataSizeInBytes != 0, _Pre_satisfies_(DataSizeInBytes % DataAlignment == 0)) SIZE_T DataSizeInBytes,
    _When_(DataSizeInBytes != 0, _Pre_satisfies_((DataAlignment & (DataAlignment - 1)) == 0)) SIZE_T DataAlignment,
    _When_(DataSizeInBytes == 0 && Data != NULL, _Deref_post_null_) _When_(DataSizeInBytes != 0, _Deref_post_bytecap_(DataSizeInBytes)) PVOID *Data,
    _Out_opt_ PSIZE_T ActualAllocationSize
    )
{
    SIZE_T AllocationSize;
    SIZE_T Header0Padding;
    SIZE_T Header1Padding;
    UINT_PTR Header0Start;
    UINT_PTR Header0End;
    UINT_PTR Header1Start;
    UINT_PTR Header1End;
    UINT_PTR DataStart;
    UINT_PTR DataEnd;
    ISTATUS Status;
    BOOL Result;

    ASSERT(OriginalAllocationSize != 0 || OriginalHeader == NULL);
    ASSERT(Header0SizeInBytes != 0);
    ASSERT(Header0Alignment != 0);
    ASSERT(Header0 != NULL);
    ASSERT((Header0Alignment & (Header0Alignment - 1)) == 0);
    ASSERT(Header0SizeInBytes % Header0Alignment == 0);
    ASSERT(Header1SizeInBytes != 0);
    ASSERT(Header1Alignment != 0);
    ASSERT(Header1 != NULL);
    ASSERT((Header1Alignment & (Header1Alignment - 1)) == 0);
    ASSERT(Header1SizeInBytes % Header1Alignment == 0);
    ASSERT(DataSizeInBytes == 0 || DataAlignment != 0);
    ASSERT(DataSizeInBytes == 0 || Data != NULL);
    ASSERT(DataSizeInBytes == 0 || (DataAlignment & (DataAlignment - 1)) == 0);
    ASSERT(DataSizeInBytes == 0 || DataSizeInBytes % DataAlignment == 0);

    if (OriginalHeader == NULL || 
        (Header0Alignment - 1) & (UINT_PTR) OriginalHeader)
    {
        Result = IrisAlignedAllocWithTwoHeaders(Header0SizeInBytes,
                                                Header0Alignment,
                                                Header0,
                                                Header1SizeInBytes,
                                                Header1Alignment,
                                                Header1,
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

    Header0Start = (UINT_PTR) OriginalHeader;

    Status = CheckedAddUIntPtrAndSizeT(Header0Start,
                                       Header0SizeInBytes,
                                       &Header0End);

    if (Status != ISTATUS_SUCCESS)
    {
        return FALSE;
    }

    if (Header0Alignment < Header1Alignment)
    {
        Header0Padding = Header0End & (Header1Alignment - 1);

        if (Header0Padding != 0)
        {
            Header0Padding = Header1Alignment - Header0Padding;

            Status = CheckedAddUIntPtrAndSizeT(Header0End,
                                               Header0Padding,
                                               &Header1Start);

            if (Status != ISTATUS_SUCCESS)
            {
                return FALSE;
            }
        }
        else
        {
            Header1Start = Header0End;
        }
    }
    else
    {
        Header1Start = Header0End;
    }

    Status = CheckedAddUIntPtrAndSizeT(Header1Start,
                                       Header1SizeInBytes,
                                       &Header1End);

    if (Status != ISTATUS_SUCCESS)
    {
        return FALSE;
    }

    if (DataSizeInBytes == 0)
    {
        AllocationSize = Header1End - Header0Start;

        if (AllocationSize <= OriginalAllocationSize)
        {
            *Header0 = OriginalHeader;
            *Header1 = (PVOID) Header1Start;

            if (Data != NULL)
            {
                *Data = NULL;
            }

            if (ActualAllocationSize != NULL)
            {
                *ActualAllocationSize = OriginalAllocationSize;
            }

            return TRUE;
        }

        Result = IrisAlignedAllocWithTwoHeaders(Header0SizeInBytes,
                                                Header0Alignment,
                                                Header0,
                                                Header1SizeInBytes,
                                                Header1Alignment,
                                                Header1,
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

    if (Header1Alignment < DataAlignment)
    {
        Header1Padding = Header1End & (DataAlignment - 1);

        if (Header1Padding != 0)
        {
            Header1Padding = DataAlignment - Header1Padding;

            Status = CheckedAddUIntPtrAndSizeT(Header1End,
                                               Header1Padding,
                                               &DataStart);

            if (Status != ISTATUS_SUCCESS)
            {
                return FALSE;
            }
        }
        else
        {
            Header1End = DataStart;
        }
    }
    else
    {
        Header1End = DataStart;
    }

    Status = CheckedAddUIntPtrAndSizeT(DataStart,
                                       DataSizeInBytes,
                                       &DataEnd);

    if (Status != ISTATUS_SUCCESS)
    {
        return FALSE;
    }

    AllocationSize = DataEnd - Header0Start; 

    if (AllocationSize <= OriginalAllocationSize)
    {
        *Header0 = OriginalHeader;
        *Header1 = (PVOID) Header1Start;
        *Data = (PVOID) DataStart;

        if (ActualAllocationSize != NULL)
        {
            *ActualAllocationSize = OriginalAllocationSize;
        }

        return TRUE;
    }

    Result = IrisAlignedAllocWithTwoHeaders(Header0SizeInBytes,
                                            Header0Alignment,
                                            Header0,
                                            Header1SizeInBytes,
                                            Header1Alignment,
                                            Header1,
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