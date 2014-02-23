/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    iris_geometry.h

Abstract:

    This file contains the internal definitions for the GEOMETRY type.

--*/

#ifndef _GEOMETRY_IRIS_INTERNAL_
#define _GEOMETRY_IRIS_INTERNAL_

#include <irisp.h>

//
// Types
//

struct _GEOMETRY {
    PCSHAPE Shape;
    PCINVERTIBLE_MATRIX ModelToWorld;
    BOOL Premultiplied;
};

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS 
GeometryTraceGeometry(
    _In_ PCGEOMETRY Geometry, 
    _In_ PCRAY WorldRay,
    _Inout_ PSHAPE_HIT_ALLOCATOR ShapeHitAllocator,
    _Inout_ PSHARED_GEOMETRY_HIT_ALLOCATOR SharedGeometryHitAllocator,
    _Outptr_result_maybenull_ PCSHARED_GEOMETRY_HIT *SharedGeometryHit,
    _Outptr_result_maybenull_ PSHAPE_HIT_LIST *ShapeHitList
    )
{
    PSHARED_GEOMETRY_HIT GeometryHit;
    ISTATUS Status;
    PCRAY TraceRay;

    ASSERT(Geometry != NULL);
    ASSERT(WorldRay != NULL);
    ASSERT(ShapeHitAllocator != NULL);
    ASSERT(SharedGeometryHitAllocator != NULL);
    ASSERT(SharedGeometryHit != NULL);
    ASSERT(ShapeHitList != NULL);

    GeometryHit = SharedGeometryHitAllocatorAllocate(SharedGeometryHitAllocator);

    if (GeometryHit == NULL)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    if (Geometry->ModelToWorld == NULL)
    {
        GeometryHit->Type = GEOMETRY_TYPE_WORLD;
        TraceRay = WorldRay;
    }
    else if (Geometry->Premultiplied != FALSE)
    {
        GeometryHit->Type = GEOMETRY_TYPE_PREMULTIPLIED;
        GeometryHit->ModelToWorld = &Geometry->ModelToWorld->Matrix;
        TraceRay = WorldRay;
    }
    else
    {
        GeometryHit->Type = GEOMETRY_TYPE_MODEL;
        GeometryHit->ModelToWorld = &Geometry->ModelToWorld->Matrix;

        RayMatrixMultiply(&Geometry->ModelToWorld->Inverse,
                          WorldRay,
                          &GeometryHit->ModelRay);

        TraceRay = &GeometryHit->ModelRay;
    }

    Status = ShapeTraceShape(Geometry->Shape,
                             TraceRay,
                             ShapeHitAllocator,
                             ShapeHitList);

    if (Status != ISTATUS_SUCCESS)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    if (*ShapeHitList == NULL)
    {
        SharedGeometryHitAllocatorFreeLastAllocation(SharedGeometryHitAllocator);
        return ISTATUS_SUCCESS;
    }

    *SharedGeometryHit = GeometryHit;

    return ISTATUS_SUCCESS;
}

#endif // _GEOMETRY_IRIS_INTERNAL_