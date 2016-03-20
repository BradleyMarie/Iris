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
        : Data(SpectrumCompositorReferencePtr)
    { }
    
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
        _In_ PCSPECTRUM Spectrum0Ptr,
        _In_ PCSPECTRUM Spectrum1Ptr
        );

    _Ret_
    SpectrumReference
    Attenuate(
        _In_ PCSPECTRUM SpectrumPtr,
        _In_ FLOAT Attenuation
        );

    _Ret_
    SpectrumReference
    AttenuatedAdd(
        _In_ PCSPECTRUM Spectrum0Ptr,
        _In_ PCSPECTRUM Spectrum1Ptr,
        _In_ FLOAT Attenuation
        );

    _Ret_
    SpectrumReference
    Reflect(
        _In_ PCSPECTRUM SpectrumPtr,
        _In_ PCREFLECTOR ReflectorPtr
        );

    _Ret_
    SpectrumReference
    AttenuatedReflect(
        _In_ PCSPECTRUM SpectrumPtr,
        _In_ PCREFLECTOR ReflectorPtr,
        _In_ FLOAT Attenuation
        );
};

} // namespace IrisSpectrum

#endif // _SPECTRUM_COMPOSITOR_REFERENCE_IRIS_SPECTRUM_PLUS_PLUS_