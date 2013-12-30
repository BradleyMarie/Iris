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

typedef struct _SHAPE SHAPE, *PSHAPE;

typedef struct _SHAPE_HIT_ALLOCATOR SHAPE_HIT_ALLOCATOR, *PSHAPE_HIT_ALLOCATOR;

typedef struct _SHAPE_HIT {
    struct _SHAPE_HIT *NextHit;
    PSHAPE Shape;
    FLOAT Distance;
    INT32 FaceHit;
    _Field_size_bytes_opt_(AdditionalDataSizeInBytes) PVOID AdditionalData;
    SIZE_T AdditionalDataSizeInBytes;
} SHAPE_HIT, *PSHAPE_HIT;

//
// Functions
//

_Check_return_
_Ret_maybenull_
PSHAPE_HIT
ShapeHitAllocatorAllocate(
    _Inout_ PSHAPE_HIT_ALLOCATOR ShapeHitCollection,
    _In_ PSHAPE_HIT NextHit,
    _In_ PSHAPE Shape,
    _In_ FLOAT Distance,
    _In_ INT32 FaceHit,
    _In_ SIZE_T AdditionalDataSize
    );

#endif // _IRIS_SHAPE_HIT_ALLOCATOR_