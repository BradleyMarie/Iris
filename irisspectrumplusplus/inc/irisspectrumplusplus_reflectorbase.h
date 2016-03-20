/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    irisspectrumplusplus_reflectorbase.h

Abstract:

    This file contains the definitions for the 
    Iris++ ReflectorBase type.

--*/

#include <irisadvancedplusplus.h>

#ifndef _REFLECTOR_BASE_IRIS_SPECTRUM_PLUS_PLUS_
#define _REFLECTOR_BASE_IRIS_SPECTRUM_PLUS_PLUS_

namespace IrisSpectrum {

//
// Types
//

class ReflectorBase {
protected:
    IRISSPECTRUMPLUSPLUSAPI
    static
    Reflector
    Create(
        _In_ std::unique_ptr<ReflectorBase> ReflectorBasePtr
        );

public:
    _Ret_
    virtual
    FLOAT
    Reflect(
        _In_ FLOAT Wavelength,
        _In_ FLOAT IncomingIntensity
        ) const;

    virtual
    ~ReflectorBase(
        void
        )
    { }
    
private:
    _Success_(return == ISTATUS_SUCCESS)
    static
    ISTATUS
    Reflect(
        _In_ PCVOID Context,
        _In_ FLOAT Wavelength,
        _In_ FLOAT IncomingIntensity,
        _Out_ PFLOAT OutgoingIntensity
        );
    
    static
    VOID 
    Free(
        _In_ _Post_invalid_ PVOID Context
        );
    
    const static REFLECTOR_VTABLE InteropVTable;
};

} // namespace IrisSpectrum

#endif // _REFLECTOR_BASE_IRIS_SPECTRUM_PLUS_PLUS_