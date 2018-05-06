/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    vector.h

Abstract:

    The vector type used by Iris.

--*/

#ifndef _IRIS_VECTOR_
#define _IRIS_VECTOR_

#include "iris/sal.h"

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>

#if __cplusplus 
#include <math.h>
#else
#include <tgmath.h>
#endif // __cplusplus

//
// Types
//

typedef struct _VECTOR3 {
    float_t x;
    float_t y;
    float_t z;
} VECTOR3, *PVECTOR3;

typedef const VECTOR3 *PCVECTOR3;

//
// Enums
//

typedef enum _VECTOR_AXIS {
    VECTOR_X_AXIS = 0,
    VECTOR_Y_AXIS = 1,
    VECTOR_Z_AXIS = 2
} VECTOR_AXIS;

//
// Functions
//

static
inline
VECTOR3
VectorCreate(
    _In_ float_t x,
    _In_ float_t y,
    _In_ float_t z
    )
{
    assert(isfinite(x));
    assert(isfinite(y));
    assert(isfinite(z));

    VECTOR3 result;
    result.x = x;
    result.y = y;
    result.z = z;

    return result;
}

static
inline
VECTOR3
VectorNegate(
    _In_ VECTOR3 vector
    )
{
    float_t x = -vector.x;
    float_t y = -vector.y;
    float_t z = -vector.z;

    return VectorCreate(x, y, z);;
}

static
inline
VECTOR3
VectorAdd(
    _In_ VECTOR3 addend0,
    _In_ VECTOR3 addend1
    )
{
    float_t x = addend0.x + addend1.x;
    float_t y = addend0.y + addend1.y;
    float_t z = addend0.z + addend1.z;

    return VectorCreate(x, y, z);
}

static
inline
VECTOR3
VectorAddScaled(
    _In_ VECTOR3 addend0,
    _In_ VECTOR3 addend1,
    _In_ float_t scalar
    )
{
    assert(isfinite(scalar));

    float_t x = fma(scalar, addend1.x, addend0.x);
    float_t y = fma(scalar, addend1.y, addend0.y);
    float_t z = fma(scalar, addend1.z, addend0.z);

    return VectorCreate(x, y, z);
}

static
inline
VECTOR3
VectorSubtract(
    _In_ VECTOR3 minuend,
    _In_ VECTOR3 subtrahend
    )
{
    float_t x = minuend.x - subtrahend.x;
    float_t y = minuend.y - subtrahend.y;
    float_t z = minuend.z - subtrahend.z;

    return VectorCreate(x, y, z);
}

static
inline
VECTOR3
VectorScale(
    _In_ VECTOR3 vector,
    _In_ float_t scalar
    )
{
    assert(isfinite(scalar) != 0);

    float_t x = vector.x * scalar;
    float_t y = vector.y * scalar;
    float_t z = vector.z * scalar;

    return VectorCreate(x, y, z);
}

static
inline
float_t
VectorDotProduct(
    _In_ VECTOR3 operand0,
    _In_ VECTOR3 operand1
    )
{
    float_t x = operand0.x * operand1.x;
    float_t y = operand0.y * operand1.y;
    float_t z = operand0.z * operand1.z;

    return x + y + z;
}

static
inline
VECTOR3
VectorCrossProduct(
    _In_ VECTOR3 operand0,
    _In_ VECTOR3 operand1
    )
{
    float_t x = operand0.y * operand1.z - operand0.z * operand1.y;
    float_t y = operand0.z * operand1.x - operand0.x * operand1.z;
    float_t z = operand0.x * operand1.y - operand0.y * operand1.x;

    return VectorCreate(x, y, z);
}

static
inline
float_t
VectorLength(
    _In_ VECTOR3 vector
    )
{
    float_t DotProduct = VectorDotProduct(vector, vector);
    return sqrt(DotProduct);
}

static
inline
bool
VectorValidate(
    _In_ VECTOR3 vector
    )
{
    if (vector.x == (float_t) 0.0 &&
        vector.y == (float_t) 0.0 &&
        vector.z == (float_t) 0.0)
    {
        return false;
    }

    if (!isfinite(vector.x) || !isfinite(vector.y) || !isfinite(vector.z))
    {
        return false;
    }

    return true;
}

static
inline
VECTOR3
VectorNormalize(
    _In_ VECTOR3 vector,
    _Out_opt_ float_t *old_length_squared,
    _Out_opt_ float_t *old_length
    )
{
    float_t length_squared = VectorDotProduct(vector, vector);

    if (old_length_squared != NULL)
    {
        *old_length_squared = length_squared;
    }

    float_t length = sqrt(length_squared);

    if (old_length != NULL)
    {
        *old_length = length;
    }

    return VectorScale(vector, (float_t) 1.0 / length);
}

static
inline
VECTOR_AXIS
VectorDominantAxis(
    _In_ VECTOR3 vector
    )
{
    float_t abs_x = fabs(vector.x);
    float_t abs_y = fabs(vector.y);
    float_t abs_z = fabs(vector.z);

    if (abs_x > abs_y && abs_x > abs_z)
    {
        return VECTOR_X_AXIS;
    }
    else if (abs_y > abs_z)
    {
        return VECTOR_Y_AXIS;
    }
    else
    {
        return VECTOR_Z_AXIS;
    }
}

static
inline
VECTOR_AXIS
VectorDiminishedAxis(
    _In_ VECTOR3 vector
    )
{
    float_t abs_x = fabs(vector.x);
    float_t abs_y = fabs(vector.y);
    float_t abs_z = fabs(vector.z);

    if (abs_x < abs_y && abs_x < abs_z)
    {
        return VECTOR_X_AXIS;
    }
    else if (abs_y < abs_z)
    {
        return VECTOR_Y_AXIS;
    }
    else
    {
        return VECTOR_Z_AXIS;
    }
}

static
inline
VECTOR3
VectorReflect(
    _In_ VECTOR3 incident,
    _In_ VECTOR3 normal
    )
{
    float_t scalar = (float_t) 2.0 * VectorDotProduct(normal, incident);

    VECTOR3 scaled_normal = VectorScale(normal, scalar);
    VECTOR3 reflected = VectorSubtract(incident, scaled_normal);

    return VectorNormalize(reflected, NULL, NULL);
}

static
inline
VECTOR3
VectorHalfAngle(
    _In_ VECTOR3 vector0,
    _In_ VECTOR3 vector1
    )
{
    VECTOR3 result = VectorAdd(vector0, vector1);
    return VectorNormalize(result, NULL, NULL);
}

#endif // _IRIS_VECTOR_