/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    irisphysx_brdfalloocator.h

Abstract:

    This file contains the definitions for the PBR_BRDF_ALLOCATOR type.

--*/

#ifndef _PBR_BRDF_ALLOCATOR_IRIS_PHYSX_
#define _PBR_BRDF_ALLOCATOR_IRIS_PHYSX_

#include <irisphysx.h>

//
// Types
//

typedef struct _PBR_BRDF_ALLOCATOR PBR_BRDF_ALLOCATOR, *PPBR_BRDF_ALLOCATOR;
typedef CONST PBR_BRDF_ALLOCATOR *PCPBR_BRDF_ALLOCATOR;

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISPHYSXAPI
ISTATUS
PbrBrdfAllocatorAllocate(
    _Inout_ PPBR_BRDF_ALLOCATOR PbrBrdfAllocator,
    _In_ PCPBR_BRDF_VTABLE PbrBrdfVTable,
    _When_(DataSizeInBytes != 0, _In_reads_bytes_opt_(DataSizeInBytes)) PCVOID Data,
    _In_ SIZE_T DataSizeInBytes,
    _When_(DataSizeInBytes != 0, _Pre_satisfies_(_Curr_ != 0 && (_Curr_ & (_Curr_ - 1)) == 0 && DataSizeInBytes % _Curr_ == 0)) SIZE_T DataAlignment,
    _Out_ PCPBR_BRDF *PbrBrdf
    );

#endif // _PBR_BRDF_ALLOCATOR_IRIS_PHYSX_