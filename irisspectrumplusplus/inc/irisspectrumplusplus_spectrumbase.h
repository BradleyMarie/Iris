/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    irisspectrumplusplus_spectrumbase.h

Abstract:

    This file contains the definitions for the 
    Iris++ SpectrumBase type.

--*/

#include <irisadvancedplusplus.h>

#ifndef _SPECTRUM_BASE_IRIS_SPECTRUM_PLUS_PLUS_
#define _SPECTRUM_BASE_IRIS_SPECTRUM_PLUS_PLUS_

namespace IrisSpectrum {

//
// Types
//

class SpectrumBase {
protected:
    IRISSPECTRUMPLUSPLUSAPI
    static
    Spectrum
    Create(
        _In_ std::unique_ptr<SpectrumBase> SpectrumBasePtr
        );

public:
    _Ret_
    virtual
    FLOAT
    Sample(
        _In_ FLOAT Wavelength
        ) const;

    virtual
    ~SpectrumBase(
        void
        )
    { }
};

} // namespace IrisSpectrum

#endif // _SPECTRUM_BASE_IRIS_SPECTRUM_PLUS_PLUS_