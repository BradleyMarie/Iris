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
ShapeTestRayInternal(
    _In_ PCSHAPE Shape, 
    _In_ RAY Ray,
    _Inout_ PHIT_ALLOCATOR HitAllocator,
    _Outptr_result_maybenull_ PHIT_LIST *HitList
    )
{
    ISTATUS Status;

    ASSERT(Shape != NULL);
    ASSERT(HitAllocator != NULL);
    ASSERT(HitList != NULL);

    HitAllocatorSetCurrentShape(HitAllocator, Shape);

    Status = Shape->VTable->TestRayRoutine(Shape->Data, 
                                           Ray,
                                           HitAllocator,
                                           HitList);

    return Status;
}

#endif // _IRIS_SHAPE_INTERNAL_