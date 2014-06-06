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
    _In_ PCVOID Context, 
    _In_ RAY Ray,
    _Inout_ PSHAPE_HIT_ALLOCATOR ShapeHitAllocator,
    _Outptr_result_maybenull_ PSHAPE_HIT_LIST *ShapeHitList
    );

typedef struct _SHAPE_VTABLE {
    PSHAPE_TRACE_ROUTINE TraceRoutine;
} SHAPE_VTABLE, *PSHAPE_VTABLE;

typedef CONST SHAPE_VTABLE *PCSHAPE_VTABLE;

struct _SHAPE {
    PCSHAPE_VTABLE VTable;
};

#endif // _IRIS_SHAPE_