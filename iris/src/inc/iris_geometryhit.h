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
    PCMATRIX ModelToWorld;
    RAY ModelRay;
};

//
// Defines
//

#define GEOMETRY_TYPE_WORLD         0x0
#define GEOMETRY_TYPE_PREMULTIPLIED 0x1
#define GEOMETRY_TYPE_MODEL         0x2

//
// Function Definitions
//

SFORCEINLINE
VOID
GeometryHitInitialize(
    _Out_ PGEOMETRY_HIT GeometryHit,
    _In_ PCSHARED_GEOMETRY_HIT SharedGeometryHit,
    _In_ PCSHAPE_HIT ShapeHit
    )
{
    ASSERT(GeometryHit != NULL);
    ASSERT(SharedGeometryHit != NULL);
    ASSERT(ShapeHit != NULL);

    GeometryHit->SharedGeometryHit = SharedGeometryHit;
    GeometryHit->Shape = ShapeHit->Shape;
    GeometryHit->Distance = ShapeHit->Distance;
    GeometryHit->FaceHit = ShapeHit->FaceHit;
    GeometryHit->AdditionalData = ShapeHit->AdditionalData;
    GeometryHit->AdditionalDataSizeInBytes = ShapeHit->AdditionalDataSizeInBytes;
}

SFORCEINLINE
COMPARISON_RESULT
GeometryHitCompare(
    _In_ PCVOID GeometryHit0,
    _In_ PCVOID GeometryHit1
    )
{
    PGEOMETRY_HIT Hit0;
    PGEOMETRY_HIT Hit1;

    ASSERT(GeometryHit0 != NULL);
    ASSERT(GeometryHit1 != NULL);

    Hit0 = (PGEOMETRY_HIT) GeometryHit0;
    Hit1 = (PGEOMETRY_HIT) GeometryHit1;

    return (Hit0->Distance <= Hit1->Distance) ? -1 : 1;
}

#endif // _IRIS_GEOMETRY_HIT_INTERNAL_