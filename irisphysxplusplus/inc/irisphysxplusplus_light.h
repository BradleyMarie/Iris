/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    irisphysxplusplus_light.h

Abstract:

    This file contains the declarations for the 
    IrisPysx++ Light type.

--*/

#include <irisphysxplusplus.h>

#ifndef _LIGHT_IRIS_PHYSX_PLUS_PLUS_
#define _LIGHT_IRIS_PHYSX_PLUS_PLUS_

namespace IrisPhysx {

//
// Forward Declarations
//

class VisibilityTester;

//
// Types
//

class Light final {
public:
    IRISPHYSXPLUSPLUSAPI
    Light(
        _In_ PPBR_LIGHT LightPtr,
        _In_ bool Retain
        );
    
    _Ret_
    IRISPHYSXPLUSPLUSAPI
    std::tuple<IrisSpectrum::SpectrumReference, Iris::Vector, FLOAT>
    Sample(
        _In_ const Iris::Point & HitPoint,
        _In_ VisibilityTester Tester,
        _In_ IrisAdvanced::RandomReference Rng,
        _In_ IrisSpectrum::SpectrumCompositorReference Compositor
        ) const;

    _Ret_
    IRISPHYSXPLUSPLUSAPI
    FLOAT
    ComputeEmissive(
        _In_ const Iris::Ray & ToLight,
        _In_ VisibilityTester Tester
        ) const;
    
    _Ret_
    IRISPHYSXPLUSPLUSAPI
    std::tuple<IrisSpectrum::SpectrumReference, FLOAT>
    ComputeEmissive(
        _In_ const Iris::Ray & ToLight,
        _In_ VisibilityTester Tester
        ) const;
    
    ~Light(
        void
        )
    {
        PbrLightRelease(Data);    
    }
    
private:
    PPBR_LIGHT Data;
};

} // namespace Iris

#endif // _LIGHT_IRIS_PHYSX_PLUS_PLUS_