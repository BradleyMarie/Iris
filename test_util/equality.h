/*++

Copyright (c) 2020 Brad Weinberger

Module Name:

    equality.h

Abstract:

    Equality tests for use in unit tests.

--*/

#ifndef _TEST_UTIL_EQUALITY_
#define _TEST_UTIL_EQUALITY_

extern "C" {
#include "iris/iris.h"
#include "iris_advanced/iris_advanced.h"
}

#include "googlemock/include/gmock/gmock.h"

#include <cmath>

bool 
operator==(
    const POINT3& p0, 
    const POINT3& p1
    )
{
    return p0.x == p1.x && p0.y == p1.y && p0.z == p1.z;
}

bool 
operator==(
    const VECTOR3& v0, 
    const VECTOR3& v1
    )
{
    return v0.x == v1.x && v0.y == v1.y && v0.z == v1.z;
}

bool 
operator==(
    const RAY& r0, 
    const RAY& r1
    )
{
    return r0.origin == r1.origin && r0.direction == r1.direction;
}

bool 
operator==(
    const COLOR3& c0, 
    const COLOR3& c1
    )
{
    return c0.x == c1.x && c0.y == c1.y && c0.z == c1.z;
}

MATCHER_P(EqualsMatrix, value, "")
{
    if (value == arg)
    {
        return true;
    }

    float_t value_contents[4][4];
    MatrixReadContents(value, value_contents);

    float_t arg_contents[4][4];
    MatrixReadContents(arg, arg_contents);

    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            if (value_contents[i][j] != arg_contents[i][j])
            {
                return false;
            }
        }
    }

    auto value_inverse = MatrixGetConstantInverse(value);
    MatrixReadContents(value_inverse, value_contents);

    auto arg_inverse = MatrixGetConstantInverse(arg);
    MatrixReadContents(arg_inverse, arg_contents);

    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            if (value_contents[i][j] != arg_contents[i][j])
            {
                return false;
            }
        }
    }

    return true;
}

MATCHER_P2(ApproximatelyEqualsMatrix, value, epsilon, "")
{
    if (value == arg)
    {
        return true;
    }

    float_t value_contents[4][4];
    MatrixReadContents(value, value_contents);

    float_t arg_contents[4][4];
    MatrixReadContents(arg, arg_contents);

    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            if (std::abs(value_contents[i][j] - arg_contents[i][j]) > epsilon)
            {
                return false;
            }
        }
    }

    auto value_inverse = MatrixGetConstantInverse(value);
    MatrixReadContents(value_inverse, value_contents);

    auto arg_inverse = MatrixGetConstantInverse(arg);
    MatrixReadContents(arg_inverse, arg_contents);

    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            if (std::abs(value_contents[i][j] - arg_contents[i][j]) > epsilon)
            {
                return false;
            }
        }
    }

    return true;
}

#endif // _TEST_UTIL_EQUALITY_