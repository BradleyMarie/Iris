/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    shape.c

Abstract:

    This module declares the shape vtable init function

--*/

#include <irisp.h>

VOID
ShapeInitializeVTable(
    _Out_ PSHAPE_VTABLE VTable,
    _In_ PTRACE_ROUTINE TraceRoutine
    )
{
    VTable->GeometryRoutines.GeometryTraceRoutine = GeometryTraceShapeCallback;
    VTable->TraceRoutine = TraceRoutine;
}