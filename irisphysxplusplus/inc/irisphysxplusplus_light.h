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
        _In_ PPBR_LIGHT LightPtr,
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
            PbrLightRetain(LightPtr);
        }
    }
    
    _Ret_
    PPBR_LIGHT
    AsPPBR_LIGHT(
        void
        )
    {
        return Data;
    }
    
    _Ret_
    PCPBR_LIGHT
    AsPCPBR_LIGHT(
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

        ISTATUS Status = PbrLightSample(Data,
                                        HitPoint.AsPOINT3(),
                                        Tester.AsPPBR_VISIBILITY_TESTER(),
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
        _In_ VisibilityTester Tester
        ) const
    {
        PCSPECTRUM Result;

        ISTATUS Status = PbrLightComputeEmissive(Data,
                                                 ToLight.AsRAY(),
                                                 Tester.AsPPBR_VISIBILITY_TESTER(),
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
        _In_ VisibilityTester Tester
        ) const
    {
        PCSPECTRUM ResultSpectrum;
        FLOAT ResultPdf;

        ISTATUS Status = PbrLightComputeEmissiveWithPdf(Data,
                                                        ToLight.AsRAY(),
                                                        Tester.AsPPBR_VISIBILITY_TESTER(),
                                                        &ResultSpectrum,
                                                        &ResultPdf);

        if (Status != ISTATUS_SUCCESS)
        {
            throw std::runtime_error(Iris::ISTATUSToCString(Status));
        }

        return std::make_tuple(IrisSpectrum::SpectrumReference(ResultSpectrum), ResultPdf);
    }
    
    ~Light(
        void
        )
    {
        PbrLightRelease(Data);    
    }
    
private:
    PPBR_LIGHT Data;
};

//
// Define VisibilityTester functions
//

_Ret_
inline
bool
VisibilityTester::Test(
    _In_ const Iris::Ray & WorldRay,
    _In_ const Light & LightRef
    )
{
    BOOL Result;

    ISTATUS Status = PBRVisibilityTesterTestLightVisibility(Data,
                                                            WorldRay.AsRAY(),
                                                            LightRef.AsPCPBR_LIGHT(),
                                                            &Result);

    if (Status != ISTATUS_SUCCESS)
    {
        throw std::runtime_error(Iris::ISTATUSToCString(Status));
    }

    return Result != FALSE;
}

} // namespace Iris

#endif // _LIGHT_IRIS_PHYSX_PLUS_PLUS_