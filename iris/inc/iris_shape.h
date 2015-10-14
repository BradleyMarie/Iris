/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    iris_shape.h

Abstract:

    This file contains the implementation of the base shape object.

--*/

#include <iris.h>

#ifndef _IRIS_SHAPE_
#define _IRIS_SHAPE_

//
// Types
//

typedef
_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS 
(*PSHAPE_TRACE_ROUTINE)(
    _In_opt_ PCVOID Context, 
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
_Success_(return == ISTATUS_SUCCESS)
IRISAPI
ISTATUS
ShapeAllocate(
    _In_ PCSHAPE_VTABLE ShapeVTable,
    _When_(DataSizeInBytes != 0, _In_reads_bytes_opt_(DataSizeInBytes)) _When_(DataSizeInBytes != 0, _Reserved_) PCVOID Data,
    _When_(DataSizeInBytes != 0, _Pre_satisfies_(DataSizeInBytes % DataAlignment == 0)) SIZE_T DataSizeInBytes,
    _When_(DataSizeInBytes != 0, _Pre_satisfies_((DataAlignment & (DataAlignment - 1)) == 0)) SIZE_T DataAlignment,
    _Out_ PSHAPE *Shape
    );

_Ret_
IRISAPI
PCSHAPE_VTABLE
ShapeGetVTable(
    _In_ PCSHAPE Shape
    );

_Ret_
IRISAPI
PCVOID
ShapeGetData(
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