/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    irisphysx_light.h

Abstract:

    This file contains the definitions for the PBR_LIGHT type.

--*/

#ifndef _PBR_LIGHT_IRIS_PHYSX_INTERNAL_
#define _PBR_LIGHT_IRIS_PHYSX_INTERNAL_

#include <irisphysxp.h>

//
// Prototypes
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
PhysxLightAllocateInternal(
    _In_ PCPHYSX_LIGHT_VTABLE LightVTable,
    _In_opt_ PPHYSX_AREA_LIGHT_REFERENCE_COUNT ReferenceCount,
    _When_(DataSizeInBytes != 0, _In_reads_bytes_opt_(DataSizeInBytes)) PCVOID Data,
    _In_ SIZE_T DataSizeInBytes,
    _When_(DataSizeInBytes != 0, _Pre_satisfies_(_Curr_ != 0 && (_Curr_ & (_Curr_ - 1)) == 0 && DataSizeInBytes % _Curr_ == 0)) SIZE_T DataAlignment,
    _Out_ PPHYSX_LIGHT *Light
    );

_Ret_
PVOID
PhysxLightGetData(
    _In_ PPHYSX_LIGHT Light
    );

VOID
PhysxLightFree(
    _In_ _Post_invalid_ PPHYSX_LIGHT Light
    );

#endif // _PBR_LIGHT_IRIS_PHYSX_INTERNAL_
