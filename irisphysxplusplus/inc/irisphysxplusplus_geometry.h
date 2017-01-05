/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    irisphysxplusplus_geometry.h

Abstract:

    This file contains the declarations for the 
    IrisPysx++ Geometry type.

--*/

#include <irisphysxplusplus.h>

#ifndef _GEOMETRY_IRIS_PHYSX_PLUS_PLUS_
#define _GEOMETRY_IRIS_PHYSX_PLUS_PLUS_

namespace IrisPhysx {

//
// Types
//

class Geometry final {
public:
    Geometry(
        _In_ PPHYSX_GEOMETRY GeometryPtr,
        _In_ bool Retain
        )
    : Data(GeometryPtr)
    {
        if (GeometryPtr == nullptr)
        {
            throw std::invalid_argument("GeometryPtr");
        }

        if (Retain)
        {
            PhysxGeometryRetain(GeometryPtr);
        }
    }
    
    _Ret_
    PPHYSX_GEOMETRY
    AsPPHYSX_GEOMETRY(
        void
        )
    {
        return Data;
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
                                                    FaceHit,
                                                    HitPoint.AsPOINT3(),
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
        PCPHYSX_MATERIAL Result;

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
        PCPHYSX_LIGHT Result;

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

    Geometry(
        _In_ const Geometry & ToCopy
        )
    : Data(ToCopy.Data)
    {
        PhysxGeometryRetain(Data);
    }

    Geometry &
    operator=(
        _In_ const Geometry & ToCopy
        )
    {
        if (this != &ToCopy)
        {
            PhysxGeometryRelease(Data);
            Data = ToCopy.Data;
            PhysxGeometryRetain(Data);
        }

        return *this;
    }

    ~Geometry(
        void
        )
    {
        PhysxGeometryRelease(Data);
    }
    
private:
    PPHYSX_GEOMETRY Data;
};

} // namespace Iris

#endif // _GEOMETRY_IRIS_PHYSX_PLUS_PLUS_
