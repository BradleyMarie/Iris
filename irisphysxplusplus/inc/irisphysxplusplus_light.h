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
// Types
//

class Light final {
public:
    Light(
        _In_ PPHYSX_LIGHT LightPtr,
        _In_ bool Retain
        )
    : Data(LightPtr)
    {
        if (LightPtr == nullptr)
        {
            throw std::invalid_argument("LightPtr");
        }

        if (Retain)
        {
            PhysxLightRetain(LightPtr);
        }
    }

    Light(
        _In_ Light && ToMove
        )
    : Data(ToMove.Data)
    {
        ToMove.Data = nullptr;
    }
    
    _Ret_
    PPHYSX_LIGHT
    AsPPHYSX_LIGHT(
        void
        )
    {
        return Data;
    }
    
    _Ret_
    PCPHYSX_LIGHT
    AsPCPHYSX_LIGHT(
        void
        ) const
    {
        return Data;
    }

    std::tuple<IrisSpectrum::SpectrumReference, Iris::Vector, FLOAT>
    Sample(
        _In_ const Iris::Point & HitPoint,
        _In_ VisibilityTester Tester,
        _In_ IrisAdvanced::Random Rng,
        _In_ IrisSpectrum::SpectrumCompositorReference Compositor
        ) const
    {
        PCSPECTRUM ResultSpectrum;
        VECTOR3 ResultToLight;
        FLOAT ResultPdf;

        ISTATUS Status = PhysxLightSample(Data,
                                          HitPoint.AsPOINT3(),
                                          Tester.AsPPHYSX_VISIBILITY_TESTER(),
                                          Rng.AsPRANDOM(),
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

    Light(
        _In_ const Light & ToCopy
        )
    : Data(ToCopy.Data)
    {
        PhysxLightRetain(Data);
    }

    Light & 
    operator=(
        _In_ const Light & ToCopy
        )
    {
        if (this != &ToCopy)
        {
            PhysxLightRelease(Data);
            Data = ToCopy.Data;
            PhysxLightRetain(Data);
        }

        return *this;
    }
    
    ~Light(
        void
        )
    {
        PhysxLightRelease(Data);
    }
    
private:
    PPHYSX_LIGHT Data;
};

} // namespace Iris

#endif // _LIGHT_IRIS_PHYSX_PLUS_PLUS_
