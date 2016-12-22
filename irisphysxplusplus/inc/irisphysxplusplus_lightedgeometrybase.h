/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    irisphysxplusplus_lightedgeometrybase.h

Abstract:

    This file contains the declarations for the 
    IrisPysx++ LightedGeometryBase type.

--*/

#include <irisphysxplusplus.h>

#ifndef _LIGHTED_GEOMETRY_BASE_IRIS_PHYSX_PLUS_PLUS_
#define _LIGHTED_GEOMETRY_BASE_IRIS_PHYSX_PLUS_PLUS_

namespace IrisPhysx {

//
// Types
//

class LightedGeometryBase : public GeometryBase {
protected:
    IRISPHYSXPLUSPLUSAPI
    static
    SIZE_T
    Create(
        _In_ AreaLightBuilder & Builder,
        _In_ std::unique_ptr<LightedGeometryBase> LightedGeometryBasePtr
        );
        
public:
    LightedGeometryBase(
         void
        ) = default;

    virtual
    FLOAT
    ComputeSurfaceArea(
        _In_ UINT32 Face
        ) const = 0;

    virtual
    Iris::Point
    SampleSurface(
        _In_ UINT32 Face,
        _In_ IrisAdvanced::RandomReference Rng
        ) const = 0;

    LightedGeometryBase(
        _In_ const LightedGeometryBase & ToCopy
        ) = delete;
        
    LightedGeometryBase &
    operator=(
        _In_ const LightedGeometryBase & ToCopy
        ) = delete;

    virtual
    ~LightedGeometryBase(
        void
        ) = default;
};

} // namespace IrisPhysx

#endif // _LIGHTED_GEOMETRY_BASE_IRIS_PHYSX_PLUS_PLUS_
