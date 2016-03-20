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
    AddSpectra(
        _In_ const Spectrum & Spectrum0Ref,
        _In_ const Spectrum & Spectrum1Ref
        )
    {
        SpectrumReference Result = AddSpectra(Spectrum0Ref.AsPCSPECTRUM(),
                                              Spectrum1Ref.AsPCSPECTRUM());
        
        return Result;
    }
    
    _Ret_
    SpectrumReference
    AddSpectra(
        _In_ const Spectrum & Spectrum0Ref,
        _In_ SpectrumReference Spectrum1Ref
        )
    {
        SpectrumReference Result = AddSpectra(Spectrum0Ref.AsPCSPECTRUM(),
                                              Spectrum1Ref.AsPCSPECTRUM());
        
        return Result;
    }
    
    _Ret_
    SpectrumReference
    AddSpectra(
        _In_ SpectrumReference Spectrum0Ref,
        _In_ const Spectrum & Spectrum1Ref
        )
    {
        SpectrumReference Result = AddSpectra(Spectrum0Ref.AsPCSPECTRUM(),
                                              Spectrum1Ref.AsPCSPECTRUM());
        
        return Result;
    }
    
    _Ret_
    SpectrumReference
    AddSpectra(
        _In_ SpectrumReference Spectrum0Ref,
        _In_ SpectrumReference Spectrum1Ref
        )
    {
        SpectrumReference Result = AddSpectra(Spectrum0Ref.AsPCSPECTRUM(),
                                              Spectrum1Ref.AsPCSPECTRUM());
        
        return Result;
    }
    
    _Ret_
    SpectrumReference
    AttenuateSpectrum(
        _In_ const Spectrum & SpectrumRef,
        _In_ FLOAT Attenuation
        )
    {
        SpectrumReference Result = AttenuateSpectrum(SpectrumRef.AsPCSPECTRUM(),
                                                     Attenuation);
        
        return Result;
    }
    
    _Ret_
    SpectrumReference
    AttenuateSpectrum(
        _In_ SpectrumReference SpectrumRef,
        _In_ FLOAT Attenuation
        )
    {
        SpectrumReference Result = AttenuateSpectrum(SpectrumRef.AsPCSPECTRUM(),
                                                     Attenuation);
        
        return Result;
    }
    
    _Ret_
    SpectrumReference
    AttenuatedAddSpectra(
        _In_ const Spectrum & Spectrum0Ref,
        _In_ const Spectrum & Spectrum1Ref,
        _In_ FLOAT Attenuation
        )
    {
        SpectrumReference Result = AttenuatedAddSpectra(Spectrum0Ref.AsPCSPECTRUM(),
                                                        Spectrum1Ref.AsPCSPECTRUM(),
                                                        Attenuation);
        
        return Result;
    }
    
    _Ret_
    SpectrumReference
    AttenuatedAddSpectra(
        _In_ const Spectrum & Spectrum0Ref,
        _In_ SpectrumReference Spectrum1Ref,
        _In_ FLOAT Attenuation
        )
    {
        SpectrumReference Result = AttenuatedAddSpectra(Spectrum0Ref.AsPCSPECTRUM(),
                                                        Spectrum1Ref.AsPCSPECTRUM(),
                                                        Attenuation);
        
        return Result;
    }
    
    _Ret_
    SpectrumReference
    AttenuatedAddSpectra(
        _In_ SpectrumReference Spectrum0Ref,
        _In_ const Spectrum & Spectrum1Ref,
        _In_ FLOAT Attenuation
        )
    {
        SpectrumReference Result = AttenuatedAddSpectra(Spectrum0Ref.AsPCSPECTRUM(),
                                                        Spectrum1Ref.AsPCSPECTRUM(),
                                                        Attenuation);
        
        return Result;
    }
    
    _Ret_
    SpectrumReference
    AttenuatedAddSpectra(
        _In_ SpectrumReference Spectrum0Ref,
        _In_ SpectrumReference Spectrum1Ref,
        _In_ FLOAT Attenuation
        )
    {
        SpectrumReference Result = AttenuatedAddSpectra(Spectrum0Ref.AsPCSPECTRUM(),
                                                        Spectrum1Ref.AsPCSPECTRUM(),
                                                        Attenuation);
        
        return Result;
    }
    
    _Ret_
    SpectrumReference
    AddReflection(
        _In_ const Spectrum & SpectrumRef,
        _In_ const Reflector & ReflectorRef
        )
    {
        SpectrumReference Result = AddReflection(SpectrumRef.AsPCSPECTRUM(),
                                                 ReflectorRef.AsPCREFLECTOR());
        
        return Result;
    }

    _Ret_
    SpectrumReference
    AddReflection(
        _In_ SpectrumReference SpectrumRef,
        _In_ const Reflector & ReflectorRef
        )
    {
        SpectrumReference Result = AddReflection(SpectrumRef.AsPCSPECTRUM(),
                                                 ReflectorRef.AsPCREFLECTOR());
        
        return Result;
    }

    _Ret_
    SpectrumReference
    AddReflection(
        _In_ const Spectrum & SpectrumRef,
        _In_ ReflectorReference ReflectorRef
        )
    {
        SpectrumReference Result = AddReflection(SpectrumRef.AsPCSPECTRUM(),
                                                 ReflectorRef.AsPCREFLECTOR());
        
        return Result;
    }

    _Ret_
    SpectrumReference
    AddReflection(
        _In_ SpectrumReference SpectrumRef,
        _In_ ReflectorReference ReflectorRef
        )
    {
        SpectrumReference Result = AddReflection(SpectrumRef.AsPCSPECTRUM(),
                                                 ReflectorRef.AsPCREFLECTOR());
        
        return Result;
    }

    _Ret_
    SpectrumReference
    AttenuatedAddReflection(
        _In_ const Spectrum & SpectrumRef,
        _In_ const Reflector & ReflectorRef,
        _In_ FLOAT Attenuation
        )
    {
        SpectrumReference Result = AttenuatedAddReflection(SpectrumRef.AsPCSPECTRUM(),
                                                           ReflectorRef.AsPCREFLECTOR(),
                                                           Attenuation);
        
        return Result;
    }
    
    _Ret_
    SpectrumReference
    AttenuatedAddReflection(
        _In_ SpectrumReference SpectrumRef,
        _In_ const Reflector & ReflectorRef,
        _In_ FLOAT Attenuation
        )
    {
        SpectrumReference Result = AttenuatedAddReflection(SpectrumRef.AsPCSPECTRUM(),
                                                           ReflectorRef.AsPCREFLECTOR(),
                                                           Attenuation);
        
        return Result;
    }
    
    _Ret_
    SpectrumReference
    AttenuatedAddReflection(
        _In_ const Spectrum & SpectrumRef,
        _In_ ReflectorReference ReflectorRef,
        _In_ FLOAT Attenuation
        )
    {
        SpectrumReference Result = AttenuatedAddReflection(SpectrumRef.AsPCSPECTRUM(),
                                                           ReflectorRef.AsPCREFLECTOR(),
                                                           Attenuation);
        
        return Result;
    }
    
    _Ret_
    SpectrumReference
    AttenuatedAddReflection(
        _In_ SpectrumReference SpectrumRef,
        _In_ ReflectorReference ReflectorRef,
        _In_ FLOAT Attenuation
        )
    {
        SpectrumReference Result = AttenuatedAddReflection(SpectrumRef.AsPCSPECTRUM(),
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
        _In_ SpectrumCompositor & ToCopy
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
    AddSpectra(
        _In_ PCSPECTRUM Spectrum0Ptr,
        _In_ PCSPECTRUM Spectrum1Ptr
        );

    _Ret_
    SpectrumReference
    AttenuateSpectrum(
        _In_ PCSPECTRUM SpectrumPtr,
        _In_ FLOAT Attenuation
        );

    _Ret_
    SpectrumReference
    AttenuatedAddSpectra(
        _In_ PCSPECTRUM Spectrum0Ptr,
        _In_ PCSPECTRUM Spectrum1Ptr,
        _In_ FLOAT Attenuation
        );

    _Ret_
    SpectrumReference
    AddReflection(
        _In_ PCSPECTRUM SpectrumPtr,
        _In_ PCREFLECTOR ReflectorPtr
        );

    _Ret_
    SpectrumReference
    AttenuatedAddReflection(
        _In_ PCSPECTRUM SpectrumPtr,
        _In_ PCREFLECTOR ReflectorPtr,
        _In_ FLOAT Attenuation
        );

    IRISSPECTRUMPLUSPLUSAPI
    SpectrumCompositor(
        void
        );
};

} // namespace IrisSpectrum

#endif // _SPECTRUM_COMPOSITOR_IRIS_SPECTRUM_PLUS_PLUS_