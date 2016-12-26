/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    irisspectrumplusplus_spectrumcompositor.h

Abstract:

    This file contains the definitions for the 
    IrisSpectrum++ SpectrumCompositor type.

--*/

#include <irisspectrumplusplus.h>

#ifndef _SPECTRUM_COMPOSITOR_IRIS_SPECTRUM_PLUS_PLUS_
#define _SPECTRUM_COMPOSITOR_IRIS_SPECTRUM_PLUS_PLUS_

namespace IrisSpectrum {

//
// Types
//

class SpectrumCompositor final {
public:
    SpectrumCompositor(
        _In_ SpectrumCompositor && ToMove
        )
        : Data(ToMove.Data)
    {
        ToMove.Data = nullptr;
    }

    static
    SpectrumCompositor
    Create(
        void
        )
    {
        return SpectrumCompositor();
    }
    
    _Ret_
    PSPECTRUM_COMPOSITOR
    AsPSPECTRUM_COMPOSITOR(
        void
        )
    {
        return Data;
    }
    
    _Ret_
    SpectrumReference
    Add(
        _In_ const Spectrum & Spectrum0Ref,
        _In_ const Spectrum & Spectrum1Ref
        )
    {
        SpectrumReference Result = Add(Spectrum0Ref.AsPCSPECTRUM(),
                                       Spectrum1Ref.AsPCSPECTRUM());
        
        return Result;
    }
    
    _Ret_
    SpectrumReference
    Add(
        _In_ const Spectrum & Spectrum0Ref,
        _In_ SpectrumReference Spectrum1Ref
        )
    {
        SpectrumReference Result = Add(Spectrum0Ref.AsPCSPECTRUM(),
                                       Spectrum1Ref.AsPCSPECTRUM());
        
        return Result;
    }
    
    _Ret_
    SpectrumReference
    Add(
        _In_ SpectrumReference Spectrum0Ref,
        _In_ const Spectrum & Spectrum1Ref
        )
    {
        SpectrumReference Result = Add(Spectrum0Ref.AsPCSPECTRUM(),
                                       Spectrum1Ref.AsPCSPECTRUM());
        
        return Result;
    }
    
    _Ret_
    SpectrumReference
    Add(
        _In_ SpectrumReference Spectrum0Ref,
        _In_ SpectrumReference Spectrum1Ref
        )
    {
        SpectrumReference Result = Add(Spectrum0Ref.AsPCSPECTRUM(),
                                       Spectrum1Ref.AsPCSPECTRUM());
        
        return Result;
    }
    
    _Ret_
    SpectrumReference
    Attenuate(
        _In_ const Spectrum & SpectrumRef,
        _In_ FLOAT Attenuation
        )
    {
        SpectrumReference Result = Attenuate(SpectrumRef.AsPCSPECTRUM(),
                                             Attenuation);
        
        return Result;
    }
    
    _Ret_
    SpectrumReference
    Attenuate(
        _In_ SpectrumReference SpectrumRef,
        _In_ FLOAT Attenuation
        )
    {
        SpectrumReference Result = Attenuate(SpectrumRef.AsPCSPECTRUM(),
                                             Attenuation);
        
        return Result;
    }
    
    _Ret_
    SpectrumReference
    Reflect(
        _In_ const Spectrum & SpectrumRef,
        _In_ const Reflector & ReflectorRef
        )
    {
        SpectrumReference Result = Reflect(SpectrumRef.AsPCSPECTRUM(),
                                           ReflectorRef.AsPCREFLECTOR());
        
        return Result;
    }

    _Ret_
    SpectrumReference
    Reflect(
        _In_ SpectrumReference SpectrumRef,
        _In_ const Reflector & ReflectorRef
        )
    {
        SpectrumReference Result = Reflect(SpectrumRef.AsPCSPECTRUM(),
                                           ReflectorRef.AsPCREFLECTOR());
        
        return Result;
    }

    _Ret_
    SpectrumReference
    Reflect(
        _In_ const Spectrum & SpectrumRef,
        _In_ ReflectorReference ReflectorRef
        )
    {
        SpectrumReference Result = Reflect(SpectrumRef.AsPCSPECTRUM(),
                                           ReflectorRef.AsPCREFLECTOR());
        
        return Result;
    }

    _Ret_
    SpectrumReference
    Reflect(
        _In_ SpectrumReference SpectrumRef,
        _In_ ReflectorReference ReflectorRef
        )
    {
        SpectrumReference Result = Reflect(SpectrumRef.AsPCSPECTRUM(),
                                           ReflectorRef.AsPCREFLECTOR());
        
        return Result;
    }

    _Ret_
    SpectrumReference
    AttenuatedReflect(
        _In_ const Spectrum & SpectrumRef,
        _In_ const Reflector & ReflectorRef,
        _In_ FLOAT Attenuation
        )
    {
        SpectrumReference Result = AttenuatedReflect(SpectrumRef.AsPCSPECTRUM(),
                                                     ReflectorRef.AsPCREFLECTOR(),
                                                     Attenuation);
        
        return Result;
    }
    
    _Ret_
    SpectrumReference
    AttenuatedReflect(
        _In_ SpectrumReference SpectrumRef,
        _In_ const Reflector & ReflectorRef,
        _In_ FLOAT Attenuation
        )
    {
        SpectrumReference Result = AttenuatedReflect(SpectrumRef.AsPCSPECTRUM(),
                                                     ReflectorRef.AsPCREFLECTOR(),
                                                     Attenuation);
        
        return Result;
    }
    
    _Ret_
    SpectrumReference
    AttenuatedReflect(
        _In_ const Spectrum & SpectrumRef,
        _In_ ReflectorReference ReflectorRef,
        _In_ FLOAT Attenuation
        )
    {
        SpectrumReference Result = AttenuatedReflect(SpectrumRef.AsPCSPECTRUM(),
                                                     ReflectorRef.AsPCREFLECTOR(),
                                                     Attenuation);
        
        return Result;
    }
    
    _Ret_
    SpectrumReference
    AttenuatedReflect(
        _In_ SpectrumReference SpectrumRef,
        _In_ ReflectorReference ReflectorRef,
        _In_ FLOAT Attenuation
        )
    {
        SpectrumReference Result = AttenuatedReflect(SpectrumRef.AsPCSPECTRUM(),
                                                     ReflectorRef.AsPCREFLECTOR(),
                                                     Attenuation);
        
        return Result;
    }
    
    void
    Clear(
        void
        )
    {
        SpectrumCompositorClear(Data);
    }
    
    SpectrumCompositor(
        _In_ const SpectrumCompositor & ToCopy
        ) = delete;
        
    SpectrumCompositor &
    operator=(
        _In_ const SpectrumCompositor & ToCopy
        ) = delete;
    
    ~SpectrumCompositor(
        void
        )
     {
         SpectrumCompositorFree(Data);
     }

private:
    PSPECTRUM_COMPOSITOR Data;

    _Ret_
    SpectrumReference
    Add(
        _In_opt_ PCSPECTRUM Spectrum0Ptr,
        _In_opt_ PCSPECTRUM Spectrum1Ptr
        )
    {
        PCSPECTRUM Result;
        
        ISTATUS Status = SpectrumCompositorAddSpectra(Data,
                                                      Spectrum0Ptr,
                                                      Spectrum1Ptr,
                                                      &Result);
                                                           
        if (Status != ISTATUS_SUCCESS)
        {
            assert(Status == ISTATUS_ALLOCATION_FAILED);
            throw std::bad_alloc();
        }

        return SpectrumReference(Result);
    }

    _Ret_
    SpectrumReference
    Attenuate(
        _In_opt_ PCSPECTRUM SpectrumPtr,
        _In_ FLOAT Attenuation
        )
    {
        PCSPECTRUM Result;
        
        ISTATUS Status = SpectrumCompositorAttenuateSpectrum(Data,
                                                             SpectrumPtr,
                                                             Attenuation,
                                                             &Result);
                                                           
        if (Status == ISTATUS_SUCCESS)
        {
            return SpectrumReference(Result);
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

    _Ret_
    SpectrumReference
    Reflect(
        _In_opt_ PCSPECTRUM SpectrumPtr,
        _In_opt_ PCREFLECTOR ReflectorPtr
        )
    {
        PCSPECTRUM Result;
        
        ISTATUS Status = SpectrumCompositorAddReflection(Data,
                                                         SpectrumPtr,
                                                         ReflectorPtr,
                                                         &Result);
                                                           
        if (Status != ISTATUS_SUCCESS)
        {
            assert(Status == ISTATUS_ALLOCATION_FAILED);
            throw std::bad_alloc();
        }

        return SpectrumReference(Result);
    }

    _Ret_
    SpectrumReference
    AttenuatedReflect(
        _In_opt_ PCSPECTRUM SpectrumPtr,
        _In_opt_ PCREFLECTOR ReflectorPtr,
        _In_ FLOAT Attenuation
        )
    {
        PCSPECTRUM Result;
        
        ISTATUS Status = SpectrumCompositorAttenuatedAddReflection(Data,
                                                                   SpectrumPtr,
                                                                   ReflectorPtr,
                                                                   Attenuation,
                                                                   &Result);

        if (Status == ISTATUS_SUCCESS)
        {
            return SpectrumReference(Result);
        }

        switch (Status)
        {
            case ISTATUS_INVALID_ARGUMENT_03:
                throw std::invalid_argument("Attenuation");
            default:
                assert(Status == ISTATUS_ALLOCATION_FAILED);
                throw std::bad_alloc();
        }
    }

    SpectrumCompositor(
        void
        )
    {
        ISTATUS Status = SpectrumCompositorAllocate(&Data);
        
        if (Status != ISTATUS_SUCCESS)
        {
            assert(Status == ISTATUS_ALLOCATION_FAILED);
            throw std::bad_alloc();
        }
    }
};

} // namespace IrisSpectrum

#endif // _SPECTRUM_COMPOSITOR_IRIS_SPECTRUM_PLUS_PLUS_