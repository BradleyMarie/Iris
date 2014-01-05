/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    iris_raytracer.h

Abstract:

    This file contains the internal definitions for the RAYTRACER type.

--*/

#ifndef _RAYTRACER_IRIS_INTERNAL_
#define _RAYTRACER_IRIS_INTERNAL_

#include <irisp.h>

//
// Types
//

struct _RAYTRACER {
    SHAPE_HIT_ALLOCATOR ShapeHitAllocator;
    SHARED_GEOMETRY_HIT_ALLOCATOR SharedGeometryHitAllocator;
    GEOMETRY_HIT_ALLOCATOR GeometryHitAllocator;
    IRIS_CONSTANT_POINTER_LIST HitList;
};

#endif // _RAYTRACER_IRIS_INTERNAL_