/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    irisphysx_arealightbuilder.h

Abstract:

    This file contains the definitions for the PHYSX_AREA_LIGHT_BUILDER type.

--*/

#ifndef _PBR_AREA_LIGHT_BUILDER_IRIS_PHYSX_
#define _PBR_AREA_LIGHT_BUILDER_IRIS_PHYSX_

#include <irisphysx.h>

//
// Types
//

typedef struct _PHYSX_AREA_LIGHT_BUILDER PHYSX_AREA_LIGHT_BUILDER, *PPHYSX_AREA_LIGHT_BUILDER;
typedef CONST PHYSX_AREA_LIGHT_BUILDER *PCPHYSX_AREA_LIGHT_BUILDER;

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISPHYSXAPI
ISTATUS
PhysxAreaLightBuilderAllocate(
    _Out_ PPHYSX_AREA_LIGHT_BUILDER *Builder
    );
    
_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISPHYSXAPI
ISTATUS
PhysxAreaLightBuilderAddGeometry(
    _Inout_ PPHYSX_AREA_LIGHT_BUILDER Builder,
    _In_ PCPBR_GEOMETRY_VTABLE GeometryVTable,
    _When_(DataSizeInBytes != 0, _In_reads_bytes_opt_(DataSizeInBytes)) PCVOID Data,
    _In_ SIZE_T DataSizeInBytes,
    _When_(DataSizeInBytes != 0, _Pre_satisfies_(_Curr_ != 0 && (_Curr_ & (_Curr_ - 1)) == 0 && DataSizeInBytes % _Curr_ == 0)) SIZE_T DataAlignment,
    _Out_ PSIZE_T GeometryIndex
    );
    
_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISPHYSXAPI
ISTATUS
PhysxAreaLightBuilderAddLight(
    _Inout_ PPHYSX_AREA_LIGHT_BUILDER Builder,
    _In_ PCPBR_LIGHT_VTABLE LightVTable,
    _When_(DataSizeInBytes != 0, _In_reads_bytes_opt_(DataSizeInBytes)) PCVOID Data,
    _In_ SIZE_T DataSizeInBytes,
    _When_(DataSizeInBytes != 0, _Pre_satisfies_(_Curr_ != 0 && (_Curr_ & (_Curr_ - 1)) == 0 && DataSizeInBytes % _Curr_ == 0)) SIZE_T DataAlignment,
    _Out_ PSIZE_T LightIndex
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISPHYSXAPI
ISTATUS
PhysxAreaLightBuilderAttachLightToGeometry(
    _Inout_ PPHYSX_AREA_LIGHT_BUILDER Builder,
    _In_ SIZE_T GeometryIndex,
    _In_ UINT32 FaceIndex,
    _In_ SIZE_T LightIndex
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISPHYSXAPI
ISTATUS
PhysxAreaLightBuilderBuildLightsAndGeometry(
    _Inout_ PPHYSX_AREA_LIGHT_BUILDER Builder,
    _Outptr_result_buffer_(*NumberOfGeometry) PPBR_GEOMETRY **Geometry,
    _Out_ PSIZE_T NumberOfGeometry,
    _Outptr_result_buffer_(*NumberOfLights) PPBR_LIGHT **Lights,
    _Out_ PSIZE_T NumberOfLights
    );

IRISPHYSXAPI
VOID
PhysxAreaLightBuilderFree(
    _In_opt_ _Post_invalid_ PPHYSX_AREA_LIGHT_BUILDER Builder
    );

#endif // _PBR_AREA_LIGHT_BUILDER_IRIS_PHYSX_