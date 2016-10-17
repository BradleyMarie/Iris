/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    irisphysx_arealightreferencecount.h

Abstract:

    This file contains the exports from the area light reference count file.

--*/

#ifndef _PHYSX_AREA_LIGHT_REFERENCE_COUNT_PHYSX_INTERNAL_
#define _PHYSX_AREA_LIGHT_REFERENCE_COUNT_PHYSX_INTERNAL_

#include <irisphysxp.h>

//
// Types
//

typedef struct _PHYSX_AREA_LIGHT_REFERENCE_COUNT PHYSX_AREA_LIGHT_REFERENCE_COUNT, *PPHYSX_AREA_LIGHT_REFERENCE_COUNT;
typedef CONST PHYSX_AREA_LIGHT_REFERENCE_COUNT *PCPHYSX_AREA_LIGHT_REFERENCE_COUNT;

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
AreaLightReferenceCountAllocate(
    _In_ SIZE_T NumberOfGeometry,
    _In_ SIZE_T NumberOfLights,
    _Out_ PPHYSX_AREA_LIGHT_REFERENCE_COUNT *ReferenceCount
    );

VOID
AreaLightReferenceCountAddGeometry(
    _Inout_ PPHYSX_AREA_LIGHT_REFERENCE_COUNT ReferenceCount,
    _In_ PPHYSX_GEOMETRY Geometry
    );

VOID
AreaLightReferenceCountAddLight(
    _Inout_ PPHYSX_AREA_LIGHT_REFERENCE_COUNT ReferenceCount,
    _In_ PPBR_LIGHT Light
    );

VOID
AreaLightReferenceCountRetain(
    _In_ PPHYSX_AREA_LIGHT_REFERENCE_COUNT ReferenceCount
    );

VOID
AreaLightReferenceCountRelease(
    _In_ _Post_invalid_ PPHYSX_AREA_LIGHT_REFERENCE_COUNT ReferenceCount
    );

VOID
AreaLightReferenceCountFree(
    _In_ _Post_invalid_ PPHYSX_AREA_LIGHT_REFERENCE_COUNT AreaLightReferenceCount
    );

#endif // _PHYSX_AREA_LIGHT_REFERENCE_COUNT_PHYSX_INTERNAL_
