/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    irisphysx_lightedgeometryadapter.h

Abstract:

    This file contains the definitions for the PHYSX_LIGHTED_GEOMETRY_ADAPTER type.

--*/

#ifndef _PHYSX_LIGHTED_GEOMETRY_ADAPTER_IRIS_PHYSX_INTERNAL_
#define _PHYSX_LIGHTED_GEOMETRY_ADAPTER_IRIS_PHYSX_INTERNAL_

#include <irisphysxp.h>

//
// Types
//

typedef struct _PHYSX_LIGHTED_GEOMETRY_ADAPTER PHYSX_LIGHTED_GEOMETRY_ADAPTER, *PPHYSX_LIGHTED_GEOMETRY_ADAPTER;
typedef CONST PHYSX_LIGHTED_GEOMETRY_ADAPTER *PCPHYSX_LIGHTED_GEOMETRY_ADAPTER;

//
// Variables
//

extern CONST PBR_GEOMETRY_VTABLE LightedGeometryAdapterVTable;

//
// Prototypes
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
PhysxLightedGeometryAdapterAllocate(
    _In_ PPHYSX_AREA_LIGHT_REFERENCE_COUNT ReferenceCount,
    _In_ SIZE_T NumberOfAttachedLights,
    _In_ PCPHYSX_LIGHTED_GEOMETRY_VTABLE PhysxLightedGeometryVTable,
    _When_(DataSizeInBytes != 0, _In_reads_bytes_opt_(DataSizeInBytes)) PCVOID Data,
    _In_ SIZE_T DataSizeInBytes,
    _When_(DataSizeInBytes != 0, _Pre_satisfies_(_Curr_ != 0 && (_Curr_ & (_Curr_ - 1)) == 0 && DataSizeInBytes % _Curr_ == 0)) SIZE_T DataAlignment,
    _Out_ PCPHYSX_LIGHTED_GEOMETRY *LightedGeometry,
    _Out_ PPHYSX_LIGHTED_GEOMETRY_ADAPTER *LightedGeometryAdapter,
    _Out_ PPBR_GEOMETRY *Geometry
    );

VOID
PhysxLightedGeometryAdapterAttachLight(
    _Inout_ PPHYSX_LIGHTED_GEOMETRY_ADAPTER LightedGeometryAdapter,
    _In_ PCPBR_LIGHT Light,
    _In_ UINT32 Face
    );

VOID
PhysxLightedGeometryAdapterFinalize(
    _Inout_ PPHYSX_LIGHTED_GEOMETRY_ADAPTER LightedGeometryAdapter
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
PhysxLightedGeometryAdapterGetLight(
    _In_ PCPBR_GEOMETRY Geometry,
    _In_ UINT32 Face,
    _Outptr_result_maybenull_ PCPBR_LIGHT *Light
    );

#endif // _PHYSX_LIGHTED_GEOMETRY_ADAPTER_IRIS_PHYSX_INTERNAL_