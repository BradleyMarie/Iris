/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    reflectorreference.c

Abstract:

    This file contains the definitions for the REFLECTOR_REFERENCE type.

--*/

#define _IRIS_SPECTRUM_EXPORT_REFLECTOR_REFERENCE_ROUTINES_
#include <irisspectrump.h>

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
ReflectorReferenceReflect(
    _In_opt_ PCREFLECTOR_REFERENCE Reflector,
    _In_ FLOAT Wavelength,
    _In_ FLOAT IncomingIntensity,
    _Out_ PFLOAT OutgoingIntensity
    )
{
    ISTATUS Status;

    Status = StaticReflectorReferenceReflect(Reflector,
                                             Wavelength,
                                             IncomingIntensity,
                                             OutgoingIntensity);

    return Status;
}