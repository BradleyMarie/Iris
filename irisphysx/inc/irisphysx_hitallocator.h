/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    irisphysx_hitallocator.h

Abstract:

    This module implements the PHYSX_HIT_ALLOCATOR public functions.

--*/

#ifndef _IRIS_PHYSX_HIT_ALLOCATOR_
#define _IRIS_PHYSX_HIT_ALLOCATOR_

#include <iris.h>

//
// Types
//

typedef struct _PHYSX_HIT_ALLOCATOR PHYSX_HIT_ALLOCATOR, *PPHYSX_HIT_ALLOCATOR;
typedef CONST PHYSX_HIT_ALLOCATOR *PCPHYSX_HIT_ALLOCATOR;

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISPHYSXAPI
ISTATUS
PhysxHitAllocatorAllocate(
    _Inout_ PPHYSX_HIT_ALLOCATOR HitAllocator,
    _In_opt_ PHIT_LIST NextHit,
    _In_ FLOAT Distance,
    _In_ UINT32 FrontFace,
    _In_ UINT32 BackFace,
    _In_reads_bytes_opt_(AdditionalDataSizeInBytes) PCVOID AdditionalData,
    _In_ SIZE_T AdditionalDataSizeInBytes,
    _In_ SIZE_T AdditionalDataAlignment,
    _Out_ PHIT_LIST *HitList
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISPHYSXAPI
ISTATUS
PhysxHitAllocatorAllocateWithHitPoint(
    _Inout_ PPHYSX_HIT_ALLOCATOR HitAllocator,
    _In_opt_ PHIT_LIST NextHit,
    _In_ FLOAT Distance,
    _In_ UINT32 FrontFace,
    _In_ UINT32 BackFace,
    _In_reads_bytes_opt_(AdditionalDataSizeInBytes) PCVOID AdditionalData,
    _In_ SIZE_T AdditionalDataSizeInBytes,
    _In_ SIZE_T AdditionalDataAlignment,
    _In_ POINT3 HitPoint,
    _Out_ PHIT_LIST *HitList
    );

#endif // _IRIS_PHYSX_HIT_ALLOCATOR_
