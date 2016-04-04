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
PbrBrdfAllocatorAllocate(
    _Inout_ PPBR_BRDF_ALLOCATOR BrdfAllocator,
    _In_ PCPBR_BRDF_VTABLE PbrBrdfVTable,
    _When_(DataSizeInBytes != 0, _In_reads_bytes_opt_(DataSizeInBytes)) PCVOID Data,
    _In_ SIZE_T DataSizeInBytes,
    _When_(DataSizeInBytes != 0, _Pre_satisfies_(_Curr_ != 0 && (_Curr_ & (_Curr_ - 1)) == 0 && DataSizeInBytes % _Curr_ == 0)) SIZE_T DataAlignment,
    _Out_ PCPBR_BRDF *PbrBrdf
    )
{
    PPBR_BRDF AllocatedBrdf;
    PVOID DataDestination;
    PVOID Header;
    
    if (BrdfAllocator == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }
    
    if (PbrBrdfVTable == NULL)
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
    
    if (PbrBrdf == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_05;
    }

    Header = DynamicMemoryAllocatorAllocateWithHeader(&BrdfAllocator->Allocator,
                                                      sizeof(PBR_BRDF),
                                                      _Alignof(PBR_BRDF),
                                                      DataSizeInBytes,
                                                      DataAlignment,
                                                      &DataDestination);
                                                      
    if (Header == NULL)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }
    
    AllocatedBrdf = (PPBR_BRDF) Header;
    
    AllocatedBrdf->VTable = PbrBrdfVTable;
    AllocatedBrdf->ReferenceCount = 0;
    AllocatedBrdf->Data = DataDestination;
    
    memcpy(DataDestination, Data, DataSizeInBytes);
    
    *PbrBrdf = AllocatedBrdf;

    return ISTATUS_SUCCESS;
}