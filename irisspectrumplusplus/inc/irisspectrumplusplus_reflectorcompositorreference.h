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
    { }
    
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
    AddReflections(
        _In_ const Reflector & Reflector0Ref,
        _In_ const Reflector & Reflector1Ref
        )
    {
        ReflectorReference Result = AddReflections(Reflector0Ref.AsPCREFLECTOR(),
                                                   Reflector1Ref.AsPCREFLECTOR());
        
        return Result;
    }
    
    _Ret_
    ReflectorReference
    AddReflections(
        _In_ const Reflector & Reflector0Ref,
        _In_ ReflectorReference Reflector1Ref
        )
    {
        ReflectorReference Result = AddReflections(Reflector0Ref.AsPCREFLECTOR(),
                                                   Reflector1Ref.AsPCREFLECTOR());
        
        return Result;
    }
    
    _Ret_
    ReflectorReference
    AddReflections(
        _In_ ReflectorReference Reflector0Ref,
        _In_ const Reflector & Reflector1Ref
        )
    {
        ReflectorReference Result = AddReflections(Reflector0Ref.AsPCREFLECTOR(),
                                                   Reflector1Ref.AsPCREFLECTOR());
        
        return Result;
    }
    
    _Ret_
    ReflectorReference
    AddReflections(
        _In_ ReflectorReference Reflector0Ref,
        _In_ ReflectorReference Reflector1Ref
        )
    {
        ReflectorReference Result = AddReflections(Reflector0Ref.AsPCREFLECTOR(),
                                                   Reflector1Ref.AsPCREFLECTOR());
        
        return Result;
    }
    
    _Ret_
    ReflectorReference
    AttenuateReflection(
        _In_ const Reflector & ReflectorRef,
        _In_ FLOAT Attenuation
        )
    {
        ReflectorReference Result = AttenuateReflection(ReflectorRef.AsPCREFLECTOR(),
                                                        Attenuation);
        
        return Result;
    }
    
    _Ret_
    ReflectorReference
    AttenuateReflection(
        _In_ ReflectorReference ReflectorRef,
        _In_ FLOAT Attenuation
        )
    {
        ReflectorReference Result = AttenuateReflection(ReflectorRef.AsPCREFLECTOR(),
                                                        Attenuation);
        
        return Result;
    }
    
    _Ret_
    ReflectorReference
    AttenuatedAddReflections(
        _In_ const Reflector & Reflector0Ref,
        _In_ const Reflector & Reflector1Ref,
        _In_ FLOAT Attenuation
        )
    {
        ReflectorReference Result = AttenuatedAddReflections(Reflector0Ref.AsPCREFLECTOR(),
                                                             Reflector1Ref.AsPCREFLECTOR(),
                                                             Attenuation);
        
        return Result;
    }
    
    _Ret_
    ReflectorReference
    AttenuatedAddReflections(
        _In_ const Reflector & Reflector0Ref,
        _In_ ReflectorReference Reflector1Ref,
        _In_ FLOAT Attenuation
        )
    {
        ReflectorReference Result = AttenuatedAddReflections(Reflector0Ref.AsPCREFLECTOR(),
                                                             Reflector1Ref.AsPCREFLECTOR(),
                                                             Attenuation);
        
        return Result;
    }
    
    _Ret_
    ReflectorReference
    AttenuatedAddReflections(
        _In_ ReflectorReference Reflector0Ref,
        _In_ const Reflector & Reflector1Ref,
        _In_ FLOAT Attenuation
        )
    {
        ReflectorReference Result = AttenuatedAddReflections(Reflector0Ref.AsPCREFLECTOR(),
                                                             Reflector1Ref.AsPCREFLECTOR(),
                                                             Attenuation);
        
        return Result;
    }
    
    _Ret_
    ReflectorReference
    AttenuatedAddReflections(
        _In_ ReflectorReference Reflector0Ref,
        _In_ ReflectorReference Reflector1Ref,
        _In_ FLOAT Attenuation
        )
    {
        ReflectorReference Result = AttenuatedAddReflections(Reflector0Ref.AsPCREFLECTOR(),
                                                             Reflector1Ref.AsPCREFLECTOR(),
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
    AddReflections(
        _In_ PCREFLECTOR Reflector0Ptr,
        _In_ PCREFLECTOR Reflector1Ptr
        );

    _Ret_
    ReflectorReference
    AttenuateReflection(
        _In_ PCREFLECTOR ReflectorPtr,
        _In_ FLOAT Attenuation
        );

    _Ret_
    ReflectorReference
    AttenuatedAddReflections(
        _In_ PCREFLECTOR Reflector0Ptr,
        _In_ PCREFLECTOR Reflector1Ptr,
        _In_ FLOAT Attenuation
        );
};

} // namespace IrisSpectrum

#endif // _REFLECTOR_COMPOSITOR_REFERENCE_IRIS_SPECTRUM_PLUS_PLUS_