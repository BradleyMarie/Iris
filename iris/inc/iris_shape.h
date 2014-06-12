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
    PFREE_ROUTINE FreeRoutine;
} SHAPE_VTABLE, *PSHAPE_VTABLE;

typedef CONST SHAPE_VTABLE *PCSHAPE_VTABLE;

//
// Functions
//

_Check_return_
_Ret_maybenull_
IRISAPI
PSHAPE
ShapeAllocate(
    _In_ PCSHAPE_VTABLE ShapeVTable,
    _In_reads_bytes_(DataSizeInBytes)PCVOID Data,
    _In_ SIZE_T DataSizeInBytes,
    _In_ SIZE_T DataAlignment
    );

_Ret_
IRISAPI
PCVOID
ShapeGetData(
    _In_ PCSHAPE Shape
    );

_Ret_
IRISAPI
PCSHAPE_VTABLE
ShapeGetVTable(
    _In_ PCSHAPE Shape
    );

IRISAPI
VOID
ShapeReference(
    _In_opt_ PSHAPE Shape
    );

IRISAPI
VOID
ShapeDereference(
    _In_opt_ _Post_invalid_ PSHAPE Shape
    );

#endif // _IRIS_SHAPE_