/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    irisspectrumplusplus_reflectorcompositor.h

Abstract:

    This file contains the definitions for the 
    IrisSpectrum++ ReflectorCompositor type.

--*/

#include <irisspectrumplusplus.h>

#ifndef _REFLECTOR_COMPOSITOR_IRIS_SPECTRUM_PLUS_PLUS_
#define _REFLECTOR_COMPOSITOR_IRIS_SPECTRUM_PLUS_PLUS_

namespace IrisSpectrum {

//
// Types
//

class ReflectorCompositor final {
public:
    ReflectorCompositor(
        _In_ ReflectorCompositor && ToMove
        )
        : Data(ToMove.Data)
    {
        ToMove.Data = nullptr;
    }

    static
    ReflectorCompositor
    Create(
        void
        )
    {
        return ReflectorCompositor();
    }
    
    _Ret_
    PREFLECTOR_COMPOSITOR
    AsPREFLECTOR_COMPOSITOR(
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
        _In_ ReflectorReference Reflector1Ref
        )
    {
        ReflectorReference Result = Add(Reflector0Ref.AsPCREFLECTOR(),
                                        Reflector1Ref.AsPCREFLECTOR());
        
        return Result;
    }
    
    _Ret_
    ReflectorReference
    Add(
        _In_ ReflectorReference Reflector0Ref,
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
        _In_ ReflectorReference Reflector0Ref,
        _In_ ReflectorReference Reflector1Ref
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
        _In_ ReflectorReference ReflectorRef,
        _In_ FLOAT Attenuation
        )
    {
        ReflectorReference Result = Attenuate(ReflectorRef.AsPCREFLECTOR(),
                                              Attenuation);
        
        return Result;
    }
    
    _Ret_
    ReflectorReference
    AttenuatedAdd(
        _In_ const Reflector & Reflector0Ref,
        _In_ const Reflector & Reflector1Ref,
        _In_ FLOAT Attenuation
        )
    {
        ReflectorReference Result = AttenuatedAdd(Reflector0Ref.AsPCREFLECTOR(),
                                                  Reflector1Ref.AsPCREFLECTOR(),
                                                  Attenuation);
        
        return Result;
    }
    
    _Ret_
    ReflectorReference
    AttenuatedAdd(
        _In_ const Reflector & Reflector0Ref,
        _In_ ReflectorReference Reflector1Ref,
        _In_ FLOAT Attenuation
        )
    {
        ReflectorReference Result = AttenuatedAdd(Reflector0Ref.AsPCREFLECTOR(),
                                                  Reflector1Ref.AsPCREFLECTOR(),
                                                  Attenuation);
        
        return Result;
    }
    
    _Ret_
    ReflectorReference
    AttenuatedAdd(
        _In_ ReflectorReference Reflector0Ref,
        _In_ const Reflector & Reflector1Ref,
        _In_ FLOAT Attenuation
        )
    {
        ReflectorReference Result = AttenuatedAdd(Reflector0Ref.AsPCREFLECTOR(),
                                                  Reflector1Ref.AsPCREFLECTOR(),
                                                  Attenuation);
        
        return Result;
    }
    
    _Ret_
    ReflectorReference
    AttenuatedAdd(
        _In_ ReflectorReference Reflector0Ref,
        _In_ ReflectorReference Reflector1Ref,
        _In_ FLOAT Attenuation
        )
    {
        ReflectorReference Result = AttenuatedAdd(Reflector0Ref.AsPCREFLECTOR(),
                                                  Reflector1Ref.AsPCREFLECTOR(),
                                                  Attenuation);
        
        return Result;
    }
    
    void
    Clear(
        void
        )
    {
        ReflectorCompositorClear(Data);
    }
    
    ReflectorCompositor(
        _In_ ReflectorCompositor & ToCopy
        ) = delete;
        
    ReflectorCompositor &
    operator=(
        _In_ const ReflectorCompositor & ToCopy
        ) = delete;
    
    ~ReflectorCompositor(
        void
        )
     {
         ReflectorCompositorFree(Data);
     }

private:
    PREFLECTOR_COMPOSITOR Data;

    _Ret_
    ReflectorReference
    Add(
        _In_ PCREFLECTOR Reflector0Ptr,
        _In_ PCREFLECTOR Reflector1Ptr
        );

    _Ret_
    ReflectorReference
    Attenuate(
        _In_ PCREFLECTOR ReflectorPtr,
        _In_ FLOAT Attenuation
        );

    _Ret_
    ReflectorReference
    AttenuatedAdd(
        _In_ PCREFLECTOR Reflector0Ptr,
        _In_ PCREFLECTOR Reflector1Ptr,
        _In_ FLOAT Attenuation
        );

    IRISSPECTRUMPLUSPLUSAPI
    ReflectorCompositor(
        void
        );
};

} // namespace IrisSpectrum

#endif // _REFLECTOR_COMPOSITOR_IRIS_SPECTRUM_PLUS_PLUS_