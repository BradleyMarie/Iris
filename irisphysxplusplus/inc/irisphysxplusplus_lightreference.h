/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    irisphysxplusplus_lightreference.h

Abstract:

    This file contains the declarations for the 
    IrisPysx++ LightReference type.

--*/

#include <irisphysxplusplus.h>

#ifndef _LIGHT_REFERENCE_IRIS_PHYSX_PLUS_PLUS_
#define _LIGHT_REFERENCE_IRIS_PHYSX_PLUS_PLUS_

namespace IrisPhysx {

//
// Types
//

class LightReference final {
public:
    LightReference(
        _In_ PCPBR_LIGHT LightPtr
        )
    : Data(LightPtr)
    { }
    
    _Ret_
    std::tuple<IrisSpectrum::SpectrumReference, Iris::Vector, FLOAT>
    Sample(
        _In_ const Iris::Point & HitPoint,
        _In_ VisibilityTester Tester,
        _In_ IrisAdvanced::RandomReference Rng,
        _In_ IrisSpectrum::SpectrumCompositorReference Compositor
        ) const;

    _Ret_
    FLOAT
    ComputeEmissive(
        _In_ const Iris::Ray & ToLight,
        _In_ VisibilityTester Tester
        ) const;
    
    _Ret_
    std::tuple<IrisSpectrum::SpectrumReference, FLOAT>
    ComputeEmissiveWithPdf(
        _In_ const Iris::Ray & ToLight,
        _In_ VisibilityTester Tester
        ) const;
    
private:
    PCPBR_LIGHT Data;
};

} // namespace Iris

#endif // _LIGHT_REFERENCE_IRIS_PHYSX_PLUS_PLUS_