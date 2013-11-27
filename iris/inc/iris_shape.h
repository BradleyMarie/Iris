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

typedef ISTATUS (*PTRACE_ROUTINE)(_In_ PVOID Context, 
                                  _In_ PRAY Ray,
                                  _Inout_ PSHAPE_HIT_LIST ShapeHitList);

typedef struct _SHAPE_VTABLE {
    GEOMETRY_VTABLE GeometryRoutines;
    PTRACE_ROUTINE TraceRoutine;
} SHAPE_VTABLE, *PSHAPE_VTABLE;

typedef struct _SHAPE {
    PSHAPE_VTABLE VTable;
};

//
// Function definitions
//

VOID
ShapeInitializeVTable(
    _Out_ PSHAPE_VTABLE VTable,
    _In_ PTRACE_ROUTINE TraceRoutine
    );

#endif // _IRIS_SHAPE_