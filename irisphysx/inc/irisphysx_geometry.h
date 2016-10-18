/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    irisphysx_geometry.h

Abstract:

    This file contains the definitions for the PHYSX_GEOMETRY type.

--*/

#ifndef _PHYSX_GEOMETRY_IRIS_PHYSX_
#define _PHYSX_GEOMETRY_IRIS_PHYSX_

#include <irisphysx.h>

//
// Types
//

typedef
_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
(*PPHYSX_GEOMETRY_TEST_RAY_ROUTINE)(
    _In_ PCVOID Context,
    _In_ RAY Ray,
    _In_ PPHYSX_HIT_ALLOCATOR HitAllocator,
    _Out_ PHIT_LIST *HitList
    );

typedef
_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
(*PPHYSX_GEOMETRY_COMPUTE_NORMAL_ROUTINE)(
    _In_ PCVOID Context,
    _In_ POINT3 HitPoint,
    _In_ UINT32 FaceHit,
    _Out_ PVECTOR3 SurfaceNormal
    );

typedef
_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
(*PPHYSX_GEOMETRY_CHECK_BOUNDS_ROUTINE)(
    _In_ PCVOID Context,
    _In_ PCMATRIX ModelToWorld,
    _In_ BOUNDING_BOX WorldAlignedBoundingBox,
    _Out_ PBOOL IsInsideBox
    );

typedef
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
(*PPHYSX_GEOMETRY_GET_MATERIAL_ROUTINE)(
    _In_opt_ PCVOID Context, 
    _In_ UINT32 FaceHit,
    _Outptr_result_maybenull_ PCPHYSX_MATERIAL *Material
    );

typedef struct _PHYSX_GEOMETRY_VTABLE {
    PPHYSX_GEOMETRY_TEST_RAY_ROUTINE TestRayRoutine;
    PPHYSX_GEOMETRY_COMPUTE_NORMAL_ROUTINE ComputeNormalRoutine;
    PPHYSX_GEOMETRY_CHECK_BOUNDS_ROUTINE CheckBoundsRoutine;
    PPHYSX_GEOMETRY_GET_MATERIAL_ROUTINE GetMaterialRoutine;
    PFREE_ROUTINE FreeRoutine;
} PHYSX_GEOMETRY_VTABLE, *PPHYSX_GEOMETRY_VTABLE;

typedef CONST PHYSX_GEOMETRY_VTABLE *PCPHYSX_GEOMETRY_VTABLE;

typedef struct _PHYSX_GEOMETRY PHYSX_GEOMETRY, *PPHYSX_GEOMETRY;
typedef CONST PHYSX_GEOMETRY *PCPHYSX_GEOMETRY;

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISPHYSXAPI
ISTATUS
PhysxGeometryAllocate(
    _In_ PCPHYSX_GEOMETRY_VTABLE GeometryVTable,
    _When_(DataSizeInBytes != 0, _In_reads_bytes_opt_(DataSizeInBytes)) PCVOID Data,
    _In_ SIZE_T DataSizeInBytes,
    _When_(DataSizeInBytes != 0, _Pre_satisfies_(_Curr_ != 0 && (_Curr_ & (_Curr_ - 1)) == 0 && DataSizeInBytes % _Curr_ == 0)) SIZE_T DataAlignment,
    _Out_ PPHYSX_GEOMETRY *Geometry
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISPHYSXAPI
ISTATUS
PhysxGeometryTestNestedGeometry(
    _In_ PCPHYSX_GEOMETRY Geometry,
    _In_ PPHYSX_HIT_ALLOCATOR HitAllocator,
    _Out_ PHIT_LIST *HitList
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISPHYSXAPI
ISTATUS 
PhysxGeometryCheckBounds(
    _In_ PCPHYSX_GEOMETRY Geometry,
    _In_ PCMATRIX ModelToWorld,
    _In_ BOUNDING_BOX WorldAlignedBoundingBox,
    _Out_ PBOOL IsInsideBox
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISPHYSXAPI
ISTATUS
PhysxGeometryComputeNormal(
    _In_ PCPHYSX_GEOMETRY Geometry,
    _In_ POINT3 ModelHitPoint,
    _In_ UINT32 FaceHit,
    _Out_ PVECTOR3 ModelSurfaceNormal
    );

_Success_(return == ISTATUS_SUCCESS)
IRISPHYSXAPI
ISTATUS
PhysxGeometryGetMaterial(
    _In_ PCPHYSX_GEOMETRY Geometry,
    _In_ UINT32 FaceHit,
    _Outptr_result_maybenull_ PCPHYSX_MATERIAL *Material
    );

_Success_(return == ISTATUS_SUCCESS)
IRISPHYSXAPI
ISTATUS
PhysxGeometryGetLight(
    _In_ PCPHYSX_GEOMETRY Geometry,
    _In_ UINT32 FaceHit,
    _Outptr_result_maybenull_ PCPHYSX_LIGHT *Light
    );

IRISPHYSXAPI
VOID
PhysxGeometryRetain(
    _In_opt_ PPHYSX_GEOMETRY Geometry
    );

IRISPHYSXAPI
VOID
PhysxGeometryRelease(
    _In_opt_ _Post_invalid_ PPHYSX_GEOMETRY Geometry
    );

#endif // _PHYSX_GEOMETRY_IRIS_PHYSX_
