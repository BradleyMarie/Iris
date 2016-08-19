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
PhysxAreaLightAllocate(
    _In_ PCPBR_LIGHT_VTABLE PbrLightVTable,
    _In_ PPHYSX_AREA_LIGHT_REFERENCE_COUNT ReferenceCount,
    _In_ SIZE_T AttachCount,
    _When_(DataSizeInBytes != 0, _In_reads_bytes_opt_(DataSizeInBytes)) PCVOID Data,
    _In_ SIZE_T DataSizeInBytes,
    _When_(DataSizeInBytes != 0, _Pre_satisfies_(_Curr_ != 0 && (_Curr_ & (_Curr_ - 1)) == 0 && DataSizeInBytes % _Curr_ == 0)) SIZE_T DataAlignment,
    _Out_ PPBR_LIGHT *PbrLight
    );

VOID
PhysxAreaLightAttachGeometry(
    _Inout_ PPBR_LIGHT AreaLight,
    _In_ PPBR_GEOMETRY Geometry,
    _In_ UINT32 Face
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
PhysxAreaLightFinalize(
    _Inout_ PPBR_LIGHT AreaLight
    );

VOID
PBRLightFree(
    _In_ _Post_invalid_ PPBR_LIGHT PBRLight
    );

#endif // _PBR_LIGHT_IRIS_PHYSX_INTERNAL_