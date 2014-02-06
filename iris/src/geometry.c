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
    _In_ PCSHAPE Shape,
    _In_ PCINVERTIBLE_MATRIX ModelToWorld,
    _In_ BOOL Premultiplied
    )
{
    PGEOMETRY Geometry;

    ASSERT(Shape != NULL);

    Geometry = (PGEOMETRY) malloc(sizeof(GEOMETRY));

    if (Geometry == NULL)
    {
        return NULL;
    }

    Geometry->Shape = Shape;
    Geometry->ModelToWorld = ModelToWorld;
    Geometry->Premultiplied = Premultiplied;

    return Geometry;
}

VOID
GeometryFree(
    _Pre_maybenull_ _Post_invalid_ PGEOMETRY Geometry
    )
{
    free(Geometry);
}