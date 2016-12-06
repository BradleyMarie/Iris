/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    irisspectrumplusplus.h

Abstract:

    This module includes all of the IrisSpectrum++ public headers.

--*/

#ifndef _IRIS_SPECTRUM_PLUS_PLUS_HEADER_
#define _IRIS_SPECTRUM_PLUS_PLUS_HEADER_

//
// Include Iris Spectrum
//

extern "C" {
#include <irisspectrum.h>
}

//
// Include C++ standard library headers
//

#include <memory>

//
// Include Iris++ Headers
//

#include <irisplusplus.h>

//
// Include IrisSpectrum++ Headers
//

#include "irisspectrumplusplus_api.h"
#include "irisspectrumplusplus_reflectorreference.h"
#include "irisspectrumplusplus_reflector.h"
#include "irisspectrumplusplus_reflectorbase.h"
#include "irisspectrumplusplus_spectrumreference.h"
#include "irisspectrumplusplus_spectrum.h"
#include "irisspectrumplusplus_spectrumbase.h"
#include "irisspectrumplusplus_reflectorcompositorreference.h"
#include "irisspectrumplusplus_reflectorcompositor.h"
#include "irisspectrumplusplus_spectrumcompositorreference.h"
#include "irisspectrumplusplus_spectrumcompositor.h"

#endif // _IRIS_SPECTRUM_PLUS_PLUS_HEADER_