/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    irisphysx_indirect.h

Abstract:

    This file contains the definitions for the INDIRECT_LIGHT type.

--*/

#ifndef _INDIRECT_LIGHT_IRIS_PHYSX_
#define _INDIRECT_LIGHT_IRIS_PHYSX_

#include <irisphysx.h>

//
// Types
//

typedef
_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
(*PINDIRECT_LIGHT_SAMPLE_ROUTINE)(
    _In_ PCVOID Context,
    _In_ POINT3 WorldHitPoint,
    _In_ VECTOR3 WorldViewer,
    _In_ PCVECTOR3 WorldNormalizedNormal,
    _In_opt_ PCVOID AdditionalData,
    _Inout_ PRANDOM Rng,
    _Inout_ PVOID RayTracer,
    _Inout_ PSPECTRUM_COMPOSITOR Compositor,
    _Out_ PCSPECTRUM *Output
    );

typedef struct _INDIRECT_LIGHT_VTABLE {
    PINDIRECT_LIGHT_SAMPLE_ROUTINE SampleRoutine;
    PFREE_ROUTINE FreeRoutine;
} INDIRECT_LIGHT_VTABLE, *PINDIRECT_LIGHT_VTABLE;

typedef CONST INDIRECT_LIGHT_VTABLE *PCINDIRECT_LIGHT_VTABLE;

typedef struct _INDIRECT_LIGHT INDIRECT_LIGHT, *PINDIRECT_LIGHT;
typedef CONST INDIRECT_LIGHT *PCINDIRECT_LIGHT;

//
// Functions
//

_Check_return_
_Ret_maybenull_
IRISPHYSXAPI
PINDIRECT_LIGHT
IndirectLightAllocate(
    _In_ PCINDIRECT_LIGHT_VTABLE IndirectLightVTable,
    _In_reads_bytes_(DataSizeInBytes) PCVOID Data,
    _In_ SIZE_T DataSizeInBytes,
    _In_ SIZE_T DataAlignment
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISPHYSXAPI
ISTATUS
IndirectLightSample(
    _In_ PCINDIRECT_LIGHT IndirectLight,
    _In_ POINT3 WorldHitPoint,
    _In_ VECTOR3 WorldViewer,
    _In_ PCVECTOR3 WorldNormalizedNormal,
    _In_opt_ PCVOID AdditionalData,
    _Inout_ PRANDOM Rng,
    _Inout_ PVOID RayTracer,
    _Inout_ PSPECTRUM_COMPOSITOR Compositor,
    _Out_ PCSPECTRUM *Output
    );

IRISPHYSXAPI
VOID
IndirectLightReference(
    _In_opt_ PINDIRECT_LIGHT IndirectLight
    );

IRISPHYSXAPI
VOID
IndirectLightDereference(
    _In_opt_ _Post_invalid_ PINDIRECT_LIGHT IndirectLight
    );

#endif // _INDIRECT_LIGHT_IRIS_PHYSX_