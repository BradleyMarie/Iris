/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    surfacenormal.cpp

Abstract:

    This file contains the definitions for the 
    Iris++ Surface Normal type.

--*/

#include <irisplusplus.h>
#include <sstream>

namespace Iris {

SurfaceNormal::SurfaceNormal(
    _In_ PSURFACE_NORMAL IrisSurfaceNormal
    )
: Data(IrisSurfaceNormal)
{ }

Vector
SurfaceNormal::ModelNormal(
    void
    )
{
    VECTOR3 IrisVector;

    ISTATUS Status = SurfaceNormalGetModelNormal(Data, &IrisVector);

    if (Status != ISTATUS_SUCCESS)
    {
        std::ostringstream error;
        error << "Iris Error: " << Status;
        throw std::runtime_error(error.str());
    }

    return Vector(IrisVector);
}

Vector
SurfaceNormal::WorldNormal(
    void
    )
{
    VECTOR3 IrisVector;

    ISTATUS Status = SurfaceNormalGetWorldNormal(Data, &IrisVector);

    if (Status != ISTATUS_SUCCESS)
    {
        std::ostringstream error;
        error << "Iris Error: " << Status;
        throw std::runtime_error(error.str());
    }

    return Vector(IrisVector);
}

Vector
SurfaceNormal::NormalizedModelNormal(
    void
    )
{
    VECTOR3 IrisVector;

    ISTATUS Status = SurfaceNormalGetNormalizedModelNormal(Data, &IrisVector);

    if (Status != ISTATUS_SUCCESS)
    {
        std::ostringstream error;
        error << "Iris Error: " << Status;
        throw std::runtime_error(error.str());
    }

    return Vector(IrisVector);
}

Vector
SurfaceNormal::NormalizedWorldNormal(
    void
    )
{
    VECTOR3 IrisVector;

    ISTATUS Status = SurfaceNormalGetNormalizedWorldNormal(Data, &IrisVector);

    if (Status != ISTATUS_SUCCESS)
    {
        std::ostringstream error;
        error << "Iris Error: " << Status;
        throw std::runtime_error(error.str());
    }

    return Vector(IrisVector);
}

} // namespace Iris