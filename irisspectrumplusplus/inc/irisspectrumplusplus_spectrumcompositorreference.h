/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    irisspectrumplusplus_spectrumcompositorreference.h

Abstract:

    This file contains the definitions for the 
    IrisSpectrum++ SpectrumCompositorReference type.

--*/

#include <irisspectrumplusplus.h>

#ifndef _SPECTRUM_COMPOSITOR_REFERENCE_IRIS_SPECTRUM_PLUS_PLUS_
#define _SPECTRUM_COMPOSITOR_REFERENCE_IRIS_SPECTRUM_PLUS_PLUS_

namespace IrisSpectrum {

//
// Types
//

class SpectrumCompositorReference final {
public:
    SpectrumCompositorReference(
        _In_ PSPECTRUM_COMPOSITOR_REFERENCE SpectrumCompositorReferencePtr
        )
    {
        if (SpectrumCompositorReferencePtr == nullptr)
        {
            throw std::invalid_argument("SpectrumCompositorReferencePtr");
        }
    }
    
    _Ret_
    PSPECTRUM_COMPOSITOR_REFERENCE
    AsPSPECTRUM_COMPOSITOR_REFERENCE(
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
    AttenuatedAdd(
        _In_ const Spectrum & Spectrum0Ref,
        _In_ const Spectrum & Spectrum1Ref,
        _In_ FLOAT Attenuation
        )
    {
        SpectrumReference Result = AttenuatedAdd(Spectrum0Ref.AsPCSPECTRUM(),
                                                 Spectrum1Ref.AsPCSPECTRUM(),
                                                 Attenuation);
        
        return Result;
    }
    
    _Ret_
    SpectrumReference
    AttenuatedAdd(
        _In_ const Spectrum & Spectrum0Ref,
        _In_ SpectrumReference Spectrum1Ref,
        _In_ FLOAT Attenuation
        )
    {
        SpectrumReference Result = AttenuatedAdd(Spectrum0Ref.AsPCSPECTRUM(),
                                                 Spectrum1Ref.AsPCSPECTRUM(),
                                                 Attenuation);
        
        return Result;
    }
    
    _Ret_
    SpectrumReference
    AttenuatedAdd(
        _In_ SpectrumReference Spectrum0Ref,
        _In_ const Spectrum & Spectrum1Ref,
        _In_ FLOAT Attenuation
        )
    {
        SpectrumReference Result = AttenuatedAdd(Spectrum0Ref.AsPCSPECTRUM(),
                                                 Spectrum1Ref.AsPCSPECTRUM(),
                                                 Attenuation);
        
        return Result;
    }
    
    _Ret_
    SpectrumReference
    AttenuatedAdd(
        _In_ SpectrumReference Spectrum0Ref,
        _In_ SpectrumReference Spectrum1Ref,
        _In_ FLOAT Attenuation
        )
    {
        SpectrumReference Result = AttenuatedAdd(Spectrum0Ref.AsPCSPECTRUM(),
                                                 Spectrum1Ref.AsPCSPECTRUM(),
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
    
    SpectrumCompositorReference(
        _In_ SpectrumCompositorReference & ToCopy
        )
    : Data(ToCopy.Data)
    { }
        
    SpectrumCompositorReference &
    operator=(
        _In_ const SpectrumCompositorReference & ToCopy
        )
    {
        Data = ToCopy.Data;
    }
        
private:
    PSPECTRUM_COMPOSITOR_REFERENCE Data;

    _Ret_
    SpectrumReference
    Add(
        _In_opt_ PCSPECTRUM Spectrum0Ptr,
        _In_opt_ PCSPECTRUM Spectrum1Ptr
        )
    {
        PCSPECTRUM Result;
        
        ISTATUS Status = SpectrumCompositorReferenceAddSpectra(Data,
                                                               Spectrum0Ptr,
                                                               Spectrum1Ptr,
                                                               &Result);
        
        if (Status == ISTATUS_SUCCESS)
        {
            return SpectrumReference(Result);
        }

        switch (Status)
        {
            case ISTATUS_ALLOCATION_FAILED:
                throw std::bad_alloc();
            default:
                throw std::runtime_error(Iris::ISTATUSToCString(Status));
        }
    }

    _Ret_
    SpectrumReference
    Attenuate(
        _In_opt_ PCSPECTRUM SpectrumPtr,
        _In_ FLOAT Attenuation
        )
    {
        PCSPECTRUM Result;
        
        ISTATUS Status = SpectrumCompositorReferenceAttenuateSpectrum(Data,
                                                                      SpectrumPtr,
                                                                      Attenuation,
                                                                      &Result);
                                                           
        if (Status == ISTATUS_SUCCESS)
        {
            return SpectrumReference(Result);
        }

        switch (Status)
        {
            case ISTATUS_ALLOCATION_FAILED:
                throw std::bad_alloc();
            default:
                throw std::runtime_error(Iris::ISTATUSToCString(Status));
        }
    }

    _Ret_
    SpectrumReference
    AttenuatedAdd(
        _In_opt_ PCSPECTRUM Spectrum0Ptr,
        _In_opt_ PCSPECTRUM Spectrum1Ptr,
        _In_ FLOAT Attenuation
        )
    {
        PCSPECTRUM Result;
        
        ISTATUS Status = SpectrumCompositorReferenceAttenuatedAddSpectra(Data,
                                                                         Spectrum0Ptr,
                                                                         Spectrum1Ptr,
                                                                         Attenuation,
                                                                         &Result);
                                                           
        if (Status == ISTATUS_SUCCESS)
        {
            return SpectrumReference(Result);
        }

        switch (Status)
        {
            case ISTATUS_ALLOCATION_FAILED:
                throw std::bad_alloc();
            default:
                throw std::runtime_error(Iris::ISTATUSToCString(Status));
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
        
        ISTATUS Status = SpectrumCompositorReferenceAddReflection(Data,
                                                                  SpectrumPtr,
                                                                  ReflectorPtr,
                                                                  &Result);
                                                           
        if (Status == ISTATUS_SUCCESS)
        {
            return SpectrumReference(Result);
        }

        switch (Status)
        {
            case ISTATUS_ALLOCATION_FAILED:
                throw std::bad_alloc();
            default:
                throw std::runtime_error(Iris::ISTATUSToCString(Status));
        }
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
        
        ISTATUS Status = SpectrumCompositorReferenceAttenuatedAddReflection(Data,
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
            case ISTATUS_ALLOCATION_FAILED:
                throw std::bad_alloc();
            default:
                throw std::runtime_error(Iris::ISTATUSToCString(Status));
        }
    }
};

} // namespace IrisSpectrum

#endif // _SPECTRUM_COMPOSITOR_REFERENCE_IRIS_SPECTRUM_PLUS_PLUS_