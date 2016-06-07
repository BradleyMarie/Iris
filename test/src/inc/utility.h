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
    const RAY & Ray
    );

bool operator==(
    const RAY & Ray1, 
    const RAY & Ray2
    );

namespace Iris {

std::ostream & 
operator<<(
    std::ostream & OStream,
    const Iris::Vector & V
    );

bool
operator==(
    const Iris::Vector & Vector1, 
    const Iris::Vector & Vector2
    );

std::ostream &
operator<<(
    std::ostream & OStream,
    const Iris::VectorAxis & Axis
    );

std::ostream &
operator<<(
    std::ostream & OStream,
    const Iris::Point & P
    );

bool
operator==(
    const Iris::Point & Point1,
    const Iris::Point & Point2
    );

bool
operator==(
    const Iris::Ray & Ray1,
    const Iris::Ray & Ray2
    );

std::ostream &
operator<<(
    std::ostream & OStream,
    const Iris::Ray & R
    );

} // namespace Iris
    
#endif // _IRIS_TEST_UTILITY_HEADER_