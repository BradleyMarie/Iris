/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    irisadvaced_boundedshape.h

Abstract:

    This file contains the definitions for the BOUNDED_SHAPE base type.

--*/

#ifndef _BOUNDED_SHAPE_IRIS_ADVANCED_
#define _BOUNDED_SHAPE_IRIS_ADVANCED_

#include <irisadvanced.h>

//
// Types
//

typedef
_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS 
(*PBOUNDED_SHAPE_CHECK_BOUNDS)(
    _In_ PCVOID Context,
    _In_ PCMATRIX ModelToWorld,
    _In_ BOUNDING_BOX WorldAlignedBoundingBox,
    _Out_ PBOOL IsInsideBox
    );

typedef struct _BOUNDED_SHAPE_VTABLE {
    SHAPE_VTABLE ShapeVTable;
    PBOUNDED_SHAPE_CHECK_BOUNDS CheckBoundsRoutine;
} BOUNDED_SHAPE_VTABLE, *PBOUNDED_SHAPE_VTABLE;

typedef CONST BOUNDED_SHAPE_VTABLE *PCBOUNDED_SHAPE_VTABLE;

typedef struct _BOUNDED_SHAPE BOUNDED_SHAPE, *PBOUNDED_SHAPE;
typedef CONST BOUNDED_SHAPE *PCBOUNDED_SHAPE;

//
// Functions
//

_Check_return_
_Ret_maybenull_
IRISADVANCEDAPI
PBOUNDED_SHAPE
BoundedShapeAllocate(
    _In_ PCBOUNDED_SHAPE_VTABLE BoundedShapeVTable,
    _In_reads_bytes_(DataSizeInBytes) PCVOID Data,
    _In_ SIZE_T DataSizeInBytes,
    _In_ SIZE_T DataAlignment
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISADVANCEDAPI
ISTATUS 
BoundedShapeCheckBounds(
    _In_ PCBOUNDED_SHAPE BoundedShape,
    _In_ PCMATRIX ModelToWorld,
    _In_ BOUNDING_BOX WorldAlignedBoundingBox,
    _Out_ PBOOL IsInsideBox
    );

IRISADVANCEDAPI
VOID
BoundedShapeReference(
    _In_opt_ PBOUNDED_SHAPE BoundedShape
    );

IRISADVANCEDAPI
VOID
BoundedShapeDereference(
    _In_opt_ _Post_invalid_ PBOUNDED_SHAPE BoundedShape
    );

#endif // _BOUNDED_SHAPE_IRIS_ADVANCED_