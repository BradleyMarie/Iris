/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    irisphysxplusplus_geometrybase.h

Abstract:

    This file contains the declarations for the 
    IrisPysx++ GeometryBase type.

--*/

#include <irisphysxplusplus.h>

#ifndef _GEOMETRY_BASE_IRIS_PHYSX_PLUS_PLUS_
#define _GEOMETRY_BASE_IRIS_PHYSX_PLUS_PLUS_

namespace IrisPhysx {

//
// Types
//

class GeometryBase {
protected:
    IRISPHYSXPLUSPLUSAPI
    static
    Geometry
    Create(
        _In_ std::unique_ptr<GeometryBase> GeometryBasePtr
        );

public:
    _Ret_
    virtual
    PHIT_LIST
    TestRay(
        _In_ Iris::Ray WorldRay,
        _In_ HitAllocator Allocator
        ) const = 0;

    _Ret_
    virtual
    bool
    CheckBounds(
        _In_ const Iris::MatrixReference & ModelToWorld,
        _In_ const BOUNDING_BOX & WorldAlignedBoundingBox
        ) const = 0;
    
    _Ret_
    virtual
    Iris::Vector
    ComputeNormal(
        _In_ const Iris::Point & HitPoint,
        _In_ const UINT32 FaceHit
        ) const = 0;
    
    _Ret_ 
    virtual
    std::optional<MaterialReference>
    GetMaterial(
        _In_ const UINT32 FaceHit
        ) const = 0;
    
    _Ret_
    virtual
    std::optional<LightReference>
    GetLight(
        _In_ const UINT32 FaceHit
        ) const = 0;
    
    virtual
    ~GeometryBase(
        void
        )
    { }
};

} // namespace IrisPhysx

#endif // _GEOMETRY_IRIS_PHYSX_PLUS_PLUS_