/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    iris_shapehitallocator.h

Abstract:

    This module implements the SHAPE_HIT_ALLOCATOR public functions.

--*/

#ifndef _IRIS_SHAPE_HIT_ALLOCATOR_
#define _IRIS_SHAPE_HIT_ALLOCATOR_

#include <iris.h>

//
// Types
//

typedef struct _SHAPE_REFERENCE SHAPE_REFERENCE, *PSHAPE_REFERENCE;
typedef CONST SHAPE_REFERENCE *PCSHAPE_REFERENCE;

typedef struct _SHAPE_HIT_ALLOCATOR SHAPE_HIT_ALLOCATOR, *PSHAPE_HIT_ALLOCATOR;
typedef CONST SHAPE_HIT_ALLOCATOR *PCSHAPE_HIT_ALLOCATOR;

typedef struct _SHAPE_HIT {
    PCSHAPE_REFERENCE ShapeReference;
    FLOAT Distance;
    INT32 FaceHit;
    _Field_size_bytes_opt_(AdditionalDataSizeInBytes) PCVOID AdditionalData;
    SIZE_T AdditionalDataSizeInBytes;
} SHAPE_HIT, *PSHAPE_HIT;

typedef CONST SHAPE_HIT *PCSHAPE_HIT;

typedef struct _SHAPE_HIT_LIST {
    struct _SHAPE_HIT_LIST *NextShapeHit;
    PCSHAPE_HIT ShapeHit;
} SHAPE_HIT_LIST, *PSHAPE_HIT_LIST;

typedef CONST SHAPE_HIT_LIST *PCSHAPE_HIT_LIST;

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISAPI
ISTATUS
ShapeHitAllocatorAllocate(
    _Inout_ PSHAPE_HIT_ALLOCATOR ShapeHitAllocator,
    _In_opt_ PSHAPE_HIT_LIST NextShapeHit,
    _In_ FLOAT Distance,
    _In_ INT32 FaceHit,
    _In_reads_bytes_opt_(AdditionalDataSizeInBytes) PCVOID AdditionalData,
    _In_ SIZE_T AdditionalDataSizeInBytes,
    _In_ SIZE_T AdditionalDataAlignment,
    _Out_ PSHAPE_HIT_LIST *ShapeHitList
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISAPI
ISTATUS
ShapeHitAllocatorAllocateWithHitPoint(
    _Inout_ PSHAPE_HIT_ALLOCATOR ShapeHitAllocator,
    _In_opt_ PSHAPE_HIT_LIST NextShapeHit,
    _In_ FLOAT Distance,
    _In_ INT32 FaceHit,
    _In_reads_bytes_opt_(AdditionalDataSizeInBytes) PCVOID AdditionalData,
    _In_ SIZE_T AdditionalDataSizeInBytes,
    _In_ SIZE_T AdditionalDataAlignment,
    _In_ POINT3 HitPoint,
    _Out_ PSHAPE_HIT_LIST *ShapeHitList
    );

#endif // _IRIS_SHAPE_HIT_ALLOCATOR_