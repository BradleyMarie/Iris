/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    iris_shape.h

Abstract:

    This file contains the implementation of the base shape object.

--*/

#ifndef _IRIS_SHAPE_
#define _IRIS_SHAPE_

#include <iris.h>

//
// Types
//

typedef
_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS 
(*PSHAPE_TRACE_ROUTINE)(
    _In_ PVOID Context, 
    _In_ PRAY Ray,
    _Inout_ PSHAPE_HIT_ALLOCATOR ShapeHitAllocator,
    _Outptr_result_maybenull_ PSHAPE_HIT *HitList
    );

typedef struct _SHAPE_VTABLE {
    PSHAPE_TRACE_ROUTINE TraceRoutine;
} SHAPE_VTABLE, *PSHAPE_VTABLE;

struct _SHAPE {
    PSHAPE_VTABLE VTable;
};

//
// Function definitions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS 
ShapeTraceShape(
    _In_ PSHAPE Shape, 
    _In_ PRAY Ray,
    _Inout_ PSHAPE_HIT_ALLOCATOR ShapeHitAllocator,
    _Outptr_result_maybenull_ PSHAPE_HIT *HitList
    )
{
    ISTATUS Status;

    ASSERT(Shape != NULL);
    ASSERT(Ray != NULL);
    ASSERT(ShapeHitAllocator != NULL);
    ASSERT(HitList != NULL);

    Status = Shape->VTable->TraceRoutine(Shape, 
                                         Ray,
                                         ShapeHitAllocator,
                                         HitList);

    return Status;
}

#endif // _IRIS_SHAPE_