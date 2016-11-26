/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    irisphysxplusplus_brdfbase.h

Abstract:

    This file contains the declarations for the 
    IrisPysx++ BRDFBase type.

--*/

#include <irisphysxplusplus.h>

#ifndef _BRDF_BASE_IRIS_PHYSX_PLUS_PLUS_
#define _BRDF_BASE_IRIS_PHYSX_PLUS_PLUS_

namespace IrisPhysx {

//
// Types
//

class BRDFBase {
protected:
    IRISPHYSXPLUSPLUSAPI
    static
    BRDF
    Create(
        _In_ std::unique_ptr<BRDFBase> BRDFBasePtr
        );

public:        
    _Ret_
    virtual
    std::tuple<IrisSpectrum::ReflectorReference, Iris::Vector, FLOAT>
    Sample(
        _In_ const Iris::Vector & Incoming,
        _In_ IrisAdvanced::RandomReference Rng,
        _In_ IrisSpectrum::ReflectorCompositorReference Compositor
        ) const = 0;
        
    _Ret_
    virtual
    std::tuple<IrisSpectrum::ReflectorReference, Iris::Vector, FLOAT>
    SampleWithLambertianFalloff(
        _In_ const Iris::Vector & Incoming,
        _In_ IrisAdvanced::RandomReference Rng,
        _In_ IrisSpectrum::ReflectorCompositorReference Compositor
        ) const = 0;
    
    _Ret_
    virtual
    IrisSpectrum::ReflectorReference
    ComputeReflectance(
        _In_ const Iris::Vector & Incoming,
        _In_ const Iris::Vector & Outgoing,
        _In_ IrisSpectrum::ReflectorCompositorReference Compositor
        ) const = 0;
    
    _Ret_
    virtual
    IrisSpectrum::ReflectorReference
    ComputeReflectanceWithLambertianFalloff(
        _In_ const Iris::Vector & Incoming,
        _In_ const Iris::Vector & Outgoing,
        _In_ IrisSpectrum::ReflectorCompositorReference Compositor
        ) const = 0;
    
    _Ret_
    virtual
    std::tuple<IrisSpectrum::ReflectorReference, FLOAT>
    ComputeReflectanceWithPdf(
        _In_ const Iris::Vector & Incoming,
        _In_ const Iris::Vector & Outgoing,
        _In_ IrisSpectrum::ReflectorCompositorReference Compositor
        ) const = 0;
    
    _Ret_
    virtual
    std::tuple<IrisSpectrum::ReflectorReference, FLOAT>
    ComputeReflectanceWithPdfWithLambertianFalloff(
        _In_ const Iris::Vector & Incoming,
        _In_ const Iris::Vector & Outgoing,
        _In_ IrisSpectrum::ReflectorCompositorReference Compositor
        ) const = 0;
    
    BRDFBase(
        _In_ const BRDFBase & ToCopy
        ) = delete;
        
    BRDFBase &
    operator=(
        _In_ const BRDFBase & ToCopy
        ) = delete;

    virtual
    ~BRDFBase(
        void
        )
    { }
};

} // namespace IrisPhysx

#endif // _BRDF_BASE_IRIS_PHYSX_PLUS_PLUS_