/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    irisphysxplusplus_brdf.h

Abstract:

    This file contains the declarations for the 
    IrisPysx++ BRDF type.

--*/

#include <irisphysxplusplus.h>

#ifndef _BRDF_IRIS_PHYSX_PLUS_PLUS_
#define _BRDF_IRIS_PHYSX_PLUS_PLUS_

namespace IrisPhysx {

//
// Types
//

class BRDF final {
public:
    IRISPHYSXPLUSPLUSAPI
    BRDF(
        _In_ PPBR_BRDF PbrBrdf,
        _In_ bool Retain
        );
        
    _Ret_
    std::tuple<IrisSpectrum::ReflectorReference, Iris::Vector, FLOAT>
    Sample(
        _In_ const Iris::Vector & Incoming,
        _In_ IrisAdvanced::RandomReference Rng,
        _In_ IrisSpectrum::ReflectorCompositorReference Compositor
        ) const;
        
    _Ret_
    std::tuple<IrisSpectrum::ReflectorReference, Iris::Vector, FLOAT>
    SampleWithLambertianFalloff(
        _In_ const Iris::Vector & Incoming,
        _In_ IrisAdvanced::RandomReference Rng,
        _In_ IrisSpectrum::ReflectorCompositorReference Compositor
        ) const;
    
    _Ret_
    IrisSpectrum::ReflectorReference
    ComputeReflectance(
        _In_ const Iris::Vector & Incoming,
        _In_ const Iris::Vector & Outgoing,
        _In_ IrisSpectrum::ReflectorCompositorReference Compositor
        ) const;
    
    _Ret_
    IrisSpectrum::ReflectorReference
    ComputeReflectanceWithLambertianFalloff(
        _In_ const Iris::Vector & Incoming,
        _In_ const Iris::Vector & Outgoing,
        _In_ IrisSpectrum::ReflectorCompositorReference Compositor
        ) const;
    
    _Ret_
    std::tuple<IrisSpectrum::ReflectorReference, FLOAT>
    ComputeReflectanceWithPdf(
        _In_ const Iris::Vector & Incoming,
        _In_ const Iris::Vector & Outgoing,
        _In_ IrisSpectrum::ReflectorCompositorReference Compositor
        ) const;
    
    _Ret_
    std::tuple<IrisSpectrum::ReflectorReference, FLOAT>
    ComputeReflectanceWithPdfWithLambertianFalloff(
        _In_ const Iris::Vector & Incoming,
        _In_ const Iris::Vector & Outgoing,
        _In_ IrisSpectrum::ReflectorCompositorReference Compositor
        ) const;
    
    ~BRDF(
        void
        )
    {
        PbrBrdfRelease(Data);    
    }
    
private:
    PPBR_BRDF Data;
};

} // namespace Iris

#endif // _BRDF_IRIS_PHYSX_PLUS_PLUS_