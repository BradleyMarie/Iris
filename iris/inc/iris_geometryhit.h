/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    iris_geometryhit.h

Abstract:

    This module defines the GEOMETRY_HIT struct.

--*/

#ifndef _IRIS_GEOMETRY_HIT_
#define _IRIS_GEOMETRY_HIT_

//
// Types
//

typedef struct _GEOMETRY_HIT {
    PCSHAPE_HIT ShapeHit;
    POINT3 WorldHitPoint;
    POINT3 ModelHitPoint;
    VECTOR3 ModelViewer;
    PCMATRIX ModelToWorld;
} GEOMETRY_HIT, *PGEOMETRY_HIT;

typedef CONST GEOMETRY_HIT *PCGEOMETRY_HIT;

#endif // _IRIS_GEOMETRY_HIT_