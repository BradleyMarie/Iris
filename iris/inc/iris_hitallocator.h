/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    iris_hitallocator.h

Abstract:

    This module implements the HIT_ALLOCATOR public functions.

--*/

#ifndef _IRIS_HIT_ALLOCATOR_
#define _IRIS_HIT_ALLOCATOR_

#include <iris.h>

//
// Types
//

typedef struct _HIT_ALLOCATOR HIT_ALLOCATOR, *PHIT_ALLOCATOR;
typedef CONST HIT_ALLOCATOR *PCHIT_ALLOCATOR;

typedef struct _HIT {
    PCVOID Data;
    FLOAT Distance;
    INT32 FaceHit;
    _Field_size_bytes_opt_(AdditionalDataSizeInBytes) PCVOID AdditionalData;
    SIZE_T AdditionalDataSizeInBytes;
} HIT, *PHIT;

typedef CONST HIT *PCHIT;

typedef struct _HIT_LIST {
    struct _HIT_LIST *NextHit;
    PCHIT Hit;
} HIT_LIST, *PHIT_LIST;

typedef CONST HIT_LIST *PCHIT_LIST;

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISAPI
ISTATUS
HitAllocatorAllocate(
    _Inout_ PHIT_ALLOCATOR HitAllocator,
    _In_opt_ PHIT_LIST NextHit,
    _In_ PCVOID Data,
    _In_ FLOAT Distance,
    _In_ INT32 FaceHit,
    _When_(AdditionalDataSizeInBytes != 0, _In_reads_bytes_opt_(AdditionalDataSizeInBytes)) PCVOID AdditionalData,
    _In_ SIZE_T AdditionalDataSizeInBytes,
    _When_(AdditionalDataSizeInBytes != 0, _Pre_satisfies_(_Curr_ != 0 && (_Curr_ & (_Curr_ - 1)) == 0 && AdditionalDataSizeInBytes % _Curr_ == 0)) SIZE_T AdditionalDataAlignment,
    _Out_ PHIT_LIST *HitList
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISAPI
ISTATUS
HitAllocatorAllocateWithHitPoint(
    _Inout_ PHIT_ALLOCATOR HitAllocator,
    _In_opt_ PHIT_LIST NextHit,
    _In_ PCVOID Data,
    _In_ FLOAT Distance,
    _In_ INT32 FaceHit,
    _When_(AdditionalDataSizeInBytes != 0, _In_reads_bytes_opt_(AdditionalDataSizeInBytes)) PCVOID AdditionalData,
    _In_ SIZE_T AdditionalDataSizeInBytes,
    _When_(AdditionalDataSizeInBytes != 0, _Pre_satisfies_(_Curr_ != 0 && (_Curr_ & (_Curr_ - 1)) == 0 && AdditionalDataSizeInBytes % _Curr_ == 0)) SIZE_T AdditionalDataAlignment,
    _In_ POINT3 HitPoint,
    _Out_ PHIT_LIST *HitList
    );

#endif // _IRIS_SHAPE_HIT_ALLOCATOR_