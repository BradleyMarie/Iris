/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    irisshadingmodel_reflector.h

Abstract:

    This file contains the definitions for the REFLECTOR type.

--*/

#ifndef _REFLECTOR_IRIS_SHADING_MODEL_
#define _REFLECTOR_IRIS_SHADING_MODEL_

#include <irisshadingmodel.h>

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
_Ret_maybenull_
IRISSHADINGMODELAPI
PREFLECTOR
ReflectorAllocate(
    _In_ PCREFLECTOR_VTABLE ReflectorVTable,
    _In_reads_bytes_(DataSizeInBytes) PCVOID Data,
    _In_ SIZE_T DataSizeInBytes,
    _In_ SIZE_T DataAlignment
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISSHADINGMODELAPI
ISTATUS
ReflectorReflect(
    _In_ PCREFLECTOR Reflector,
    _In_ FLOAT Wavelength,
    _In_ FLOAT IncomingIntensity,
    _Out_ PFLOAT OutgoingIntensity
    );

IRISSHADINGMODELAPI
VOID
ReflectorReference(
    _In_opt_ PREFLECTOR Reflector
    );

IRISSHADINGMODELAPI
VOID
ReflectorDereference(
    _In_opt_ _Post_invalid_ PREFLECTOR Reflector
    );

#endif // _REFLECTOR_IRIS_SHADING_MODEL_