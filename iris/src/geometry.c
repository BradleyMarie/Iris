/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    geometry.c

Abstract:

    This file contains the function definitions for the GEOMETRY type.

--*/

#include <irisp.h>

_Check_return_
_Ret_maybenull_
PGEOMETRY
GeometryAllocate(
    _In_ PSHAPE Shape,
    _In_opt_ PMATRIX ModelToWorld,
    _In_ BOOL Premultiplied
    )
{
    PGEOMETRY Geometry;

    if (Shape == NULL)
    {
        return NULL;
    }

    Geometry = (PGEOMETRY) malloc(sizeof(GEOMETRY));

    if (Geometry == NULL)
    {
        return NULL;
    }

    Geometry->Shape = Shape;
    Geometry->ModelToWorld = ModelToWorld;
    Geometry->Premultiplied = Premultiplied;

    MatrixReference(ModelToWorld);
    ShapeReference(Shape);

    return Geometry;
}

VOID
GeometryFree(
    _In_opt_ _Post_invalid_ PGEOMETRY Geometry
    )
{
    if (Geometry == NULL)
    {
        return;
    }

    MatrixDereference(Geometry->ModelToWorld);
    ShapeDereference(Geometry->Shape);
    free(Geometry);
}