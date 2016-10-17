/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    irisphysxplusplus_geometryreference.h

Abstract:

    This file contains the declarations for the 
    IrisPysx++ Geometry type.

--*/

#include <irisphysxplusplus.h>

#ifndef _GEOMETRY_REFERENCE_IRIS_PHYSX_PLUS_PLUS_
#define _GEOMETRY_REFERENCE_IRIS_PHYSX_PLUS_PLUS_

namespace IrisPhysx {

//
// Types
//

class GeometryReference final {
public:
    GeometryReference(
        _In_ PCPHYSX_GEOMETRY GeometryPtr
        )
    : Data(GeometryPtr)
    {
        if (GeometryPtr == nullptr)
        {
            throw std::invalid_argument("GeometryPtr");
        }
    }

    _Ret_
    PCPHYSX_GEOMETRY
    AsPCPHYSX_GEOMETRY(
        void
        ) const
    {
        return Data;
    }

    _Ret_opt_
    PHIT_LIST
    TestNestedGeometry(
        _In_ HitAllocator Allocator
        ) const
    {
        PHIT_LIST Result;

        ISTATUS Status = PhysxGeometryTestNestedGeometry(Data,
                                                         Allocator.AsPPHYSX_HIT_ALLOCATOR(),
                                                         &Result);

        if (Status != ISTATUS_SUCCESS)
        {
            throw std::runtime_error(Iris::ISTATUSToCString(Status));
        }

        return Result;
    }
    
    _Ret_
    bool
    CheckBounds(
        _In_ const Iris::Matrix & ModelToWorld,
        _In_ const BOUNDING_BOX & WorldAlignedBoundingBox
        ) const
    {
        BOOL Result;

        ISTATUS Status = PhysxGeometryCheckBounds(Data,
                                                  ModelToWorld.AsPCMATRIX(),
                                                  WorldAlignedBoundingBox,
                                                  &Result);

        if (Status != ISTATUS_SUCCESS)
        {
            throw std::runtime_error(Iris::ISTATUSToCString(Status));
        }

        return Result != FALSE;
    }
    
	_Ret_
    Iris::Vector
    ComputeNormal(
        _In_ const Iris::Point & HitPoint,
        _In_ const UINT32 FaceHit
        ) const
    {
        VECTOR3 Result;

        ISTATUS Status = PhysxGeometryComputeNormal(Data,
						                            HitPoint.AsPOINT3(),
								                    FaceHit,
                                                    &Result);

        if (Status != ISTATUS_SUCCESS)
        {
            throw std::runtime_error(Iris::ISTATUSToCString(Status));
        }

        return Iris::Vector(Result);
    }
    
	_Ret_
	std::optional<MaterialReference>
    GetMaterial(
        _In_ const UINT32 FaceHit
        ) const
    {
        PCPBR_MATERIAL Result;

        ISTATUS Status = PhysxGeometryGetMaterial(Data,
						                          FaceHit,
								                  &Result);

        if (Status != ISTATUS_SUCCESS)
        {
            throw std::runtime_error(Iris::ISTATUSToCString(Status));
        }

        if (Result == nullptr)
        {
            return std::nullopt;
        }

        return std::make_optional(MaterialReference(Result));
    }
    
	_Ret_
    std::optional<LightReference>
    GetLight(
        _In_ const UINT32 FaceHit
        ) const
    {
        PCPBR_LIGHT Result;

        ISTATUS Status = PhysxGeometryGetLight(Data,
						                       FaceHit,
								               &Result);

        if (Status != ISTATUS_SUCCESS)
        {
            throw std::runtime_error(Iris::ISTATUSToCString(Status));
        }

        if (Result == nullptr)
        {
            return std::nullopt;
        }

        return std::make_optional(LightReference(Result));
    }
    
private:
    PCPHYSX_GEOMETRY Data;
};

} // namespace Iris

#endif // _GEOMETRY_REFERENCE_IRIS_PHYSX_PLUS_PLUS_
