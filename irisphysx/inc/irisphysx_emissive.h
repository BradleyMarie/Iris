/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    irisphysx_emissive.h

Abstract:

    This file contains the definitions for the EMISSIVE_LIGHT type.

--*/

#ifndef _EMISSIVE_LIGHT_IRIS_PHYSX_
#define _EMISSIVE_LIGHT_IRIS_PHYSX_

#include <irisphysx.h>

//
// Types
//

typedef
_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
(*PEMISSIVE_LIGHT_SAMPLE_ROUTINE)(
    _In_ PCVOID Context,
    _In_ POINT3 WorldHitPoint,
    _In_ VECTOR3 WorldViewer,
    _In_ PCVECTOR3 WorldNormalizedNormal,
    _In_opt_ PCVOID AdditionalData,
    _Inout_ PRANDOM Rng,
    _Out_ PCSPECTRUM *Output
    );

typedef struct _EMISSIVE_LIGHT_VTABLE {
    PEMISSIVE_LIGHT_SAMPLE_ROUTINE SampleRoutine;
    PFREE_ROUTINE FreeRoutine;
} EMISSIVE_LIGHT_VTABLE, *PEMISSIVE_LIGHT_VTABLE;

typedef CONST EMISSIVE_LIGHT_VTABLE *PCEMISSIVE_LIGHT_VTABLE;

typedef struct _EMISSIVE_LIGHT EMISSIVE_LIGHT, *PEMISSIVE_LIGHT;
typedef CONST EMISSIVE_LIGHT *PCEMISSIVE_LIGHT;

//
// Functions
//

_Check_return_
_Ret_maybenull_
IRISPHYSXAPI
PEMISSIVE_LIGHT
EmissiveLightAllocate(
    _In_ PCEMISSIVE_LIGHT_VTABLE EmissiveLightVTable,
    _In_reads_bytes_(DataSizeInBytes) PCVOID Data,
    _In_ SIZE_T DataSizeInBytes,
    _In_ SIZE_T DataAlignment
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISPHYSXAPI
ISTATUS
EmissiveLightSample(
    _In_ PCEMISSIVE_LIGHT EmissiveLight,
    _In_ POINT3 WorldHitPoint,
    _In_ VECTOR3 WorldViewer,
    _In_ PCVECTOR3 WorldNormalizedNormal,
    _In_opt_ PCVOID AdditionalData,
    _Inout_ PRANDOM Rng,
    _Out_ PCSPECTRUM *Output
    );

IRISPHYSXAPI
VOID
EmissiveLightReference(
    _In_opt_ PEMISSIVE_LIGHT EmissiveLight
    );

IRISPHYSXAPI
VOID
EmissiveLightDereference(
    _In_opt_ _Post_invalid_ PEMISSIVE_LIGHT EmissiveLight
    );

#endif // _EMISSIVE_LIGHT_IRIS_PHYSX_