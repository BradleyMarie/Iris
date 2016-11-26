/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    irisphysxplusplus_arealightbuilder.h

Abstract:

    This file contains the declarations for the 
    IrisPysx++ AreaLightBuilder type.

--*/

#include <irisphysxplusplus.h>

#ifndef _AREA_LIGHT_BUILDER_IRIS_PHYSX_PLUS_PLUS_
#define _AREA_LIGHT_BUILDER_IRIS_PHYSX_PLUS_PLUS_

namespace IrisPhysx {

//
// Types
//

class AreaLightBuilder final {
public:
    AreaLightBuilder(
        _In_ AreaLightBuilder && ToMove
        )
    : Data(ToMove.Data)
    {
        ToMove.Data = nullptr;
    }
    
    static
    AreaLightBuilder
    Create(
        void
        )
    { 
        return AreaLightBuilder();
    }

    _Ret_
    PPHYSX_AREA_LIGHT_BUILDER
    AsPPHYSX_AREA_LIGHT_BUILDER(
        void
        )
    {
        return Data;
    }

    IRISPHYSXPLUSPLUSAPI
    void
    AttachLightToGemoetry(
        _In_ SIZE_T GeometryIndex,
        _In_ UINT32 FaceIndex,
        _In_ SIZE_T LightIndex
        );

    IRISPHYSXPLUSPLUSAPI
    std::tuple<std::vector<Geometry>, std::vector<Light>>
    Build(
        void
        );

    virtual
    ~AreaLightBuilder(
        void
        )
    {
        PhysxAreaLightBuilderFree(Data);
    }

private:
    PPHYSX_AREA_LIGHT_BUILDER Data;

    AreaLightBuilder(
        void
        )
    { 
        ISTATUS Status = PhysxAreaLightBuilderAllocate(&Data);

        if (Status != ISTATUS_SUCCESS)
        {
            ASSERT(Status == ISTATUS_ALLOCATION_FAILED);
            throw std::bad_alloc();
        }
    }
};

} // namespace Iris

#endif // _AREA_LIGHT_BUILDER_IRIS_PHYSX_PLUS_PLUS_
