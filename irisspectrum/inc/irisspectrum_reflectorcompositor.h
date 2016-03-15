/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    irisspectrum_reflectorcompositor.h

Abstract:

    This file contains the definitions for the REFLECTOR_COMPOSITOR type.

--*/

#ifndef _REFLECTOR_COMPOSITOR_IRIS_SPECTRUM_
#define _REFLECTOR_COMPOSITOR_IRIS_SPECTRUM_

#include <irisspectrum.h>

//
// Types
//

typedef struct _REFLECTOR_COMPOSITOR REFLECTOR_COMPOSITOR, *PREFLECTOR_COMPOSITOR;
typedef CONST REFLECTOR_COMPOSITOR *PCREFLECTOR_COMPOSITOR;

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISSPECTRUMAPI
ISTATUS
ReflectorCompositorAllocate(
    _Out_ PREFLECTOR_COMPOSITOR *Compositor
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISSPECTRUMAPI
ISTATUS
ReflectorCompositorAddReflections(
    _Inout_ PREFLECTOR_COMPOSITOR Compositor,
    _In_opt_ PCREFLECTOR Reflector0,
    _In_opt_ PCREFLECTOR Reflector1,
    _Out_ PCREFLECTOR *Sum
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISSPECTRUMAPI
ISTATUS
ReflectorCompositorAttenuateReflection(
    _Inout_ PREFLECTOR_COMPOSITOR Compositor,
    _In_opt_ PCREFLECTOR Reflector,
    _In_ FLOAT Attenuation,
    _Out_ PCREFLECTOR *AttenuatedReflector
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISSPECTRUMAPI
ISTATUS
ReflectorCompositorAttenuatedAddReflections(
    _Inout_ PREFLECTOR_COMPOSITOR Compositor,
    _In_opt_ PCREFLECTOR Reflector0,
    _In_opt_ PCREFLECTOR Reflector1,
    _In_ FLOAT Attenuation,
    _Out_ PCREFLECTOR *AttenuatedReflector
    );

_Ret_
IRISSPECTRUMAPI
PREFLECTOR_COMPOSITOR_REFERENCE
ReflectorCompositorGetReflectorCompositorReference(
    _In_ PREFLECTOR_COMPOSITOR Compositor
    );

IRISSPECTRUMAPI
VOID
ReflectorCompositorClear(
    _Inout_ PREFLECTOR_COMPOSITOR Compositor
    );

IRISSPECTRUMAPI
VOID
ReflectorCompositorFree(
    _In_opt_ _Post_invalid_ PREFLECTOR_COMPOSITOR Compositor
    );

#endif // _REFLECTOR_COMPOSITOR_IRIS_SPECTRUM_