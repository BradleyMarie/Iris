/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    irisspectrumplusplus_reflectorcompositorreference.h

Abstract:

    This file contains the definitions for the 
    IrisSpectrum++ ReflectorCompositorReference type.

--*/

#include <irisspectrumplusplus.h>

#ifndef _REFLECTOR_COMPOSITOR_REFERENCE_IRIS_SPECTRUM_PLUS_PLUS_
#define _REFLECTOR_COMPOSITOR_REFERENCE_IRIS_SPECTRUM_PLUS_PLUS_

namespace IrisSpectrum {

//
// Types
//

class ReflectorCompositorReference final {
public:
    ReflectorCompositorReference(
        _In_ PREFLECTOR_COMPOSITOR_REFERENCE ReflectorCompositorReferencePtr
        )
    : Data(ReflectorCompositorReferencePtr)
    { 
        if (ReflectorCompositorReferencePtr == nullptr)
        {
            throw std::invalid_argument("ReflectorCompositorReferencePtr");
        }
    }
    
    _Ret_
    PREFLECTOR_COMPOSITOR_REFERENCE
    AsPREFLECTOR_COMPOSITOR_REFERENCE(
        void
        )
    {
        return Data;
    }

    _Ret_
    ReflectorReference
    Add(
        _In_ const Reflector & Reflector0Ref,
        _In_ const Reflector & Reflector1Ref
        )
    {
        ReflectorReference Result = Add(Reflector0Ref.AsPCREFLECTOR(),
                                        Reflector1Ref.AsPCREFLECTOR());
        
        return Result;
    }
    
    _Ret_
    ReflectorReference
    Add(
        _In_ const Reflector & Reflector0Ref,
        _In_ const ReflectorReference & Reflector1Ref
        )
    {
        ReflectorReference Result = Add(Reflector0Ref.AsPCREFLECTOR(),
                                        Reflector1Ref.AsPCREFLECTOR());
        
        return Result;
    }
    
    _Ret_
    ReflectorReference
    Add(
        _In_ const ReflectorReference & Reflector0Ref,
        _In_ const Reflector & Reflector1Ref
        )
    {
        ReflectorReference Result = Add(Reflector0Ref.AsPCREFLECTOR(),
                                        Reflector1Ref.AsPCREFLECTOR());
        
        return Result;
    }
    
    _Ret_
    ReflectorReference
    Add(
        _In_ const ReflectorReference & Reflector0Ref,
        _In_ const ReflectorReference & Reflector1Ref
        )
    {
        ReflectorReference Result = Add(Reflector0Ref.AsPCREFLECTOR(),
                                        Reflector1Ref.AsPCREFLECTOR());
        
        return Result;
    }
    
    _Ret_
    ReflectorReference
    Attenuate(
        _In_ const Reflector & ReflectorRef,
        _In_ FLOAT Attenuation
        )
    {
        ReflectorReference Result = Attenuate(ReflectorRef.AsPCREFLECTOR(),
                                              Attenuation);
        
        return Result;
    }
    
    _Ret_
    ReflectorReference
    Attenuate(
        _In_ const ReflectorReference & ReflectorRef,
        _In_ FLOAT Attenuation
        )
    {
        ReflectorReference Result = Attenuate(ReflectorRef.AsPCREFLECTOR(),
                                              Attenuation);
        
        return Result;
    }
    
    ReflectorCompositorReference(
        _In_ ReflectorCompositorReference & ToCopy
        )
    : Data(ToCopy.Data)
    { }
        
    ReflectorCompositorReference &
    operator=(
        _In_ const ReflectorCompositorReference & ToCopy
        )
    {
        Data = ToCopy.Data;
    }

private:
    PREFLECTOR_COMPOSITOR_REFERENCE Data;

    _Ret_
    ReflectorReference
    Add(
        _In_opt_ PCREFLECTOR Reflector0Ptr,
        _In_opt_ PCREFLECTOR Reflector1Ptr
        )
    {
        PCREFLECTOR Result;
        
        ISTATUS Status = ReflectorCompositorReferenceAddReflections(Data,
                                                                    Reflector0Ptr,
                                                                    Reflector1Ptr,
                                                                    &Result);

        if (Status != ISTATUS_SUCCESS)
        {
            assert(Status == ISTATUS_ALLOCATION_FAILED);
            throw std::bad_alloc();
        } 
        
        return ReflectorReference(Result);
    }

    _Ret_
    ReflectorReference
    Attenuate(
        _In_opt_ PCREFLECTOR ReflectorPtr,
        _In_opt_ FLOAT Attenuation
        )
    {
        PCREFLECTOR Result;
        
        ISTATUS Status = ReflectorCompositorReferenceAttenuateReflection(Data,
                                                                         ReflectorPtr,
                                                                         Attenuation,
                                                                         &Result);

        if (Status == ISTATUS_SUCCESS)
        {
            return ReflectorReference(Result);
        } 

        switch (Status)
        {
            case ISTATUS_INVALID_ARGUMENT_02:
                throw std::invalid_argument("Attenuation");
            default:
                assert(Status == ISTATUS_ALLOCATION_FAILED);
                throw std::bad_alloc();
        }
    }
};

} // namespace IrisSpectrum

#endif // _REFLECTOR_COMPOSITOR_REFERENCE_IRIS_SPECTRUM_PLUS_PLUS_