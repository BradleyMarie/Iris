/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    iris_geometry.h

Abstract:

    This file contains the definitions for the GEOMETRY type.

--*/

#ifndef _GEOMETRY_IRIS_
#define _GEOMETRY_IRIS_

#include <iris.h>

//
// Types
//

typedef struct _GEOMETRY GEOMETRY, *PGEOMETRY;
typedef CONST GEOMETRY *PCGEOMETRY;

//
// Functions
//

_Check_return_
_Ret_maybenull_
IRISAPI
PGEOMETRY
GeometryAllocate(
    _In_ PSHAPE Shape,
    _In_opt_ PMATRIX ModelToWorld,
    _In_ BOOL Premultiplied
    );

IRISAPI
VOID
GeometryFree(
    _Pre_maybenull_ _Post_invalid_ PGEOMETRY Geometry
    );

#endif // _GEOMETRY_IRIS_