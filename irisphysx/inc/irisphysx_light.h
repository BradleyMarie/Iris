/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    irisphysx_light.h

Abstract:

    This file contains the definitions for the LIGHT type.

--*/

#ifndef _LIGHT_IRIS_PHYSX_
#define _LIGHT_IRIS_PHYSX_

#include <irisphysx.h>

//
// Types
//

typedef
_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
(*PLIGHT_SAMPLE)(
    _In_ PCVOID Context,
    _In_ POINT3 HitPoint,
    _Inout_ PVOID VisibilityTester,
    _Inout_ PRANDOM Rng,
    _Inout_ PSPECTRUM_COMPOSITOR Compositor,
    _Out_ PCSPECTRUM *Spectrum,
    _Out_ PVECTOR3 ToLight,
    _Out_ PFLOAT Pdf
    );

typedef struct _LIGHT_VTABLE {
    PLIGHT_SAMPLE SampleRoutine;
    PFREE_ROUTINE FreeRoutine;
} LIGHT_VTABLE, *PLIGHT_VTABLE;

typedef CONST LIGHT_VTABLE *PCLIGHT_VTABLE;

typedef struct _LIGHT LIGHT, *PLIGHT;
typedef CONST LIGHT *PCLIGHT;

//
// Functions
//

_Check_return_
_Ret_maybenull_
IRISPHYSXAPI
PLIGHT
LightAllocate(
    _In_ PCLIGHT_VTABLE LightVTable,
    _In_reads_bytes_(DataSizeInBytes) PCVOID Data,
    _In_ SIZE_T DataSizeInBytes,
    _In_ SIZE_T DataAlignment
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISPHYSXAPI
ISTATUS
LightSample(
    _In_ PCLIGHT Light,
    _In_ POINT3 HitPoint,
    _Inout_ PVOID VisibilityTester,
    _Inout_ PRANDOM Rng,
    _Inout_ PSPECTRUM_COMPOSITOR Compositor,
    _Out_ PCSPECTRUM *Spectrum,
    _Out_ PVECTOR3 ToLight,
    _Out_ PFLOAT Pdf
    );

IRISPHYSXAPI
VOID
LightReference(
    _In_opt_ PLIGHT Light
    );

IRISPHYSXAPI
VOID
LightDereference(
    _In_opt_ _Post_invalid_ PLIGHT Light
    );

#endif // _LIGHT_IRIS_PHYSX_