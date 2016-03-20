/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    spectrumcompositorreference.cpp

Abstract:

    This file contains the definitions for the 
    IrisSpectrum++ SpectrumCompositorReference type.

--*/

#include <irisspectrumplusplusp.h>

namespace IrisSpectrum {

_Ret_
SpectrumReference
SpectrumCompositorReference::AddSpectra(
    _In_ PCSPECTRUM Spectrum0Ptr,
    _In_ PCSPECTRUM Spectrum1Ptr
    )
{
    PCSPECTRUM Result;
    
    ISTATUS Status = SpectrumCompositorReferenceAddSpectra(Data,
                                                           Spectrum0Ptr,
                                                           Spectrum1Ptr,
                                                           &Result);
                                                       
    switch (Status)
    {
        case ISTATUS_SUCCESS:
            break;
        case ISTATUS_ALLOCATION_FAILED:
            throw std::bad_alloc();
            break;
        default:
            ASSERT(false);
    }
    
    return SpectrumReference(Result);
}

_Ret_
SpectrumReference
SpectrumCompositorReference::AttenuateSpectrum(
    _In_ PCSPECTRUM SpectrumPtr,
    _In_ FLOAT Attenuation
    )
{
    PCSPECTRUM Result;
    
    ISTATUS Status = SpectrumCompositorReferenceAttenuateSpectrum(Data,
                                                                  SpectrumPtr,
                                                                  Attenuation,
                                                                  &Result);
                                                       
    switch (Status)
    {
        case ISTATUS_SUCCESS:
            break;
        case ISTATUS_INVALID_ARGUMENT_02:
            throw std::invalid_argument("Attenuation");
            break;
        case ISTATUS_ALLOCATION_FAILED:
            throw std::bad_alloc();
            break;
        default:
            ASSERT(false);
    }
    
    return SpectrumReference(Result);
}

_Ret_
SpectrumReference
SpectrumCompositorReference::AttenuatedAddSpectra(
    _In_ PCSPECTRUM Spectrum0Ptr,
    _In_ PCSPECTRUM Spectrum1Ptr,
    _In_ FLOAT Attenuation
    )
{
    PCSPECTRUM Result;
    
    ISTATUS Status = SpectrumCompositorReferenceAttenuatedAddSpectra(Data,
                                                                     Spectrum0Ptr,
                                                                     Spectrum1Ptr,
                                                                     Attenuation,
                                                                     &Result);
                                                       
    switch (Status)
    {
        case ISTATUS_SUCCESS:
            break;
        case ISTATUS_INVALID_ARGUMENT_03:
            throw std::invalid_argument("Attenuation");
            break;
        case ISTATUS_ALLOCATION_FAILED:
            throw std::bad_alloc();
            break;
        default:
            ASSERT(false);
    }
    
    return SpectrumReference(Result);
}

_Ret_
SpectrumReference
SpectrumCompositorReference::AddReflection(
    _In_ PCSPECTRUM SpectrumPtr,
    _In_ PCREFLECTOR ReflectorPtr
    )
{
    PCSPECTRUM Result;
    
    ISTATUS Status = SpectrumCompositorReferenceAddReflection(Data,
                                                              SpectrumPtr,
                                                              ReflectorPtr,
                                                              &Result);
                                                       
    switch (Status)
    {
        case ISTATUS_SUCCESS:
            break;
        case ISTATUS_ALLOCATION_FAILED:
            throw std::bad_alloc();
            break;
        default:
            ASSERT(false);
    }
    
    return SpectrumReference(Result);
}

_Ret_
SpectrumReference
SpectrumCompositorReference::AttenuatedAddReflection(
    _In_ PCSPECTRUM SpectrumPtr,
    _In_ PCREFLECTOR ReflectorPtr,
    _In_ FLOAT Attenuation
    )
{
    PCSPECTRUM Result;
    
    ISTATUS Status = SpectrumCompositorReferenceAttenuatedAddReflection(Data,
                                                                        SpectrumPtr,
                                                                        ReflectorPtr,
                                                                        Attenuation,
                                                                        &Result);

    switch (Status)
    {
        case ISTATUS_SUCCESS:
            break;
        case ISTATUS_INVALID_ARGUMENT_03:
            throw std::invalid_argument("Attenuation");
            break;
        case ISTATUS_ALLOCATION_FAILED:
            throw std::bad_alloc();
            break;
        default:
            ASSERT(false);
    }
    
    return SpectrumReference(Result);
}

} // namespace IrisSpectrum