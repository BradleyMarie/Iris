/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    irisspectrum_reflector.h

Abstract:

    This file contains the definitions for the REFLECTOR type.

--*/

#ifndef _REFLECTOR_IRIS_SPECTRUM_
#define _REFLECTOR_IRIS_SPECTRUM_

#include <irisspectrum.h>

//
// Types
//

typedef
_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
(*PREFLECTOR_REFLECT_ROUTINE)(
    _In_ PCVOID Context,
    _In_ FLOAT Wavelength,
    _In_ FLOAT IncomingIntensity,
    _Out_ PFLOAT OutgoingIntensity
    );

typedef struct _PREFLECTOR_VTABLE {
    PREFLECTOR_REFLECT_ROUTINE ReflectRoutine;
    PFREE_ROUTINE FreeRoutine;
} REFLECTOR_VTABLE, *PREFLECTOR_VTABLE;

typedef CONST REFLECTOR_VTABLE *PCREFLECTOR_VTABLE;

typedef struct _REFLECTOR REFLECTOR, *PREFLECTOR;
typedef CONST REFLECTOR *PCREFLECTOR;

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISSPECTRUMAPI
ISTATUS
ReflectorAllocate(
    _In_ PCREFLECTOR_VTABLE ReflectorVTable,
    _When_(DataSizeInBytes != 0, _In_reads_bytes_opt_(DataSizeInBytes)) PCVOID Data,
    _In_ SIZE_T DataSizeInBytes,
    _When_(DataSizeInBytes != 0, _Pre_satisfies_(_Curr_ != 0 && (_Curr_ & (_Curr_ - 1)) == 0 && DataSizeInBytes % _Curr_ == 0)) SIZE_T DataAlignment,
    _Out_ PREFLECTOR *Reflector
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISSPECTRUMAPI
ISTATUS
ReflectorReflect(
    _In_opt_ PCREFLECTOR Reflector,
    _In_ FLOAT Wavelength,
    _In_ FLOAT IncomingIntensity,
    _Out_ PFLOAT OutgoingIntensity
    );

_Ret_
IRISSPECTRUMAPI
PCREFLECTOR_REFERENCE
ReflectorGetReflectorReference(
    _In_ PCREFLECTOR Reflector
    );

IRISSPECTRUMAPI
VOID
ReflectorRetain(
    _In_opt_ PREFLECTOR Reflector
    );

IRISSPECTRUMAPI
VOID
ReflectorRelease(
    _In_opt_ _Post_invalid_ PREFLECTOR Reflector
    );

#endif // _REFLECTOR_IRIS_SPECTRUM_