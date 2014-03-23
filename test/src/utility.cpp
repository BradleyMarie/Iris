/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    utility.cpp

Abstract:

    This file contains the definitions for the utility functions used
    to allow the Iris types to interact with UnitTest++.

--*/

#include <iristest.h>

#define EPSILON ((FLOAT) 0.005)

std::ostream & 
operator<<(
    std::ostream & OStream,
    const VECTOR3 & Vector
    )
{
    OStream << "(" << Vector.X << ", " << Vector.Y << ", " << Vector.Z << ")";

    return OStream;
}

bool
operator==(
    const VECTOR3 & Vector1, 
    const VECTOR3 & Vector2
    )
{
    FLOAT AbsDifference;

    AbsDifference = AbsFloat(Vector1.X - Vector2.X);

    if (AbsDifference > EPSILON)
    {
        return false;
    }

    AbsDifference = AbsFloat(Vector1.Y - Vector2.Y);

    if (AbsDifference > EPSILON)
    {
        return false;
    }

    AbsDifference = AbsFloat(Vector1.Z - Vector2.Z);

    if (AbsDifference > EPSILON)
    {
        return false;
    }

    return true;
}

std::ostream & 
operator<<(
    std::ostream & OStream, 
    const POINT3 & Point
    )
{
    OStream << "(" << Point.X << ", " << Point.Y << ", " << Point.Z << ")";

    return OStream;
}

bool operator==(
    const POINT3 & Point1, 
    const POINT3 & Point2
    )
{
    FLOAT AbsDifference;

    AbsDifference = AbsFloat(Point1.X - Point2.X);

    if (AbsDifference > EPSILON)
    {
        return false;
    }

    AbsDifference = AbsFloat(Point1.Y - Point2.Y);

    if (AbsDifference > EPSILON)
    {
        return false;
    }

    AbsDifference = AbsFloat(Point1.Z - Point2.Z);

    if (AbsDifference > EPSILON)
    {
        return false;
    }

    return true;
}

std::ostream & 
operator<<(
    std::ostream & OStream, 
    const RAY & Ray
    )
{
    OStream << "((" << Ray.Origin.X << ", " << Ray.Origin.Y << ", " << Ray.Origin.Z << ")";
    OStream << " ,(" << Ray.Direction.X << ", " << Ray.Direction.Y << ", " << Ray.Direction.Z << "))";

    return OStream;
}

bool operator==(
    const RAY & Ray1, 
    const RAY & Ray2
    )
{
    return Ray1.Origin == Ray2.Origin && 
           Ray1.Direction == Ray2.Direction && 
           Ray1.Time == Ray2.Time;
}
