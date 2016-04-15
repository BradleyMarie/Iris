/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    irisspectrum.h

Abstract:

    This module includes all of the Iris Spectrum internal headers.

--*/

#ifndef _IRIS_SPECTRUM_INTERNAL_HEADER_
#define _IRIS_SPECTRUM_INTERNAL_HEADER_

#ifndef _IRIS_SPECTRUM_EXPORT_REFLECTOR_ROUTINES_
#define _DISABLE_IRIS_SPECTRUM_REFLECTOR_EXPORTS_
#endif // _IRIS_SPECTRUM_EXPORT_REFLECTOR_ROUTINES_

#ifndef _IRIS_SPECTRUM_EXPORT_SPECTRUM_ROUTINES_
#define _DISABLE_IRIS_SPECTRUM_SPECTRUM_EXPORTS_
#endif // _IRIS_SPECTRUM_EXPORT_SPECTRUM_ROUTINES_

#include <irisspectrum.h>
#include <iriscommon_malloc.h>
#include <iriscommon_staticallocator.h>

#include "irisspectrum_spectrum.h"
#include "irisspectrum_reflector.h"
#include "irisspectrum_reflectorcompositor.h"

#endif // _IRIS_SPECTRUM_INTERNAL_HEADER_