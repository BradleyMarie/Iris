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

typedef struct _SHARED_GEOMETRY_HIT SHARED_GEOMETRY_HIT, *PSHARED_GEOMETRY_HIT;

typedef struct _GEOMETRY_HIT {
    SHAPE_HIT ShapeHit;
    PSHARED_GEOMETRY_HIT SharedGeometryHit;
} GEOMETRY_HIT, *PGEOMETRY_HIT;

//
// Functions
//

VOID
GeometryHitComputeModelViewer(
    _In_ PGEOMETRY_HIT GeometryHit,
    _In_ PVECTOR3 WorldViewer,
    _Out_ PVECTOR3 ModelViewer
    );

VOID
GeometryHitComputeModelHit(
    _In_ PGEOMETRY_HIT GeometryHit,
    _In_ PPOINT3 WorldHitPoint,
    _Out_ PPOINT3 ModelHitPoint
    );

PMATRIX
GeometryHitGetModelToWorld(
    _In_ PGEOMETRY_HIT GeometryHit
    );

#endif // _IRIS_GEOMETRY_HIT_