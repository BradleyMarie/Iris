/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    irisphysx_brdf.h

Abstract:

    This file contains the definitions for the PHYSX_BRDF type.

--*/

#ifndef _PHYSX_BRDF_IRIS_PHYSX_INTERNAL_
#define _PHYSX_BRDF_IRIS_PHYSX_INTERNAL_

#include <irisphysxp.h>

//
// Internal Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
PhysxBrdfAllocateUsingAllocator(
    _Inout_ PDYNAMIC_MEMORY_ALLOCATOR Allocator,
    _In_ PCPHYSX_BRDF_VTABLE BrdfVTable,
    _When_(DataSizeInBytes != 0, _In_reads_bytes_opt_(DataSizeInBytes)) PCVOID Data,
    _In_ SIZE_T DataSizeInBytes,
    _When_(DataSizeInBytes != 0, _Pre_satisfies_(_Curr_ != 0 && (_Curr_ & (_Curr_ - 1)) == 0 && DataSizeInBytes % _Curr_ == 0)) SIZE_T DataAlignment,
    _Out_ PCPHYSX_BRDF *Brdf
    );

#endif // _PHYSX_BRDF_IRIS_PHYSX_INTERNAL_