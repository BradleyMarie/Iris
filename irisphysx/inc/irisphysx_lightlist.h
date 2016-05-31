/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    irisphysx_lightlist.h

Abstract:

    This file contains the definitions for the PHYSX_LIGHT_LIST type.

--*/

#ifndef _PHYSX_LIGHT_LIST_IRIS_PHYSX_
#define _PHYSX_LIGHT_LIST_IRIS_PHYSX_

#include <irisphysx.h>

//
// Types
//

typedef struct _PHYSX_LIGHT_LIST PHYSX_LIGHT_LIST, *PPHYSX_LIGHT_LIST;
typedef CONST PHYSX_LIGHT_LIST *PCPHYSX_LIGHT_LIST;

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISPHYSXAPI
ISTATUS
PhysxLightListAllocate(
    _Out_ PPHYSX_LIGHT_LIST *LightList
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISPHYSXAPI
ISTATUS
PhysxLightListAddLight(
    _In_ PPHYSX_LIGHT_LIST LightList,
    _In_ PPBR_LIGHT Light
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISPHYSXAPI
ISTATUS
PhysxLightListGetLight(
    _In_ PCPHYSX_LIGHT_LIST LightList,
    _In_ SIZE_T Index,
    _Out_ PCPBR_LIGHT *Light
    );

IRISPHYSXAPI
ISTATUS
PhysxLightListGetSize(
    _In_ PCPHYSX_LIGHT_LIST LightList,
    _Out_ PSIZE_T Size
    );

IRISPHYSXAPI
ISTATUS
PhysxLightListClear(
    _In_ PPHYSX_LIGHT_LIST LightList
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISPHYSXAPI
ISTATUS
PhysxLightListGetRandomLight(
    _In_ PCPHYSX_LIGHT_LIST LightList,
    _In_ PRANDOM_REFERENCE Rng,
    _Out_ PCPBR_LIGHT *Light
    );

IRISPHYSXAPI
VOID
PhysxLightListFree(
    _In_opt_ _Post_invalid_ PPHYSX_LIGHT_LIST LightList
    );

#endif // _PHYSX_LIGHT_LIST_IRIS_PHYSX_