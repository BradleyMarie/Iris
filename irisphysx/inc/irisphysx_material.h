/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    irisphysx_material.h

Abstract:

    This file contains the definitions for the MATERIAL type.

--*/

#ifndef _MATERIAL_IRIS_PHYSX_
#define _MATERIAL_IRIS_PHYSX_

#include <irisphysx.h>

//
// Types
//

typedef
_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
(*PMATERIAL_SAMPLE_ROUTINE)(
    _In_ PCVOID Context,
    _In_ POINT3 ModelHitPoint,
    _In_opt_ PCVOID AdditionalData,
    _In_ VECTOR3 ShapeSurfaceNormal,
    _In_ PCMATRIX ModelToWorld,
    _Inout_ PBRDF_ALLOCATOR BrdfAllocator,
    _Out_ PBRDF *Brdf
    );

typedef struct _MATERIAL_VTABLE {
    PMATERIAL_SAMPLE_ROUTINE SampleRoutine;
    PFREE_ROUTINE FreeRoutine;
} MATERIAL_VTABLE, *PMATERIAL_VTABLE;

typedef CONST MATERIAL_VTABLE *PCMATERIAL_VTABLE;

typedef struct _MATERIAL MATERIAL, *PMATERIAL;
typedef CONST MATERIAL *PCMATERIAL;

//
// Functions
//

_Check_return_
_Ret_maybenull_
IRISPHYSXAPI
PMATERIAL
MaterialAllocate(
    _In_ PCMATERIAL_VTABLE MaterialVTable,
    _In_reads_bytes_(DataSizeInBytes) PCVOID Data,
    _In_ SIZE_T DataSizeInBytes,
    _In_ SIZE_T DataAlignment
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISPHYSXAPI
ISTATUS
MaterialSample(
    _In_ PCMATERIAL Material,
    _In_ POINT3 ModelHitPoint,
    _In_opt_ PCVOID AdditionalData,
    _In_ VECTOR3 ShapeSurfaceNormal,
    _In_opt_ PCMATRIX ModelToWorld,
    _Inout_ PBRDF_ALLOCATOR BrdfAllocator,
    _Out_ PBRDF *Brdf
    );

IRISPHYSXAPI
VOID
MaterialReference(
    _In_opt_ PMATERIAL Material
    );

IRISPHYSXAPI
VOID
MaterialDereference(
    _In_opt_ _Post_invalid_ PMATERIAL Material
    );

#endif // _MATERIAL_IRIS_PHYSX_