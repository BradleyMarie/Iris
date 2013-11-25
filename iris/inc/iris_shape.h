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

typedef struct _SHAPE_VTABLE {
    IRIS_OBJECT_VTABLE VTableFreeRoutines;
    IRIS_OBJECT_VTABLE SHape
    ISTATUS (*TraceRoutine)(_In_ PRAY, _Inout_ PSHAPE_HIT_LIST);
    PVOID ReservedForIris;
    SIZE_T ContextSize;
} SHAPE_VTABLE, *PSHAPE_VTABLE;

PSHAPE_VTABLE
ShapeCreateVTable(
    VOID (*FreeRoutine)(_Pre_maybenull_ _Post_invalid_ PVOID),
    ISTATUS (*TraceRoutine)(_In_ PRAY, _Inout_ PSHAPE_HIT_LIST),
    SIZE_T ContextSize
    );


typedef struct _SHAPE_BASE {
    PSHAPE_VTABLE ShapeVTable;
} SHAPE_BASE, *PSHAPE_BASE;

PSHAPE
ShapeCreate(
    PSHAPE_VTABLE ShapeVTable,
    PVOID Context,
    SIZE_T ContextSize
    );

#endif // _IRIS_SHAPE_