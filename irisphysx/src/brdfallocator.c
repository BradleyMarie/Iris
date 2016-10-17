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
PhysxBrdfAllocatorAllocate(
    _Inout_ PPHYSX_BRDF_ALLOCATOR Allocator,
    _In_ PCPHYSX_BRDF_VTABLE BrdfVTable,
    _When_(DataSizeInBytes != 0, _In_reads_bytes_opt_(DataSizeInBytes)) PCVOID Data,
    _In_ SIZE_T DataSizeInBytes,
    _When_(DataSizeInBytes != 0, _Pre_satisfies_(_Curr_ != 0 && (_Curr_ & (_Curr_ - 1)) == 0 && DataSizeInBytes % _Curr_ == 0)) SIZE_T DataAlignment,
    _Out_ PCPHYSX_BRDF *Brdf
    )
{
    ISTATUS Status;

    if (Allocator == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    Status = PhysxBrdfAllocateUsingAllocator(&Allocator->Allocator,
                                             BrdfVTable,
                                             Data,
                                             DataSizeInBytes,
                                             DataAlignment,
                                             Brdf);

    return Status;
}
