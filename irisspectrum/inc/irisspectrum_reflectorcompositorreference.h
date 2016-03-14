/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    irisspectrum_reflectorcompositorreference.h

Abstract:

    This file contains the definitions for the REFLECTOR_COMPOSITOR_REFERENCE type.

--*/

#ifndef _REFLECTOR_COMPOSITOR_REFERENCE_IRIS_SPECTRUM_
#define _REFLECTOR_COMPOSITOR_REFERENCE_IRIS_SPECTRUM_

#include <irisspectrum.h>

//
// Types
//

typedef struct _REFLECTOR_COMPOSITOR_REFERENCE REFLECTOR_COMPOSITOR_REFERENCE, *PREFLECTOR_COMPOSITOR_REFERENCE;
typedef CONST REFLECTOR_COMPOSITOR_REFERENCE *PCREFLECTOR_COMPOSITOR_REFERENCE;

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISSPECTRUMAPI
ISTATUS
ReflectorCompositorReferenceAddReflections(
    _Inout_ PREFLECTOR_COMPOSITOR_REFERENCE Compositor,
    _In_opt_ PCREFLECTOR_REFERENCE Reflector0,
    _In_opt_ PCREFLECTOR_REFERENCE Reflector1,
    _Out_ PCREFLECTOR_REFERENCE *Sum
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISSPECTRUMAPI
ISTATUS
ReflectorCompositorReferenceAttenuateReflection(
    _Inout_ PREFLECTOR_COMPOSITOR_REFERENCE Compositor,
    _In_opt_ PCREFLECTOR_REFERENCE Reflector,
    _In_ FLOAT Attenuation,
    _Out_ PCREFLECTOR_REFERENCE *AttenuatedReflector
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISSPECTRUMAPI
ISTATUS
ReflectorCompositorReferenceAttenuatedAddReflections(
    _Inout_ PREFLECTOR_COMPOSITOR_REFERENCE Compositor,
    _In_opt_ PCREFLECTOR_REFERENCE Reflector0,
    _In_opt_ PCREFLECTOR_REFERENCE Reflector1,
    _In_ FLOAT Attenuation,
    _Out_ PCREFLECTOR_REFERENCE *AttenuatedReflector
    );

#endif // _REFLECTOR_COMPOSITOR_REFERENCE_IRIS_SPECTRUM_