/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    irisphysxplusplus_arealightbase.h

Abstract:

    This file contains the declarations for the 
    IrisPysx++ AreaLightBase type.

--*/

#include <irisphysxplusplus.h>

#ifndef _AREA_LIGHT_BASE_IRIS_PHYSX_PLUS_PLUS_
#define _AREA_LIGHT_BASE_IRIS_PHYSX_PLUS_PLUS_

namespace IrisPhysx {

//
// Types
//

class AreaLightBase {
    IRISPHYSXPLUSPLUSAPI
    static
    SIZE_T
    Create(
        _In_ AreaLightBuilder & Builder,
        _In_ std::unique_ptr<AreaLightBase> AreaLightBasePtr
        );

public:
    AreaLightBase(
        void
        ) = default;

    virtual
    IrisSpectrum::SpectrumReference
    ComputeEmissive(
        _In_ const Iris::Point & HitPoint,
        _In_ const IrisSpectrum::SpectrumCompositorReference Compositor
        ) const = 0;

    AreaLightBase(
        _In_ const AreaLightBase & ToCopy
        ) = delete;
        
    AreaLightBase &
    operator=(
        _In_ const AreaLightBase & ToCopy
        ) = delete;

    virtual
    ~AreaLightBase(
        void
        ) = default;
};

} // namespace Iris

#endif // _AREA_LIGHT_BASE_IRIS_PHYSX_PLUS_PLUS_
