/*++

Copyright (c) 2021 Brad Weinberger

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
    bool result = c0.values[0] == c1.values[0] &&
                  c0.values[1] == c1.values[1] &&
                  c0.values[2] == c1.values[2] &&
                  c0.color_space == c1.color_space;
    return result;
}

MATCHER_P2(ApproximatelyEqualsColor, value, epsilon, "")
{
    bool result = std::abs(value.values[0] - arg.values[0]) <= epsilon &&
                  std::abs(value.values[1] - arg.values[1]) <= epsilon &&
                  std::abs(value.values[2] - arg.values[2]) <= epsilon &&
                  value.color_space == arg.color_space;
    return result;
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