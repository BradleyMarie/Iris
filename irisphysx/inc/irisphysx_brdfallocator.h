/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    irisphysx_brdfalloocator.h

Abstract:

    This file contains the definitions for the PHYSX_BRDF_ALLOCATOR type.

--*/

#ifndef _PHYSX_BRDF_ALLOCATOR_IRIS_PHYSX_
#define _PHYSX_BRDF_ALLOCATOR_IRIS_PHYSX_

#include <irisphysx.h>

//
// Types
//

typedef struct _PHYSX_BRDF_ALLOCATOR PHYSX_BRDF_ALLOCATOR, *PPHYSX_BRDF_ALLOCATOR;
typedef CONST PHYSX_BRDF_ALLOCATOR *PCPHYSX_BRDF_ALLOCATOR;

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISPHYSXAPI
ISTATUS
PhysxBrdfAllocatorAllocate(
    _Inout_ PPHYSX_BRDF_ALLOCATOR Allocator,
    _In_ PCPHYSX_BRDF_VTABLE BrdfVTable,
    _When_(DataSizeInBytes != 0, _In_reads_bytes_opt_(DataSizeInBytes)) PCVOID Data,
    _In_ SIZE_T DataSizeInBytes,
    _When_(DataSizeInBytes != 0, _Pre_satisfies_(_Curr_ != 0 && (_Curr_ & (_Curr_ - 1)) == 0 && DataSizeInBytes % _Curr_ == 0)) SIZE_T DataAlignment,
    _Out_ PCPHYSX_BRDF *Brdf
    );

#endif // _PHYSX_BRDF_ALLOCATOR_IRIS_PHYSX_