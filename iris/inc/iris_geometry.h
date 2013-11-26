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

//
// Types
//

typedef struct _GEOMETRY_VTABLE {
    PVOID GeometryTraceRoutine;
} GEOMETRY_VTABLE, *PGEOMETRY_VTABLE;

typedef struct _GEOMETRY {
    PGEOMETRY_VTABLE VTable;
} GEOMETRY, *PGEOMETRY;

typedef struct _PREMULTIPLIED_GEOMETRY {
    PGEOMETRY_VTABLE VTable;
    PMATRIX ModelToWorld;
    PSHAPE Shape;
} PREMULTIPLIED_GEOMETRY, *PPREMULTIPLIED_GEOMETRY;

typedef struct _MODEL_GEOMETRY {
    PGEOMETRY_VTABLE VTable;
    PMATRIX WorldToModel;
    PSHAPE Shape;
} MODEL_GEOMETRY, *PMODEL_GEOMETRY;

//
// Function definitions
//

VOID
GeometryInitializeModelGeometry(
    _Out_ PMODEL_GEOMETRY Geometry,
    _In_ PMATRIX WorldToModel,
    _In_ PSHAPE Shape
    );

VOID
GeometryInitializePremultipliedGeometry(
    _Out_ PPREMULTIPLIED_GEOMETRY Geometry,
    _In_ PMATRIX ModelToWorld,
    _In_ PSHAPE Shape
    );

#endif // _IRIS_SHAPE_