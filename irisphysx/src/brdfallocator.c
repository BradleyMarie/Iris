/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    brdfallocator.c

Abstract:

    This file contains the definitions for the BRDF_ALLOCATOR type.

--*/

#include <irisphysxp.h>

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
BrdfAllocatorAllocate(
    _Inout_ PBRDF_ALLOCATOR BrdfAllocator,
    _In_ PCBRDF_VTABLE BrdfVTable,
    _When_(DataSizeInBytes != 0, _In_reads_bytes_opt_(DataSizeInBytes)) PCVOID Data,
    _In_ SIZE_T DataSizeInBytes,
    _When_(DataSizeInBytes != 0, _Pre_satisfies_(_Curr_ != 0 && (_Curr_ & (_Curr_ - 1)) == 0 && DataSizeInBytes % _Curr_ == 0)) SIZE_T DataAlignment,
    _Out_ PCBRDF *Brdf
    )
{
    PBRDF AllocatedBrdf;
    PVOID DataDestination;
    PVOID Header;
    
    if (BrdfAllocator == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }
    
    if (BrdfVTable == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }
    
    if (DataSizeInBytes != 0)
    {
        if (Data == NULL)
        {
            return ISTATUS_INVALID_ARGUMENT_COMBINATION_00;
        }
        
        if (DataAlignment == 0 ||
            DataAlignment & DataAlignment - 1)
        {
            return ISTATUS_INVALID_ARGUMENT_COMBINATION_01;    
        }
        
        if (DataSizeInBytes % DataAlignment != 0)
        {
            return ISTATUS_INVALID_ARGUMENT_COMBINATION_02;
        }
    }
    
    if (Brdf == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_05;
    }

    Header = DynamicMemoryAllocatorAllocateWithHeader(&BrdfAllocator->Allocator,
                                                      sizeof(BRDF),
                                                      _Alignof(BRDF),
                                                      DataSizeInBytes,
                                                      DataAlignment,
                                                      &DataDestination);
                                                      
    if (Header == NULL)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }
    
    AllocatedBrdf = (PBRDF) Header;
    
    AllocatedBrdf->VTable = BrdfVTable;
    AllocatedBrdf->ReferenceCount = 0;
    AllocatedBrdf->Data = DataDestination;
    
    memcpy(DataDestination, Data, DataSizeInBytes);
    
    *Brdf = AllocatedBrdf;

    return ISTATUS_SUCCESS;
}