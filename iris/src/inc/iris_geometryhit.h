/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    iris_geometryhit.h

Abstract:

    This module declares the internal geometry hit type.

--*/

#ifndef _IRIS_GEOMETRY_HIT_INTERNAL_
#define _IRIS_GEOMETRY_HIT_INTERNAL_

#include <irisp.h>

//
// Types
//

typedef UINT8 GEOMETRY_TYPE;
typedef GEOMETRY_TYPE *PGEOMETRY_TYPE;

struct _SHARED_GEOMETRY_HIT {
    GEOMETRY_TYPE Type;
    PMATRIX ModelToWorld;
    RAY ModelRay;
};

//
// Defines
//

#define GEOMETRY_TYPE_WORLD         0x0
#define GEOMETRY_TYPE_PREMULTIPLIED 0x1
#define GEOMETRY_TYPE_MODEL         0x2

#endif // _IRIS_GEOMETRY_HIT_INTERNAL_