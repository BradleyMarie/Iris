/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    test_util.h

Abstract:

    Utility function to aid in unit test writing.

--*/

#ifndef _TEST_TEST_UTIL_
#define _TEST_TEST_UTIL_

#include "iris/point.h"
#include "iris/vector.h"

bool operator==(
    const POINT3& p0, 
    const POINT3& p1
    )
{
    return p0.x == p1.x && p0.y == p1.y && p0.z == p1.z;
}

bool operator==(
    const VECTOR3& v0, 
    const VECTOR3& v1
    )
{
    return v0.x == v1.x && v0.y == v1.y && v0.z == v1.z;
}

#endif // _TEST_TEST_UTIL_