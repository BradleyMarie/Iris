/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    irisphysxtoolkitplusplus_sphere.h

Abstract:

    This file contains the definition for the Sphere type.

--*/

#include <irisphysxtoolkitplusplus.h>

#ifndef _SPHERE_IRIS_PHYSX_TOOLKIT_PLUS_PLUS_
#define _SPHERE_IRIS_PHYSX_TOOLKIT_PLUS_PLUS_

namespace IrisPhysxToolkit {
namespace Sphere {

//
// Functions
//

static
inline
IrisPhysx::Geometry
Create(
    _In_ const Iris::Point & Center,
    _In_ FLOAT Radius,
    _In_ std::optional<IrisPhysx::Material> FrontMaterial,
    _In_ std::optional<IrisPhysx::Material> BackMaterial
    )
{
    PPBR_MATERIAL IrisFrontMaterial = (FrontMaterial) ? FrontMaterial->AsPPBR_MATERIAL() : nullptr;
    PPBR_MATERIAL IrisBackMaterial = (BackMaterial) ? BackMaterial->AsPPBR_MATERIAL() : nullptr;

    PPBR_GEOMETRY GeometryPtr;
    ISTATUS Status = PhysxSphereAllocate(Center.AsPOINT3(),
                                         Radius,
                                         IrisFrontMaterial,
                                         IrisBackMaterial,
                                         &GeometryPtr);

    switch (Status)
    {
        case ISTATUS_SUCCESS:
            break;
        case ISTATUS_INVALID_ARGUMENT_00:
            throw std::invalid_argument("Center");
        case ISTATUS_INVALID_ARGUMENT_01:
            throw std::invalid_argument("Radius");
        case ISTATUS_ALLOCATION_FAILED:
            throw std::bad_alloc();
    }

    assert(Status == ISTATUS_SUCCESS);
    return IrisPhysx::Geometry(GeometryPtr, false);
}

} // namespace Sphere
} // namespace IrisPhysxToolkit

#endif // _SPHERE_IRIS_PHYSX_TOOLKIT_PLUS_PLUS_