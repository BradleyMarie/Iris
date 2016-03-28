/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    irisphysx_hitallocator.h

Abstract:

    This module implements the PBR_HIT_ALLOCATOR public functions.

--*/

#ifndef _IRIS_PHYSX_HIT_ALLOCATOR_
#define _IRIS_PHYSX_HIT_ALLOCATOR_

#include <iris.h>

//
// Types
//

typedef struct _PBR_GEOMETRY PBR_GEOMETRY, *PPBR_GEOMETRY;
typedef CONST PBR_GEOMETRY *PCPBR_GEOMETRY;

typedef struct _PBR_HIT_ALLOCATOR PBR_HIT_ALLOCATOR, *PPBR_HIT_ALLOCATOR;
typedef CONST PBR_HIT_ALLOCATOR *PCPBR_HIT_ALLOCATOR;

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISPHYSXAPI
ISTATUS
PBRHitAllocatorAllocate(
    _Inout_ PPBR_HIT_ALLOCATOR PBRHitAllocator,
    _In_opt_ PHIT_LIST NextHit,
    _In_ FLOAT Distance,
    _In_ INT32 FaceHit,
    _In_reads_bytes_opt_(AdditionalDataSizeInBytes) PCVOID AdditionalData,
    _In_ SIZE_T AdditionalDataSizeInBytes,
    _In_ SIZE_T AdditionalDataAlignment,
    _Out_ PHIT_LIST *HitList
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISPHYSXAPI
ISTATUS
PBRHitAllocatorAllocateWithHitPoint(
    _Inout_ PPBR_HIT_ALLOCATOR PBRHitAllocator,
    _In_opt_ PHIT_LIST NextHit,
    _In_ FLOAT Distance,
    _In_ INT32 FaceHit,
    _In_reads_bytes_opt_(AdditionalDataSizeInBytes) PCVOID AdditionalData,
    _In_ SIZE_T AdditionalDataSizeInBytes,
    _In_ SIZE_T AdditionalDataAlignment,
    _In_ POINT3 HitPoint,
    _Out_ PHIT_LIST *HitList
    );

#endif // _IRIS_PHYSX_HIT_ALLOCATOR_