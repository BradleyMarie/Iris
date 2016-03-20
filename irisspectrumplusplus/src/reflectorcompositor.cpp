/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    reflectorcompositor.cpp

Abstract:

    This file contains the definitions for the 
    IrisAdvanced++ ReflectorCompositor type.

--*/

#include <irisspectrumplusplusp.h>

namespace IrisSpectrum {

ReflectorCompositor::ReflectorCompositor(
    void
    )
{
    ISTATUS Status = ReflectorCompositorAllocate(&Data);
    
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
ReflectorReference
ReflectorCompositor::Add(
    _In_ PCREFLECTOR Reflector0Ptr,
    _In_ PCREFLECTOR Reflector1Ptr
    )
{
    PCREFLECTOR Result;
    
    ISTATUS Status = ReflectorCompositorAddReflections(Data,
                                                       Reflector0Ptr,
                                                       Reflector1Ptr,
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
    
    return ReflectorReference(Result);
}

_Ret_
ReflectorReference
ReflectorCompositor::Attenuate(
    _In_ PCREFLECTOR ReflectorPtr,
    _In_ FLOAT Attenuation
    )
{
    PCREFLECTOR Result;
    
    ISTATUS Status = ReflectorCompositorAttenuateReflection(Data,
                                                            ReflectorPtr,
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
    
    return ReflectorReference(Result);
}

_Ret_
ReflectorReference
ReflectorCompositor::AttenuatedAdd(
    _In_ PCREFLECTOR Reflector0Ptr,
    _In_ PCREFLECTOR Reflector1Ptr,
    _In_ FLOAT Attenuation
    )
{
    PCREFLECTOR Result;
    
    ISTATUS Status = ReflectorCompositorAttenuatedAddReflections(Data,
                                                                 Reflector0Ptr,
                                                                 Reflector1Ptr,
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
    
    return ReflectorReference(Result);
}

} // namespace IrisSpectrum