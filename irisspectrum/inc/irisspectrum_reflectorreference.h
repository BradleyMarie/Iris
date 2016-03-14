/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    irisspectrum_reflectorreference.h

Abstract:

    This file contains the definitions for the REFLECTOR_REFERENCE type.

--*/

#ifndef _REFLECTOR_REFERENCE_IRIS_SPECTRUM_
#define _REFLECTOR_REFERENCE_IRIS_SPECTRUM_

#include <irisspectrum.h>

//
// Types
//

typedef struct _REFLECTOR_REFERENCE REFLECTOR_REFERENCE, *PREFLECTOR_REFERENCE;
typedef CONST REFLECTOR_REFERENCE *PCREFLECTOR_REFERENCE;

//
// Functions
//

#ifndef _DISABLE_IRIS_SPECTRUM_REFLECTOR_REFERENCE_EXPORTS_

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISSPECTRUMAPI
ISTATUS
ReflectorReferenceReflect(
    _In_opt_ PCREFLECTOR_REFERENCE Reflector,
    _In_ FLOAT Wavelength,
    _In_ FLOAT IncomingIntensity,
    _Out_ PFLOAT OutgoingIntensity
    );

#endif // _DISABLE_IRIS_SPECTRUM_REFLECTOR_REFERENCE_EXPORTS_

#endif // _REFLECTOR_REFERENCE_IRIS_SPECTRUM_