/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    irisphysxtoolkitplusplus_triangle.h

Abstract:

    This file contains the definition for the Triangle type.

--*/

#include <irisphysxtoolkitplusplus.h>

#ifndef _TRIANGLE_IRIS_PHYSX_TOOLKIT_PLUS_PLUS_
#define _TRIANGLE_IRIS_PHYSX_TOOLKIT_PLUS_PLUS_

namespace IrisPhysxToolkit {
namespace Triangle {

//
// Functions
//

static
inline
IrisPhysx::Geometry
Create(
    _In_ const Iris::Point & Vertex0,
    _In_ const Iris::Point & Vertex1,
    _In_ const Iris::Point & Vertex2,
    _In_ std::optional<IrisPhysx::Material> FrontMaterial,
    _In_ std::optional<IrisPhysx::Material> BackMaterial
    )
{
    PPBR_MATERIAL IrisFrontMaterial = (FrontMaterial) ? FrontMaterial->AsPPBR_MATERIAL() : nullptr;
    PPBR_MATERIAL IrisBackMaterial = (BackMaterial) ? BackMaterial->AsPPBR_MATERIAL() : nullptr;

    PPBR_GEOMETRY GeometryPtr;
    ISTATUS Status = PhysxTriangleAllocate(Vertex0.AsPOINT3(),
                                           Vertex1.AsPOINT3(),
                                           Vertex2.AsPOINT3(),
                                           IrisFrontMaterial,
                                           IrisBackMaterial,
                                           &GeometryPtr);

    switch (Status)
    {
        case ISTATUS_SUCCESS:
            break;
        case ISTATUS_INVALID_ARGUMENT_00:
            throw std::invalid_argument("Vertex0");
        case ISTATUS_INVALID_ARGUMENT_01:
            throw std::invalid_argument("Vertex1");
        case ISTATUS_INVALID_ARGUMENT_02:
            throw std::invalid_argument("Vertex2");
        case ISTATUS_ALLOCATION_FAILED:
            throw std::bad_alloc();
    }

    assert(Status == ISTATUS_SUCCESS);
    return IrisPhysx::Geometry(GeometryPtr, false);
}

} // namespace Triangle
} // namespace IrisPhysxToolkit

#endif // _TRIANGLE_IRIS_PHYSX_TOOLKIT_PLUS_PLUS_