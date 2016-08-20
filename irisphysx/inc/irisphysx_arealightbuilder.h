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

typedef
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
(*PPHYSX_LIGHTED_GEOMETRY_COMPUTE_SURFACE_AREA)(
    _In_opt_ PCVOID Context, 
    _In_ UINT32 FaceHit,
    _Out_ PFLOAT SurfaceArea
    );

typedef
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
(*PPHYSX_LIGHTED_GEOMETRY_SAMPLE_FACE)(
    _In_opt_ PCVOID Context, 
    _In_ UINT32 FaceHit,
    _Out_ PPOINT3 Sample
    );

typedef struct _PHYSX_LIGHTED_GEOMETRY_VTABLE {
    PBR_GEOMETRY_VTABLE Header;
    PPHYSX_LIGHTED_GEOMETRY_SAMPLE_FACE SampleFaceRoutine;
    PPHYSX_LIGHTED_GEOMETRY_COMPUTE_SURFACE_AREA ComputeSurfaceAreaRoutine;
} PHYSX_LIGHTED_GEOMETRY_VTABLE, *PPHYSX_LIGHTED_GEOMETRY_VTABLE;

typedef CONST PHYSX_LIGHTED_GEOMETRY_VTABLE *PCPHYSX_LIGHTED_GEOMETRY_VTABLE;

typedef
_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
(*PPBR_PHYSX_AREA_LIGHT_COMPUTE_EMISSIVE)(
    _In_ PCVOID Context,
    _In_ POINT3 OnLight,
    _Inout_ PRANDOM_REFERENCE Rng,
    _Inout_ PSPECTRUM_COMPOSITOR_REFERENCE Compositor,
    _Out_ PCSPECTRUM *Spectrum
    );

typedef struct _PHYSX_AREA_LIGHT_VTABLE {
    PPBR_PHYSX_AREA_LIGHT_COMPUTE_EMISSIVE ComputeEmissiveRoutine;
    PFREE_ROUTINE FreeRoutine;
} PHYSX_AREA_LIGHT_VTABLE, *PPHYSX_AREA_LIGHT_VTABLE;

typedef CONST PHYSX_AREA_LIGHT_VTABLE *PCPHYSX_AREA_LIGHT_VTABLE; 

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
    _In_ PCPHYSX_LIGHTED_GEOMETRY_VTABLE LightedGeometryVTable,
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
    _In_ PCPHYSX_AREA_LIGHT_VTABLE AreaLightVTable,
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