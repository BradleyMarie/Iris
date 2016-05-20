/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    irisphysx_material.h

Abstract:

    This file contains the definitions for the PBR_MATERIAL type.

--*/

#ifndef _PBR_MATERIAL_IRIS_PHYSX_
#define _PBR_MATERIAL_IRIS_PHYSX_

#include <irisphysx.h>

//
// Types
//

typedef
_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
(*PPBR_MATERIAL_SAMPLE_ROUTINE)(
    _In_ PCVOID Context,
    _In_ POINT3 ModelHitPoint,
    _In_opt_ PCVOID AdditionalData,
    _In_opt_ PCMATRIX ModelToWorld,
    _Inout_ PPBR_BRDF_ALLOCATOR BrdfAllocator,
    _Out_ PCPBR_BRDF *PbrBrdf
    );

typedef struct _PBR_MATERIAL_VTABLE {
    PPBR_MATERIAL_SAMPLE_ROUTINE SampleRoutine;
    PFREE_ROUTINE FreeRoutine;
} PBR_MATERIAL_VTABLE, *PPBR_MATERIAL_VTABLE;

typedef CONST PBR_MATERIAL_VTABLE *PCPBR_MATERIAL_VTABLE;

typedef struct _PBR_MATERIAL PBR_MATERIAL, *PPBR_MATERIAL;
typedef CONST PBR_MATERIAL *PCPBR_MATERIAL;

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISPHYSXAPI
ISTATUS
PbrMaterialAllocate(
    _In_ PCPBR_MATERIAL_VTABLE PbrMaterialVTable,
    _When_(DataSizeInBytes != 0, _In_reads_bytes_opt_(DataSizeInBytes)) PCVOID Data,
    _In_ SIZE_T DataSizeInBytes,
    _When_(DataSizeInBytes != 0, _Pre_satisfies_(_Curr_ != 0 && (_Curr_ & (_Curr_ - 1)) == 0 && DataSizeInBytes % _Curr_ == 0)) SIZE_T DataAlignment,
    _Out_ PPBR_MATERIAL *PbrMaterial
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISPHYSXAPI
ISTATUS
PbrMaterialSample(
    _In_ PCPBR_MATERIAL Material,
    _In_ POINT3 ModelHitPoint,
    _In_opt_ PCVOID AdditionalData,
    _In_opt_ PCMATRIX ModelToWorld,
    _Inout_ PPBR_BRDF_ALLOCATOR BrdfAllocator,
    _Out_ PCPBR_BRDF *Brdf
    );

IRISPHYSXAPI
VOID
PbrMaterialRetain(
    _In_opt_ PPBR_MATERIAL PbrMaterial
    );

IRISPHYSXAPI
VOID
PbrMaterialRelease(
    _In_opt_ _Post_invalid_ PPBR_MATERIAL PbrMaterial
    );

#endif // _PBR_MATERIAL_IRIS_PHYSX_