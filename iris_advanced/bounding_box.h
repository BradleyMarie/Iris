/*++

Copyright (c) 2021 Brad Weinberger

Module Name:

    bounding_box.h

Abstract:

    An axis-aligned bounding box. Note, the ray-box intersection may return some
    false positives and should not be used in cases where absolute accuracy is
    required.

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
    float_t min_x = IMin(box.corners[0].x, point.x);
    float_t min_y = IMin(box.corners[0].y, point.y);
    float_t min_z = IMin(box.corners[0].z, point.z);
    POINT3 corner0 = PointCreate(min_x, min_y, min_z);

    float_t max_x = IMax(box.corners[1].x, point.x);
    float_t max_y = IMax(box.corners[1].y, point.y);
    float_t max_z = IMax(box.corners[1].z, point.z);
    POINT3 corner1 = PointCreate(max_x, max_y, max_z);

    return BoundingBoxCreate(corner0, corner1);
}

static
inline
BOUNDING_BOX
BoundingBoxUnion(
    _In_ BOUNDING_BOX box0,
    _In_ BOUNDING_BOX box1
    )
{
    BOUNDING_BOX result = BoundingBoxEnvelop(box0, box1.corners[0]);
    result = BoundingBoxEnvelop(result, box1.corners[1]);
    return result;
}

static
inline
BOUNDING_BOX
BoundingBoxIntersection(
    _In_ BOUNDING_BOX box0,
    _In_ BOUNDING_BOX box1
    )
{
    float_t min_x = IMax(box0.corners[0].x, box1.corners[0].x);
    float_t min_y = IMax(box0.corners[0].y, box1.corners[0].y);
    float_t min_z = IMax(box0.corners[0].z, box1.corners[0].z);
    POINT3 corner0 = PointCreate(min_x, min_y, min_z);

    float_t max_x = IMin(box0.corners[1].x, box1.corners[1].x);
    float_t max_y = IMin(box0.corners[1].y, box1.corners[1].y);
    float_t max_z = IMin(box0.corners[1].z, box1.corners[1].z);
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
float_t
BoundingBoxSurfaceArea(
    _In_ BOUNDING_BOX box
    )
{
    VECTOR3 diagonal = PointSubtract(box.corners[1], box.corners[0]);
    
    float_t area0 = diagonal.x * diagonal.z;
    float_t area1 = diagonal.y * diagonal.x;
    float_t area2 = diagonal.z * diagonal.y;
    
    return (float_t)2.0 * (area0 + area1 + area2);
}

static
inline
VECTOR_AXIS
BoundingBoxDominantAxis(
    _In_ BOUNDING_BOX box
    )
{
    VECTOR3 diagonal = PointSubtract(box.corners[1], box.corners[0]);
    return VectorDominantAxis(diagonal);
}

static
inline
bool
BoundingBoxOverlaps(
    _In_ BOUNDING_BOX box0,
    _In_ BOUNDING_BOX box1
    )
{
    bool overlaps = box0.corners[0].x < box1.corners[1].x &&
                    box1.corners[0].x < box0.corners[1].x &&
                    box0.corners[0].y < box1.corners[1].y &&
                    box1.corners[0].y < box0.corners[1].y &&
                    box0.corners[0].z < box1.corners[1].z &&
                    box1.corners[0].z < box0.corners[1].z;

    return overlaps;
}

static
inline
bool
BoundingBoxIntersect(
    _In_ BOUNDING_BOX box,
    _In_ RAY ray,
    _Out_opt_ PVECTOR3 inverse_direction,
    _Out_opt_ float_t *first_hit,
    _Out_opt_ float_t *second_hit
    )
{
    ray.direction.x = (float_t)1.0 / ray.direction.x;
    ray.direction.y = (float_t)1.0 / ray.direction.y;
    ray.direction.z = (float_t)1.0 / ray.direction.z;

    float_t tx1 = (box.corners[0].x - ray.origin.x) * ray.direction.x;
    float_t tx2 = (box.corners[1].x - ray.origin.x) * ray.direction.x;

    float_t min = IMin(tx1, tx2);
    float_t max = IMax(tx1, tx2);

    float_t ty1 = (box.corners[0].y - ray.origin.y) * ray.direction.y;
    float_t ty2 = (box.corners[1].y - ray.origin.y) * ray.direction.y;

    float_t local_min = IMin(ty1, ty2);
    float_t local_max = IMax(ty1, ty2);

    min = IMax(min, local_min);
    max = IMin(max, local_max);

    float_t tz1 = (box.corners[0].z - ray.origin.z) * ray.direction.z;
    float_t tz2 = (box.corners[1].z - ray.origin.z) * ray.direction.z;

    local_min = IMin(tz1, tz2);
    local_max = IMax(tz1, tz2);

    min = IMax(min, local_min);
    max = IMin(max, local_max);

    if (inverse_direction)
    {
        *inverse_direction = ray.direction;
    }

    if (first_hit)
    {
        *first_hit = min;
    }

    if (second_hit)
    {
        *second_hit = max;
    }

    return min <= max;
}

#endif // _IRIS_ADVANCED_BOUNDING_BOX_