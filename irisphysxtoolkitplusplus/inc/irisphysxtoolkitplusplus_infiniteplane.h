/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    irisphysxtoolkitplusplus_infiniteplane.h

Abstract:

    This file contains the definition for the InfinitePlane type.

--*/

#include <irisphysxtoolkitplusplus.h>

#ifndef _INFINITE_PLANE_IRIS_PHYSX_TOOLKIT_PLUS_PLUS_
#define _INFINITE_PLANE_IRIS_PHYSX_TOOLKIT_PLUS_PLUS_

namespace IrisPhysxToolkit {
namespace InfinitePlane {

//
// Functions
//

static
inline
IrisPhysx::Geometry
Create(
    _In_ const Iris::Point & Point,
    _In_ const Iris::Vector & SurfaceNormal,
    _In_ std::optional<IrisPhysx::Material> FrontMaterial,
    _In_ std::optional<IrisPhysx::Material> BackMaterial
    )
{
    PPHYSX_MATERIAL IrisFrontMaterial = (FrontMaterial) ? FrontMaterial->AsPPHYSX_MATERIAL() : nullptr;
    PPHYSX_MATERIAL IrisBackMaterial = (BackMaterial) ? BackMaterial->AsPPHYSX_MATERIAL() : nullptr;

    PPHYSX_GEOMETRY GeometryPtr;
    ISTATUS Status = PhysxInfinitePlaneAllocate(Point.AsPOINT3(),
                                                SurfaceNormal.AsVECTOR3(),
                                                IrisFrontMaterial,
                                                IrisBackMaterial,
                                                &GeometryPtr);

    switch (Status)
    {
        case ISTATUS_SUCCESS:
            break;
        case ISTATUS_INVALID_ARGUMENT_00:
            throw std::invalid_argument("Point");
        case ISTATUS_INVALID_ARGUMENT_01:
            throw std::invalid_argument("SurfaceNormal");
        case ISTATUS_ALLOCATION_FAILED:
            throw std::bad_alloc();
    }

    assert(Status == ISTATUS_SUCCESS);
    return IrisPhysx::Geometry(GeometryPtr, false);
}

} // namespace InfinitePlane
} // namespace IrisPhysxToolkit

#endif // _INFINITE_PLANE_IRIS_PHYSX_TOOLKIT_PLUS_PLUS_