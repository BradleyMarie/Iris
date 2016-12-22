/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    irisspectrumplusplus_reflectorbase.h

Abstract:

    This file contains the definitions for the 
    Iris++ ReflectorBase type.

--*/

#include <irisspectrumplusplus.h>

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
    ReflectorBase(
        void
        ) = default;
    
    _Ret_
    virtual
    FLOAT
    Reflect(
        _In_ FLOAT Wavelength,
        _In_ FLOAT IncomingIntensity
        ) const = 0;

    ReflectorBase(
        _In_ const ReflectorBase & ToCopy
        ) = delete;
        
    ReflectorBase &
    operator=(
        _In_ const ReflectorBase & ToCopy
        ) = delete;

    virtual
    ~ReflectorBase(
        void
        ) = default;
};

} // namespace IrisSpectrum

#endif // _REFLECTOR_BASE_IRIS_SPECTRUM_PLUS_PLUS_