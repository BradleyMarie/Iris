/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    bounding_box.h

Abstract:

    An axis-aligned bounding box.

--*/

#ifndef _IRIS_ADVANCED_BOUNDING_BOX_
#define _IRIS_ADVANCED_BOUNDING_BOX_

#include "iris/iris.h"

//
// Types
//

typedef struct _BOUNDING_BOX {
    POINT3 corners[2];
} BOUNDING_BOX, *PBOUNDING_BOX;

typedef const BOUNDING_BOX *PCBOUNDING_BOX;

//
// Functions
//

static
inline
BOUNDING_BOX
BoundingBoxCreate(
    _In_ POINT3 corner0,
    _In_ POINT3 corner1
    )
{
    assert(corner0.x <= corner1.x);
    assert(corner0.y <= corner1.y);
    assert(corner0.z <= corner1.z);

    BOUNDING_BOX result;
    result.corners[0] = corner0;
    result.corners[1] = corner1;

    return result;
}

static
inline
bool
BoundingBoxValidate(
    _In_ BOUNDING_BOX box
    )
{
    if (!PointValidate(box.corners[0]) ||
        !PointValidate(box.corners[1]) ||
        box.corners[1].x < box.corners[0].x ||
        box.corners[1].y < box.corners[0].y ||
        box.corners[1].z < box.corners[0].z)
    {
        return false;
    }
    
    return true;
}

static
inline
BOUNDING_BOX
BoundingBoxEnvelop(
    _In_ BOUNDING_BOX box,
    _In_ POINT3 point
    )
{
    float_t min_x = fmin(box.corners[0].x, point.x);
    float_t min_y = fmin(box.corners[0].y, point.y);
    float_t min_z = fmin(box.corners[0].z, point.z);
    POINT3 corner0 = PointCreate(min_x, min_y, min_z);

    float_t max_x = fmax(box.corners[1].x, point.x);
    float_t max_y = fmax(box.corners[1].y, point.y);
    float_t max_z = fmax(box.corners[1].z, point.z);
    POINT3 corner1 = PointCreate(max_x, max_y, max_z);

    return BoundingBoxCreate(corner0, corner1);
}

static
inline
BOUNDING_BOX
BoundingBoxTransform(
    _In_opt_ PCMATRIX matrix,
    _In_ BOUNDING_BOX box
    )
{
    POINT3 transformed = PointMatrixMultiply(matrix, box.corners[0]);
    BOUNDING_BOX result = BoundingBoxCreate(transformed, transformed);

    for (uint32_t i = 1; i < 8; i++)
    {
        POINT3 corner = PointCreate(box.corners[(i >> 0) & 1u].x,
                                    box.corners[(i >> 1) & 1u].y,
                                    box.corners[(i >> 2) & 1u].z);
        transformed = PointMatrixMultiply(matrix, corner);
        result = BoundingBoxEnvelop(result, transformed);
    }

    return result;
}

static
inline
bool
BoundingBoxOverlaps(
    _In_ BOUNDING_BOX box0,
    _In_ BOUNDING_BOX box1
    )
{
    bool overlaps = box0.corners[0].x <= box1.corners[1].x &&
                    box1.corners[0].x <= box0.corners[1].x &&
                    box0.corners[0].y <= box1.corners[1].y &&
                    box1.corners[0].y <= box0.corners[1].y &&
                    box0.corners[0].z <= box1.corners[1].z &&
                    box1.corners[0].z <= box0.corners[1].z;

    return overlaps;
}

#endif // _IRIS_ADVANCED_BOUNDING_BOX_