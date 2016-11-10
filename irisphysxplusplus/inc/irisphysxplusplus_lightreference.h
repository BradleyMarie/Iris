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
        _In_ PCPHYSX_LIGHT LightPtr
        )
    : Data(LightPtr)
    { }
    
    _Ret_
    PCPHYSX_LIGHT
    AsPCPHYSX_LIGHT(
        void
        ) const
    {
        return Data;
    }
    
    _Ret_
    std::tuple<IrisSpectrum::SpectrumReference, Iris::Vector, FLOAT>
    Sample(
        _In_ const Iris::Point & HitPoint,
        _In_ VisibilityTester Tester,
        _In_ IrisAdvanced::RandomReference Rng,
        _In_ IrisSpectrum::SpectrumCompositorReference Compositor
        ) const
    {
        PCSPECTRUM ResultSpectrum;
        VECTOR3 ResultToLight;
        FLOAT ResultPdf;

        ISTATUS Status = PhysxLightSample(Data,
                                          HitPoint.AsPOINT3(),
                                          Tester.AsPPHYSX_VISIBILITY_TESTER(),
                                          Rng.AsPRANDOM_REFERENCE(),
                                          Compositor.AsPSPECTRUM_COMPOSITOR_REFERENCE(),
                                          &ResultSpectrum,
                                          &ResultToLight,
                                          &ResultPdf);

        if (Status != ISTATUS_SUCCESS)
        {
            throw std::runtime_error(Iris::ISTATUSToCString(Status));
        }

        return std::make_tuple(IrisSpectrum::SpectrumReference(ResultSpectrum), Iris::Vector(ResultToLight), ResultPdf);
    }

    _Ret_
    IrisSpectrum::SpectrumReference
    ComputeEmissive(
        _In_ const Iris::Ray & ToLight,
        _In_ VisibilityTester Tester,
        _In_ IrisSpectrum::SpectrumCompositorReference Compositor
        ) const
    {
        PCSPECTRUM Result;

        ISTATUS Status = PhysxLightComputeEmissive(Data,
                                                   ToLight.AsRAY(),
                                                   Tester.AsPPHYSX_VISIBILITY_TESTER(),
                                                   Compositor.AsPSPECTRUM_COMPOSITOR_REFERENCE(),
                                                   &Result);

        if (Status != ISTATUS_SUCCESS)
        {
            throw std::runtime_error(Iris::ISTATUSToCString(Status));
        }

        return IrisSpectrum::SpectrumReference(Result);
    }
    
    _Ret_
    std::tuple<IrisSpectrum::SpectrumReference, FLOAT>
    ComputeEmissiveWithPdf(
        _In_ const Iris::Ray & ToLight,
        _In_ VisibilityTester Tester,
        _In_ IrisSpectrum::SpectrumCompositorReference Compositor
        ) const
    {
        PCSPECTRUM ResultSpectrum;
        FLOAT ResultPdf;

        ISTATUS Status = PhysxLightComputeEmissiveWithPdf(Data,
                                                          ToLight.AsRAY(),
                                                          Tester.AsPPHYSX_VISIBILITY_TESTER(),
                                                          Compositor.AsPSPECTRUM_COMPOSITOR_REFERENCE(),
                                                          &ResultSpectrum,
                                                          &ResultPdf);

        if (Status != ISTATUS_SUCCESS)
        {
            throw std::runtime_error(Iris::ISTATUSToCString(Status));
        }

        return std::make_tuple(IrisSpectrum::SpectrumReference(ResultSpectrum), ResultPdf);
    }
    
private:
    PCPHYSX_LIGHT Data;
};

} // namespace Iris

#endif // _LIGHT_REFERENCE_IRIS_PHYSX_PLUS_PLUS_
