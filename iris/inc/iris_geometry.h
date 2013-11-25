/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    iris_geometry.h

Abstract:

    This file contains the implementation of the geometry base object.

--*/

#ifndef _IRIS_SHAPE_
#define _IRIS_SHAPE_

#include <iris.h>

#define GEOMETRY_WORLD_GEOMETRY 0
#define GEOMETRY_PREMULTIPLIED_GEOMETRY 1
#define GEOMETRY_MODEL_GEOMETRY 2

typedef struct _WORLD_GEOMETRY {
    SHAPE_VTABLE VTable;
} MODEL_GEOMETRY, *PMODEL_GEOMETRY;

typedef struct _PREMULTIPLIED_GEOMETRY {
    SHAPE_VTABLE VTable;
    PMATRIX ModelToWorld;
} MODEL_GEOMETRY, *PMODEL_GEOMETRY;

typedef struct _MODEL_GEOMETRY {
    SHAPE_VTABLE VTable;
    PMATRIX WorldToModel;
} MODEL_GEOMETRY, *PMODEL_GEOMETRY;

typedef ISTATUS (*PTRACE_ROUTINE)(_In_ PVOID Context, 
                                  _In_ PRAY Ray,
                                  _Inout_ PSHAPE_HIT_LIST ShapeHitList);

typedef struct _GEOMETRY_VTABLE GEOMETRY_VTABLE, *PGEOMETRY_VTABLE;

PGEOMETRY_VTABLE
GeometryCreateVTable(
    _In_ PFREE_ROUTINE FreeRoutine,
    _In_ PTRACE_ROUTINE TraceRoutine,
    _In_ UINT8 GeometryType
    );



typedef struct _SHAPE_VTABLE SHAPE_VTABLE, *PSHAPE_VTABLE;
typedef struct _SHAPE_BASE SHAPE_BASE, *PSHAPE_BASE;

GeometryCreateModelGeometry(
    PTRACE_ROUTINE
    )

PSHAPE_VTABLE
ShapeCreateVTable(
    _In_ PFREE_ROUTINE FreeRoutine,
    _In_ PTRACE_ROUTINE TraceRoutine
    );

PSHAPE_BASE
ShapeCreate(
    _In_ PSHAPE_VTABLE ShapeVTable,
    _In_ SIZE_T ObjectSize
    );

#endif // _IRIS_SHAPE_