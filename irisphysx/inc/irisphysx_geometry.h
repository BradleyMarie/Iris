/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    irisphysx_geometry.h

Abstract:

    This file contains the definitions for the PBR_GEOMETRY type.

--*/

#ifndef _PBR_GEOMETRY_IRIS_PHYSX_
#define _PBR_GEOMETRY_IRIS_PHYSX_

#include <irisphysx.h>

//
// Types
//

typedef
_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
(*PPBR_GEOMETRY_TEST_RAY_ROUTINE)(
    _In_ PCVOID Context,
    _In_ RAY Ray,
    _In_ PPBR_HIT_ALLOCATOR PBRHitAllocator,
    _Out_ PHIT_LIST *HitList
    );

typedef
_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
(*PPBR_GEOMETRY_COMPUTE_NORMAL_ROUTINE)(
    _In_ PCVOID Context,
    _In_ POINT3 HitPoint,
    _In_ UINT32 FaceHit,
    _Out_ PVECTOR3 SurfaceNormal
    );

typedef
_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
(*PPBR_GEOMETRY_CHECK_BOUNDS_ROUTINE)(
    _In_ PCVOID Context,
    _In_ PCMATRIX ModelToWorld,
    _In_ BOUNDING_BOX WorldAlignedBoundingBox,
    _Out_ PBOOL IsInsideBox
    );

typedef
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
(*PPBR_GEOMETRY_GET_MATERIAL_ROUTINE)(
    _In_opt_ PCVOID Context, 
    _In_ UINT32 FaceHit,
    _Outptr_result_maybenull_ PCPBR_MATERIAL *Material
    );

typedef struct _PBR_GEOMETRY_VTABLE {
    PPBR_GEOMETRY_TEST_RAY_ROUTINE TestRayRoutine;
    PPBR_GEOMETRY_COMPUTE_NORMAL_ROUTINE ComputeNormalRoutine;
    PPBR_GEOMETRY_CHECK_BOUNDS_ROUTINE CheckBoundsRoutine;
    PPBR_GEOMETRY_GET_MATERIAL_ROUTINE GetMaterialRoutine;
    PFREE_ROUTINE FreeRoutine;
} PBR_GEOMETRY_VTABLE, *PPBR_GEOMETRY_VTABLE;

typedef CONST PBR_GEOMETRY_VTABLE *PCPBR_GEOMETRY_VTABLE;

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISPHYSXAPI
ISTATUS
PBRGeometryAllocate(
    _In_ PCPBR_GEOMETRY_VTABLE PBRGeometryVTable,
    _When_(DataSizeInBytes != 0, _In_reads_bytes_opt_(DataSizeInBytes)) PCVOID Data,
    _In_ SIZE_T DataSizeInBytes,
    _When_(DataSizeInBytes != 0, _Pre_satisfies_(_Curr_ != 0 && (_Curr_ & (_Curr_ - 1)) == 0 && DataSizeInBytes % _Curr_ == 0)) SIZE_T DataAlignment,
    _Out_ PPBR_GEOMETRY *PBRGeometry
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISPHYSXAPI
ISTATUS
PBRGeometryTestNestedGeometry(
    _In_ PCPBR_GEOMETRY PBRGeometry,
    _In_ PPBR_HIT_ALLOCATOR PBRHitAllocator,
    _Out_ PHIT_LIST *HitList
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISPHYSXAPI
ISTATUS 
PBRGeometryCheckBounds(
    _In_ PCPBR_GEOMETRY PBRGeometry,
    _In_ PCMATRIX ModelToWorld,
    _In_ BOUNDING_BOX WorldAlignedBoundingBox,
    _Out_ PBOOL IsInsideBox
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISPHYSXAPI
ISTATUS
PBRGeometryComputeNormal(
    _In_ PCPBR_GEOMETRY PBRGeometry,
    _In_ POINT3 ModelHitPoint,
    _In_ UINT32 FaceHit,
    _Out_ PVECTOR3 ModelSurfaceNormal
    );

_Success_(return == ISTATUS_SUCCESS)
IRISPHYSXAPI
ISTATUS
PBRGeometryGetMaterial(
    _In_ PCPBR_GEOMETRY PBRGeometry,
    _In_ UINT32 FaceHit,
    _Outptr_result_maybenull_ PCPBR_MATERIAL *Material
    );

_Success_(return == ISTATUS_SUCCESS)
IRISPHYSXAPI
ISTATUS
PBRGeometryGetLight(
    _In_ PCPBR_GEOMETRY PBRGeometry,
    _In_ UINT32 FaceHit,
    _Outptr_result_maybenull_ PCPBR_LIGHT *Light
    );

IRISPHYSXAPI
VOID
PBRGeometryRetain(
    _In_opt_ PPBR_GEOMETRY PBRGeometry
    );

IRISPHYSXAPI
VOID
PBRGeometryRelease(
    _In_opt_ _Post_invalid_ PPBR_GEOMETRY PBRGeometry
    );

#endif // _PPBR_GEOMETRY_IRIS_PHYSX_