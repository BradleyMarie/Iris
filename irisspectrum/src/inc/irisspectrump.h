/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    irisspectrum.h

Abstract:

    This module includes all of the Iris Spectrum internal headers.

--*/

#ifndef _IRIS_SPECTRUM_INTERNAL_HEADER_
#define _IRIS_SPECTRUM_INTERNAL_HEADER_

#ifndef _IRIS_SPECTRUM_EXPORT_REFLECTOR_REFERENCE_ROUTINES_
#define _DISABLE_IRIS_SPECTRUM_REFLECTOR_REFERENCE_EXPORTS_
#endif // _IRIS_SPECTRUM_EXPORT_REFLECTOR_REFERENCE_ROUTINES_

#ifndef _IRIS_SPECTRUM_EXPORT_SPECTRUM_REFERENCE_ROUTINES_
#define _DISABLE_IRIS_SPECTRUM_SPECTRUM_REFERENCE_EXPORTS_
#endif // _IRIS_SPECTRUM_EXPORT_SPECTRUM_REFERENCE_ROUTINES_

#include <irisspectrum.h>
#include <iriscommon_malloc.h>
#include <iriscommon_staticallocator.h>

#include "irisspectrum_spectrumreference.h"
#include "irisspectrum_reflectorreference.h"

#endif // _IRIS_SPECTRUM_INTERNAL_HEADER_