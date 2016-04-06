/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    brdf.cpp

Abstract:

    This file contains the definitions for the 
    IrisPysx++ BRDF type.

--*/

#include <irisphysxplusplusp.h>
namespace IrisPhysx {

BRDF::BRDF(
    _In_ PPBR_BRDF PbrBrdf,
    _In_ bool Retain
    )
: Data(PbrBrdf)
{ 
    if (PbrBrdf == nullptr)
    {
        throw std::invalid_argument("PbrBrdf");
    }
    
    if (Retain)
    {
        PbrBrdfRetain(Data);
    }
}

_Ret_
std::tuple<IrisSpectrum::ReflectorReference, Iris::Vector, FLOAT>
BRDF::Sample(
    _In_ const Iris::Vector & Incoming,
    _In_ IrisAdvanced::RandomReference Rng,
    _In_ IrisSpectrum::ReflectorCompositorReference Compositor
    ) const
{
    VECTOR3 Outgoing;
    FLOAT Pdf;
    PCREFLECTOR Reflector;
    ISTATUS Status;
    
    Status = PbrBrdfSample(Data,
                           Incoming.AsVECTOR3(),
                           Rng.AsPRANDOM_REFERENCE(),
                           Compositor.AsPREFLECTOR_COMPOSITOR_REFERENCE(),
                           &Reflector,
                           &Outgoing,
                           &Pdf);

    switch (Status)
    {
        case ISTATUS_SUCCESS:
            break;
        case ISTATUS_INVALID_ARGUMENT_01:
            throw std::invalid_argument("Incoming");
            break;
        case ISTATUS_ALLOCATION_FAILED:
            throw std::bad_alloc();
            break;
        default:
            ASSERT(false);
    }
    
    return std::make_tuple(IrisSpectrum::ReflectorReference(Reflector), Iris::Vector(Outgoing), Pdf);
}

_Ret_
std::tuple<IrisSpectrum::ReflectorReference, Iris::Vector, FLOAT>
BRDF::SampleWithLambertianFalloff(
    _In_ const Iris::Vector & Incoming,
    _In_ IrisAdvanced::RandomReference Rng,
    _In_ IrisSpectrum::ReflectorCompositorReference Compositor
    ) const
{
    VECTOR3 Outgoing;
    FLOAT Pdf;
    PCREFLECTOR Reflector;
    ISTATUS Status;
    
    Status = PbrBrdfSampleWithLambertianFalloff(Data,
                                                Incoming.AsVECTOR3(),
                                                Rng.AsPRANDOM_REFERENCE(),
                                                Compositor.AsPREFLECTOR_COMPOSITOR_REFERENCE(),
                                                &Reflector,
                                                &Outgoing,
                                                &Pdf);

    switch (Status)
    {
        case ISTATUS_SUCCESS:
            break;
        case ISTATUS_INVALID_ARGUMENT_01:
            throw std::invalid_argument("Incoming");
            break;
        case ISTATUS_ALLOCATION_FAILED:
            throw std::bad_alloc();
            break;
        default:
            ASSERT(false);
    }
    
    return std::make_tuple(IrisSpectrum::ReflectorReference(Reflector), Iris::Vector(Outgoing), Pdf);
}

_Ret_
IrisSpectrum::ReflectorReference
BRDF::ComputeReflectance(
    _In_ const Iris::Vector & Incoming,
    _In_ const Iris::Vector & Outgoing,
    _In_ IrisSpectrum::ReflectorCompositorReference Compositor
    ) const
{
    PCREFLECTOR Reflector;
    ISTATUS Status;
    
    Status = PbrBrdfComputeReflectance(Data,
                                       Incoming.AsVECTOR3(),
                                       Outgoing.AsVECTOR3(),
                                       Compositor.AsPREFLECTOR_COMPOSITOR_REFERENCE(),
                                       &Reflector);

    switch (Status)
    {
        case ISTATUS_SUCCESS:
            break;
        case ISTATUS_INVALID_ARGUMENT_01:
            throw std::invalid_argument("Incoming");
            break;
        case ISTATUS_INVALID_ARGUMENT_02:
            throw std::invalid_argument("Outgoing");
            break;
        case ISTATUS_ALLOCATION_FAILED:
            throw std::bad_alloc();
            break;
        default:
            ASSERT(false);
    }
    
    return IrisSpectrum::ReflectorReference(Reflector);
}

_Ret_
IrisSpectrum::ReflectorReference
BRDF::ComputeReflectanceWithLambertianFalloff(
    _In_ const Iris::Vector & Incoming,
    _In_ const Iris::Vector & Outgoing,
    _In_ IrisSpectrum::ReflectorCompositorReference Compositor
    ) const
{
    PCREFLECTOR Reflector;
    ISTATUS Status;
    
    Status = PbrBrdfComputeReflectanceWithLambertianFalloff(Data,
                                                            Incoming.AsVECTOR3(),
                                                            Outgoing.AsVECTOR3(),
                                                            Compositor.AsPREFLECTOR_COMPOSITOR_REFERENCE(),
                                                            &Reflector);

    switch (Status)
    {
        case ISTATUS_SUCCESS:
            break;
        case ISTATUS_INVALID_ARGUMENT_01:
            throw std::invalid_argument("Incoming");
            break;
        case ISTATUS_INVALID_ARGUMENT_02:
            throw std::invalid_argument("Outgoing");
            break;
        case ISTATUS_ALLOCATION_FAILED:
            throw std::bad_alloc();
            break;
        default:
            ASSERT(false);
    }
    
    return IrisSpectrum::ReflectorReference(Reflector);
}

_Ret_
std::tuple<IrisSpectrum::ReflectorReference, FLOAT>
BRDF::ComputeReflectanceWithPdf(
    _In_ const Iris::Vector & Incoming,
    _In_ const Iris::Vector & Outgoing,
    _In_ IrisSpectrum::ReflectorCompositorReference Compositor
    ) const
{
    FLOAT Pdf;
    PCREFLECTOR Reflector;
    ISTATUS Status;
    
    Status = PbrBrdfComputeReflectanceWithPdf(Data,
                                              Incoming.AsVECTOR3(),
                                              Outgoing.AsVECTOR3(),
                                              Compositor.AsPREFLECTOR_COMPOSITOR_REFERENCE(),
                                              &Reflector,
                                              &Pdf);

    switch (Status)
    {
        case ISTATUS_SUCCESS:
            break;
        case ISTATUS_INVALID_ARGUMENT_01:
            throw std::invalid_argument("Incoming");
            break;
        case ISTATUS_INVALID_ARGUMENT_02:
            throw std::invalid_argument("Outgoing");
            break;
        case ISTATUS_ALLOCATION_FAILED:
            throw std::bad_alloc();
            break;
        default:
            ASSERT(false);
    }
    
    return std::make_tuple(IrisSpectrum::ReflectorReference(Reflector), Pdf);
}

_Ret_
std::tuple<IrisSpectrum::ReflectorReference, FLOAT>
BRDF::ComputeReflectanceWithPdfWithLambertianFalloff(
    _In_ const Iris::Vector & Incoming,
    _In_ const Iris::Vector & Outgoing,
    _In_ IrisSpectrum::ReflectorCompositorReference Compositor
    ) const
{
    FLOAT Pdf;
    PCREFLECTOR Reflector;
    ISTATUS Status;
    
    Status = PbrBrdfComputeReflectanceWithPdfWithLambertianFalloff(Data,
                                                                   Incoming.AsVECTOR3(),
                                                                   Outgoing.AsVECTOR3(),
                                                                   Compositor.AsPREFLECTOR_COMPOSITOR_REFERENCE(),
                                                                   &Reflector,
                                                                   &Pdf);

    switch (Status)
    {
        case ISTATUS_SUCCESS:
            break;
        case ISTATUS_INVALID_ARGUMENT_01:
            throw std::invalid_argument("Incoming");
            break;
        case ISTATUS_INVALID_ARGUMENT_02:
            throw std::invalid_argument("Outgoing");
            break;
        case ISTATUS_ALLOCATION_FAILED:
            throw std::bad_alloc();
            break;
        default:
            ASSERT(false);
    }
    
    return std::make_tuple(IrisSpectrum::ReflectorReference(Reflector), Pdf);
}

} // namespace IrisPhysx