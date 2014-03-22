/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    utility.h

Abstract:

    This file contains the prototypes for the utility functions used
    to allow the Iris types to interact with UnitTest++.

--*/

#ifndef _IRIS_TEST_UTILITY_HEADER_
#define _IRIS_TEST_UTILITY_HEADER_

#include <iristest.h>

std::ostream & 
operator<<(
    std::ostream & OStream,
    const VECTOR3 & Vector
    );

bool
operator==(
    const VECTOR3 & Vector1, 
    const VECTOR3 & Vector2
    );

std::ostream & 
operator<<(
    std::ostream & OStream, 
    const POINT3 & Point
    );

bool operator==(
    const POINT3 & Point1, 
    const POINT3 & Point2
    );

std::ostream & 
operator<<(
    std::ostream & OStream, 
    const MATRIX & Matrix
    );

#endif // _IRIS_TEST_UTILITY_HEADER_