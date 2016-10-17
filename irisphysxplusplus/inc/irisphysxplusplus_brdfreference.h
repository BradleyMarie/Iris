/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    irisphysxplusplus_brdfreference.h

Abstract:

    This file contains the declarations for the 
    IrisPysx++ BRDFReference type.

--*/

#include <irisphysxplusplus.h>

#ifndef _BRDF_REFERENCE_IRIS_PHYSX_PLUS_PLUS_
#define _BRDF_REFERENCE_IRIS_PHYSX_PLUS_PLUS_

namespace IrisPhysx {

//
// Types
//

class BRDFReference final {
public:
    BRDFReference(
        _In_ PCPHYSX_BRDF Brdf
        )
    : Data(Brdf)
    { 
        if (Brdf == nullptr)
        {
            throw std::invalid_argument("Brdf");
        }
    }
     
    _Ret_
    PCPHYSX_BRDF
    AsPCPHYSX_BRDF(
        void
        ) const
    {
        return Data;
    }
        
    _Ret_
    std::tuple<IrisSpectrum::ReflectorReference, Iris::Vector, FLOAT>
    Sample(
        _In_ const Iris::Vector & Incoming,
        _In_ const Iris::Vector & SurfaceNormal,
        _In_ IrisAdvanced::RandomReference Rng,
        _In_ IrisSpectrum::ReflectorCompositorReference Compositor
        ) const
    {
        VECTOR3 Outgoing;
        FLOAT Pdf;
        PCREFLECTOR Reflector;
        ISTATUS Status;
        
        Status = PhysxBrdfSample(Data,
                                 Incoming.AsVECTOR3(),
                                 SurfaceNormal.AsVECTOR3(),
                                 Rng.AsPRANDOM_REFERENCE(),
                                 Compositor.AsPREFLECTOR_COMPOSITOR_REFERENCE(),
                                 &Reflector,
                                 &Outgoing,
                                 &Pdf);

        if (Status != ISTATUS_SUCCESS)
        {
            throw std::runtime_error(Iris::ISTATUSToCString(Status));
        }
        
        return std::make_tuple(IrisSpectrum::ReflectorReference(Reflector), Iris::Vector(Outgoing), Pdf);
    }
        
    _Ret_
    std::tuple<IrisSpectrum::ReflectorReference, Iris::Vector, FLOAT>
    SampleWithLambertianFalloff(
        _In_ const Iris::Vector & Incoming,
        _In_ const Iris::Vector & SurfaceNormal,
        _In_ IrisAdvanced::RandomReference Rng,
        _In_ IrisSpectrum::ReflectorCompositorReference Compositor
        ) const
    {
        VECTOR3 Outgoing;
        FLOAT Pdf;
        PCREFLECTOR Reflector;
        ISTATUS Status;
        
        Status = PhysxBrdfSampleWithLambertianFalloff(Data,
                                                      Incoming.AsVECTOR3(),
                                                      SurfaceNormal.AsVECTOR3(),
                                                      Rng.AsPRANDOM_REFERENCE(),
                                                      Compositor.AsPREFLECTOR_COMPOSITOR_REFERENCE(),
                                                      &Reflector,
                                                      &Outgoing,
                                                      &Pdf);

        if (Status != ISTATUS_SUCCESS)
        {
            throw std::runtime_error(Iris::ISTATUSToCString(Status));
        }
        
        return std::make_tuple(IrisSpectrum::ReflectorReference(Reflector), Iris::Vector(Outgoing), Pdf);
    }
    
    _Ret_
    IrisSpectrum::ReflectorReference
    ComputeReflectance(
        _In_ const Iris::Vector & Incoming,
        _In_ const Iris::Vector & SurfaceNormal,
        _In_ const Iris::Vector & Outgoing,
        _In_ IrisSpectrum::ReflectorCompositorReference Compositor
        ) const
    {
        PCREFLECTOR Reflector;
        ISTATUS Status;
        
        Status = PhysxBrdfComputeReflectance(Data,
                                             Incoming.AsVECTOR3(),
                                             SurfaceNormal.AsVECTOR3(),
                                             Outgoing.AsVECTOR3(),
                                             Compositor.AsPREFLECTOR_COMPOSITOR_REFERENCE(),
                                             &Reflector);

        if (Status != ISTATUS_SUCCESS)
        {
            throw std::runtime_error(Iris::ISTATUSToCString(Status));
        }
        
        return IrisSpectrum::ReflectorReference(Reflector);
    }
    
    _Ret_
    IrisSpectrum::ReflectorReference
    ComputeReflectanceWithLambertianFalloff(
        _In_ const Iris::Vector & Incoming,
        _In_ const Iris::Vector & SurfaceNormal,
        _In_ const Iris::Vector & Outgoing,
        _In_ IrisSpectrum::ReflectorCompositorReference Compositor
        ) const
    {
        PCREFLECTOR Reflector;
        ISTATUS Status;
        
        Status = PhysxBrdfComputeReflectanceWithLambertianFalloff(Data,
                                                                  Incoming.AsVECTOR3(),
                                                                  SurfaceNormal.AsVECTOR3(),
                                                                  Outgoing.AsVECTOR3(),
                                                                  Compositor.AsPREFLECTOR_COMPOSITOR_REFERENCE(),
                                                                  &Reflector);

        if (Status != ISTATUS_SUCCESS)
        {
            throw std::runtime_error(Iris::ISTATUSToCString(Status));
        }
        
        return IrisSpectrum::ReflectorReference(Reflector);
    }
    
    _Ret_
    std::tuple<IrisSpectrum::ReflectorReference, FLOAT>
    ComputeReflectanceWithPdf(
        _In_ const Iris::Vector & Incoming,
        _In_ const Iris::Vector & SurfaceNormal,
        _In_ const Iris::Vector & Outgoing,
        _In_ IrisSpectrum::ReflectorCompositorReference Compositor
        ) const
    {
        FLOAT Pdf;
        PCREFLECTOR Reflector;
        ISTATUS Status;
        
        Status = PhysxBrdfComputeReflectanceWithPdf(Data,
                                                    Incoming.AsVECTOR3(),
                                                    SurfaceNormal.AsVECTOR3(),
                                                    Outgoing.AsVECTOR3(),
                                                    Compositor.AsPREFLECTOR_COMPOSITOR_REFERENCE(),
                                                    &Reflector,
                                                    &Pdf);

        if (Status != ISTATUS_SUCCESS)
        {
            throw std::runtime_error(Iris::ISTATUSToCString(Status));
        }
        
        return std::make_tuple(IrisSpectrum::ReflectorReference(Reflector), Pdf);
    }
    
    _Ret_
    std::tuple<IrisSpectrum::ReflectorReference, FLOAT>
    ComputeReflectanceWithPdfWithLambertianFalloff(
        _In_ const Iris::Vector & Incoming,
        _In_ const Iris::Vector & SurfaceNormal,
        _In_ const Iris::Vector & Outgoing,
        _In_ IrisSpectrum::ReflectorCompositorReference Compositor
        ) const
    {
        FLOAT Pdf;
        PCREFLECTOR Reflector;
        ISTATUS Status;
        
        Status = PhysxBrdfComputeReflectanceWithPdfWithLambertianFalloff(Data,
                                                                         Incoming.AsVECTOR3(),
                                                                         SurfaceNormal.AsVECTOR3(),
                                                                         Outgoing.AsVECTOR3(),
                                                                         Compositor.AsPREFLECTOR_COMPOSITOR_REFERENCE(),
                                                                         &Reflector,
                                                                         &Pdf);

        if (Status != ISTATUS_SUCCESS)
        {
            throw std::runtime_error(Iris::ISTATUSToCString(Status));
        }
        
        return std::make_tuple(IrisSpectrum::ReflectorReference(Reflector), Pdf);
    }
    
private:
    PCPHYSX_BRDF Data;
};

} // namespace Iris

#endif // _BRDF_REFERENCE_IRIS_PHYSX_PLUS_PLUS_
