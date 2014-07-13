/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    iris_shape.h

Abstract:

    This file contains the implementation of the internal shape functions.

--*/

#ifndef _IRIS_SHAPE_INTERNAL_
#define _IRIS_SHAPE_INTERNAL_

#include <irisp.h>

//
// Types
//

struct _SHAPE {
    PCSHAPE_VTABLE VTable;
    SIZE_T ReferenceCount;
    PVOID Data;
};

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS 
ShapeTraceShape(
    _In_ PCSHAPE Shape, 
    _In_ RAY Ray,
    _Inout_ PSHAPE_HIT_ALLOCATOR ShapeHitAllocator,
    _Outptr_result_maybenull_ PSHAPE_HIT_LIST *ShapeHitList
    )
{
    ISTATUS Status;

    ASSERT(Shape != NULL);
    ASSERT(ShapeHitAllocator != NULL);
    ASSERT(ShapeHitList != NULL);

    ShapeHitAllocatorSetCurrentShape(ShapeHitAllocator, Shape);

    Status = Shape->VTable->TraceRoutine(Shape->Data, 
                                         Ray,
                                         ShapeHitAllocator,
                                         ShapeHitList);

    return Status;
}

#endif // _IRIS_SHAPE_INTERNAL_