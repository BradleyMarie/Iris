/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    spectrumcompositor.cpp

Abstract:

    This file contains the definitions for the 
    IrisSpectrum++ SpectrumCompositor type.

--*/

#include <irisspectrumplusplusp.h>

namespace IrisSpectrum {

SpectrumCompositor::SpectrumCompositor(
    void
    )
{
    ISTATUS Status = SpectrumCompositorAllocate(&Data);
    
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
}

_Ret_
SpectrumReference
SpectrumCompositor::Add(
    _In_ PCSPECTRUM Spectrum0Ptr,
    _In_ PCSPECTRUM Spectrum1Ptr
    )
{
    PCSPECTRUM Result;
    
    ISTATUS Status = SpectrumCompositorAddSpectra(Data,
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
SpectrumCompositor::Attenuate(
    _In_ PCSPECTRUM SpectrumPtr,
    _In_ FLOAT Attenuation
    )
{
    PCSPECTRUM Result;
    
    ISTATUS Status = SpectrumCompositorAttenuateSpectrum(Data,
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
SpectrumCompositor::AttenuatedAdd(
    _In_ PCSPECTRUM Spectrum0Ptr,
    _In_ PCSPECTRUM Spectrum1Ptr,
    _In_ FLOAT Attenuation
    )
{
    PCSPECTRUM Result;
    
    ISTATUS Status = SpectrumCompositorAttenuatedAddSpectra(Data,
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
SpectrumCompositor::Reflect(
    _In_ PCSPECTRUM SpectrumPtr,
    _In_ PCREFLECTOR ReflectorPtr
    )
{
    PCSPECTRUM Result;
    
    ISTATUS Status = SpectrumCompositorAddReflection(Data,
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
SpectrumCompositor::AttenuatedReflect(
    _In_ PCSPECTRUM SpectrumPtr,
    _In_ PCREFLECTOR ReflectorPtr,
    _In_ FLOAT Attenuation
    )
{
    PCSPECTRUM Result;
    
    ISTATUS Status = SpectrumCompositorAttenuatedAddReflection(Data,
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