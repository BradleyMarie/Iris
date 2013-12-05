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

//
// Functions
//

SFORCEINLINE
VOID
SharedGeometryHitSetWorld(
    _Out_ PSHARED_GEOMETRY_HIT GeometryHit
    )
{
    ASSERT(GeometryHit != NULL);

    GeometryHit->Type = GEOMETRY_TYPE_WORLD;
}

SFORCEINLINE
VOID
SharedGeometryHitSetPremultiplied(
    _Out_ PSHARED_GEOMETRY_HIT GeometryHit,
    _In_ PMATRIX ModelToWorld
    )
{
    ASSERT(GeometryHit != NULL);
    ASSERT(ModelToWorld != NULL);

    GeometryHit->Type = GEOMETRY_TYPE_PREMULTIPLIED;
    GeometryHit->ModelToWorld = ModelToWorld;
}

SFORCEINLINE
VOID
SharedGeometryHitSetModel(
    _Out_ PSHARED_GEOMETRY_HIT GeometryHit,
    _In_ PMATRIX ModelToWorld,
    _In_ PRAY ModelRay
    )
{
    ASSERT(GeometryHit != NULL);
    ASSERT(ModelToWorld != NULL);
    ASSERT(ModelRay != NULL);

    GeometryHit->Type = GEOMETRY_TYPE_WORLD;
    GeometryHit->ModelToWorld = ModelToWorld;
    GeometryHit->ModelRay = *ModelRay;
}

#endif // _IRIS_GEOMETRY_HIT_INTERNAL_