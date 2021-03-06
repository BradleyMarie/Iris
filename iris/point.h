/*++

Copyright (c) 2021 Brad Weinberger

Module Name:

    point.h

Abstract:

    The point type used by Iris.

--*/

#ifndef _IRIS_POINT_
#define _IRIS_POINT_

#include "iris/vector.h"

//
// Types
//

typedef struct _POINT3 {
    float_t x;
    float_t y;
    float_t z;
} POINT3, *PPOINT3;

typedef const POINT3 *PCPOINT3;

//
// Functions
//

static
inline
POINT3
PointCreate(
    _In_ float_t x,
    _In_ float_t y,
    _In_ float_t z
    )
{
    assert(isfinite(x));
    assert(isfinite(y));
    assert(isfinite(z));

    POINT3 point;
    point.x = x;
    point.y = y;
    point.z = z;

    return point;
}

static
inline
float_t
PointGetElement(
    _In_ POINT3 point,
    _In_ VECTOR_AXIS axis
    )
{
    assert(axis == VECTOR_X_AXIS ||
           axis == VECTOR_Y_AXIS ||
           axis == VECTOR_Z_AXIS);

    const float_t *as_array = &point.x;
    return as_array[axis];
}

static
inline
VECTOR3
PointSubtract(
    _In_ POINT3 minuend,
    _In_ POINT3 subtrahend
    )
{
    float_t x = minuend.x - subtrahend.x;
    float_t y = minuend.y - subtrahend.y;
    float_t z = minuend.z - subtrahend.z;

    return VectorCreate(x, y, z);
}

static
inline
POINT3
PointVectorSubtract(
    _In_ POINT3 minuend,
    _In_ VECTOR3 subtrahend
    )
{
    float_t x = minuend.x - subtrahend.x;
    float_t y = minuend.y - subtrahend.y;
    float_t z = minuend.z - subtrahend.z;

    return PointCreate(x, y, z);
}

static
inline
POINT3
PointVectorAdd(
    _In_ POINT3 addend0,
    _In_ VECTOR3 addend1
    )
{

    float_t x = addend0.x + addend1.x;
    float_t y = addend0.y + addend1.y;
    float_t z = addend0.z + addend1.z;

    return PointCreate(x, y, z);
}

static
inline
POINT3
PointVectorAddScaled(
    _In_ POINT3 point,
    _In_ VECTOR3 vector,
    _In_ float_t scalar
    )
{
    assert(isfinite(scalar));

    float_t x = point.x + vector.x * scalar;
    float_t y = point.y + vector.y * scalar;
    float_t z = point.z + vector.z * scalar;

    return PointCreate(x, y, z);
}

static
inline
POINT3
PointVectorSubtractScaled(
    _In_ POINT3 point,
    _In_ VECTOR3 vector,
    _In_ float_t scalar
    )
{
    assert(isfinite(scalar));
    
    float_t negated_scalar = -scalar;

    POINT3 Difference = PointVectorAddScaled(point,
                                             vector, 
                                             negated_scalar);

    return Difference;
}

static
inline
bool
PointValidate(
    _In_ POINT3 point
    )
{
    if (!isfinite(point.x) || !isfinite(point.y) || !isfinite(point.z) )
    {
        return false;
    }

    return true;
}

#endif // _IRIS_POINT_