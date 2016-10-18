/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    irisphysx_material.h

Abstract:

    This file contains the definitions for the PHYSX_MATERIAL type.

--*/

#ifndef _PHYSX_MATERIAL_IRIS_PHYSX_
#define _PHYSX_MATERIAL_IRIS_PHYSX_

#include <irisphysx.h>

//
// Types
//

typedef
_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
(*PPHYSX_MATERIAL_SAMPLE_ROUTINE)(
    _In_ PCVOID Context,
    _In_ POINT3 ModelHitPoint,
    _In_ VECTOR3 ModelSurfaceNormal,
    _In_ VECTOR3 WorldSurfaceNormal,
    _In_opt_ PCVOID AdditionalData,
    _In_opt_ PCMATRIX ModelToWorld,
    _Inout_ PPHYSX_BRDF_ALLOCATOR Allocator,
    _Out_ PVECTOR3 WorldShadingNormal,
    _Out_ PCPHYSX_BRDF *Brdf
    );

typedef struct _PHYSX_MATERIAL_VTABLE {
    PPHYSX_MATERIAL_SAMPLE_ROUTINE SampleRoutine;
    PFREE_ROUTINE FreeRoutine;
} PHYSX_MATERIAL_VTABLE, *PPHYSX_MATERIAL_VTABLE;

typedef CONST PHYSX_MATERIAL_VTABLE *PCPHYSX_MATERIAL_VTABLE;

typedef struct _PHYSX_MATERIAL PHYSX_MATERIAL, *PPHYSX_MATERIAL;
typedef CONST PHYSX_MATERIAL *PCPHYSX_MATERIAL;

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISPHYSXAPI
ISTATUS
PhysxMaterialAllocate(
    _In_ PCPHYSX_MATERIAL_VTABLE MaterialVTable,
    _When_(DataSizeInBytes != 0, _In_reads_bytes_opt_(DataSizeInBytes)) PCVOID Data,
    _In_ SIZE_T DataSizeInBytes,
    _When_(DataSizeInBytes != 0, _Pre_satisfies_(_Curr_ != 0 && (_Curr_ & (_Curr_ - 1)) == 0 && DataSizeInBytes % _Curr_ == 0)) SIZE_T DataAlignment,
    _Out_ PPHYSX_MATERIAL *Material
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISPHYSXAPI
ISTATUS
PhysxMaterialSample(
    _In_ PCPHYSX_MATERIAL Material,
    _In_ POINT3 ModelHitPoint,
    _In_ VECTOR3 ModelSurfaceNormal,
    _In_ VECTOR3 WorldSurfaceNormal,
    _In_opt_ PCVOID AdditionalData,
    _In_opt_ PCMATRIX ModelToWorld,
    _Inout_ PPHYSX_BRDF_ALLOCATOR Allocator,
    _Out_ PVECTOR3 WorldShadingNormal,
    _Out_ PCPHYSX_BRDF *Brdf
    );

IRISPHYSXAPI
VOID
PhysxMaterialRetain(
    _In_opt_ PPHYSX_MATERIAL Material
    );

IRISPHYSXAPI
VOID
PhysxMaterialRelease(
    _In_opt_ _Post_invalid_ PPHYSX_MATERIAL Material
    );

#endif // _PHYSX_MATERIAL_IRIS_PHYSX_
