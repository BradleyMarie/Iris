/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    irisphysx_arealightadapter.h

Abstract:

    This file contains the internal exports from the area light adapter.

--*/

#ifndef _PHYSX_AREA_LIGHT_ADAPTER_IRIS_PHYSX_INTERNAL_
#define _PHYSX_AREA_LIGHT_ADAPTER_IRIS_PHYSX_INTERNAL_

#include <irisphysxp.h>

//
// Type
//

typedef struct _PHYSX_AREA_LIGHT_ADAPTER PHYSX_AREA_LIGHT_ADAPTER, *PPHYSX_AREA_LIGHT_ADAPTER;
typedef CONST PHYSX_AREA_LIGHT_ADAPTER *PCPHYSX_AREA_LIGHT_ADAPTER;

//
// Variables
//

extern CONST PBR_LIGHT_VTABLE AreaLightAdapterVTable;

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
PhysxAreaLightAdapterAllocate(
    _In_ PPHYSX_AREA_LIGHT_REFERENCE_COUNT ReferenceCount,
    _In_ SIZE_T NumberOfAttachedGeometry,
    _In_ PCPHYSX_AREA_LIGHT_VTABLE AreaLightVTable,
    _When_(DataSizeInBytes != 0, _In_reads_bytes_opt_(DataSizeInBytes)) PCVOID Data,
    _In_ SIZE_T DataSizeInBytes,
    _When_(DataSizeInBytes != 0, _Pre_satisfies_(_Curr_ != 0 && (_Curr_ & (_Curr_ - 1)) == 0 && DataSizeInBytes % _Curr_ == 0)) SIZE_T DataAlignment,
    _Out_ PPHYSX_AREA_LIGHT_ADAPTER *AreaLightAdapter,
    _Out_ PPBR_LIGHT *Light
    );

VOID
PhysxAreaLightAdapterAttachGeometry(
    _Inout_ PPHYSX_AREA_LIGHT_ADAPTER AreaLightAdapter,
    _In_ PCPHYSX_LIGHTED_GEOMETRY LightedGeometry,
    _In_ UINT32 Face,
    _In_ FLOAT SurfaceArea
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
PhysxAreaLightAdapterFinalize(
    _Inout_ PPHYSX_AREA_LIGHT_ADAPTER AreaLightAdapter
    );

#endif // _PHYSX_AREA_LIGHT_ADAPTER_IRIS_PHYSX_INTERNAL_