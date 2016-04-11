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
    IRISPHYSXPLUSPLUSAPI
    Geometry(
        _In_ PPBR_GEOMETRY GeometryPtr,
        _In_ bool Retain
        );
    
    _Ret_opt_
    IRISPHYSXPLUSPLUSAPI
    PHIT_LIST
    TestNestedGeometry(
        _In_ HitAllocator Allocator
        ) const;
    
    _Ret_
    IRISPHYSXPLUSPLUSAPI
    bool
    CheckBounds(
        _In_ BOUNDING_BOX WorldAlignedBoundingBox,
        _In_ const Iris::Matrix & ModelToWorld
        ) const;
    
	_Ret_
    IRISPHYSXPLUSPLUSAPI
    std::optional<Iris::Vector>
    ComputeNormal(
        _In_ const Iris::Point & HitPoint,
        _In_ const UINT32 FaceHit
        ) const;
    
	_Ret_
    IRISPHYSXPLUSPLUSAPI
	std::optional<MaterialReference>
    GetMaterial(
        _In_ const UINT32 FaceHit
        ) const;
    
	_Ret_
    IRISPHYSXPLUSPLUSAPI
    std::optional<LightReference>
    GetLight(
        _In_ const UINT32 FaceHit
        ) const;
    
    ~Geometry(
        void
        )
    {
        PBRGeometryRelease(Data);    
    }
    
private:
    PPBR_GEOMETRY Data;
};

} // namespace Iris

#endif // _GEOMETRY_IRIS_PHYSX_PLUS_PLUS_