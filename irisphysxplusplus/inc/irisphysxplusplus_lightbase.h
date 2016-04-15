/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    irisphysxplusplus_light.h

Abstract:

    This file contains the declarations for the 
    IrisPysx++ Light type.

--*/

#include <irisphysxplusplus.h>

#ifndef _LIGHT_BASE_IRIS_PHYSX_PLUS_PLUS_
#define _LIGHT_BASE_IRIS_PHYSX_PLUS_PLUS_

namespace IrisPhysx {

//
// Types
//

class LightBase {
protected:
    IRISPHYSXPLUSPLUSAPI
    static
    Light
    Create(
        _In_ std::unique_ptr<LightBase> LightBasePtr
        );

public:
    _Ret_
    virtual
    std::tuple<IrisSpectrum::SpectrumReference, Iris::Vector, FLOAT>
    Sample(
        _In_ const Iris::Point & HitPoint,
        _In_ VisibilityTester Tester,
        _In_ IrisAdvanced::RandomReference Rng,
        _In_ IrisSpectrum::SpectrumCompositorReference Compositor
        ) const = 0;

    _Ret_
    virtual
    IrisSpectrum::SpectrumReference
    ComputeEmissive(
        _In_ const Iris::Ray & ToLight,
        _In_ VisibilityTester Tester
        ) const = 0;
    
    _Ret_
    virtual
    std::tuple<IrisSpectrum::SpectrumReference, FLOAT>
    ComputeEmissiveWithPdf(
        _In_ const Iris::Ray & ToLight,
        _In_ VisibilityTester Tester
        ) const = 0;
    
    virtual
    ~LightBase(
        void
        )
    { }
};

} // namespace IrisPhysx

#endif // _LIGHT_BASE_IRIS_PHYSX_PLUS_PLUS_