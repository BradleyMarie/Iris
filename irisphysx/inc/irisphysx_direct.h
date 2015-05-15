/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    irisphysx_direct.h

Abstract:

    This file contains the definitions for the DIRECT_LIGHT type.

--*/

#ifndef _DIRECT_LIGHT_IRIS_PHYSX_
#define _DIRECT_LIGHT_IRIS_PHYSX_

#include <irisphysx.h>

//
// Types
//

typedef
_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
(*PDIRECT_LIGHT_SAMPLE_ROUTINE)(
    _In_ PCVOID Context,
    _In_ POINT3 WorldHitPoint,
    _In_ VECTOR3 WorldViewer,
    _In_ PCVECTOR3 WorldNormalizedNormal,
    _In_opt_ PCVOID AdditionalData,
    _Inout_ PRANDOM Rng,
    _Inout_ PVISIBILITY_TESTER VisibilityTester,
    _Inout_ PSPECTRUM_COMPOSITOR Compositor,
    _Out_ PCSPECTRUM *Output
    );

typedef struct _DIRECT_LIGHT_VTABLE {
    PDIRECT_LIGHT_SAMPLE_ROUTINE SampleRoutine;
    PFREE_ROUTINE FreeRoutine;
} DIRECT_LIGHT_VTABLE, *PDIRECT_LIGHT_VTABLE;

typedef CONST DIRECT_LIGHT_VTABLE *PCDIRECT_LIGHT_VTABLE;

typedef struct _DIRECT_LIGHT DIRECT_LIGHT, *PDIRECT_LIGHT;
typedef CONST DIRECT_LIGHT *PCDIRECT_LIGHT;

//
// Functions
//

_Check_return_
_Ret_maybenull_
IRISPHYSXAPI
PDIRECT_LIGHT
DirectLightAllocate(
    _In_ PCDIRECT_LIGHT_VTABLE DirectLightVTable,
    _In_reads_bytes_(DataSizeInBytes) PCVOID Data,
    _In_ SIZE_T DataSizeInBytes,
    _In_ SIZE_T DataAlignment
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISPHYSXAPI
ISTATUS
DirectLightSample(
    _In_ PCDIRECT_LIGHT DirectLight,
    _In_ POINT3 WorldHitPoint,
    _In_ VECTOR3 WorldViewer,
    _In_ PCVECTOR3 WorldNormalizedNormal,
    _In_opt_ PCVOID AdditionalData,
    _Inout_ PRANDOM Rng,
    _Inout_ PVISIBILITY_TESTER VisibilityTester,
    _Inout_ PSPECTRUM_COMPOSITOR Compositor,
    _Out_ PCSPECTRUM *Output
    );

IRISPHYSXAPI
VOID
DirectLightReference(
    _In_opt_ PDIRECT_LIGHT DirectLight
    );

IRISPHYSXAPI
VOID
DirectLightDereference(
    _In_opt_ _Post_invalid_ PDIRECT_LIGHT DirectLight
    );

#endif // _DIRECT_LIGHT_IRIS_PHYSX_