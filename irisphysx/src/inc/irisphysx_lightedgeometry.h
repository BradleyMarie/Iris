/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    irisphysx_lightedgeometry.h

Abstract:

    This file contains the definitions for the PHYSX_LIGHTED_GEOMETRY type.

--*/

#ifndef _PHYSX_LIGHTED_GEOMETRY_IRIS_PHYSX_INTERNAL_
#define _PHYSX_LIGHTED_GEOMETRY_IRIS_PHYSX_INTERNAL_

#include <irisphysxp.h>

//
// Types
//

typedef struct _PHYSX_LIGHTED_GEOMETRY {
    PCPHYSX_LIGHTED_GEOMETRY_VTABLE VTable;
    PVOID Data;
} PHYSX_LIGHTED_GEOMETRY, *PPHYSX_LIGHTED_GEOMETRY;

typedef CONST PHYSX_LIGHTED_GEOMETRY *PCPHYSX_LIGHTED_GEOMETRY;

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS
PhysxLightedGeometryAllocate(
    _In_ PCPHYSX_LIGHTED_GEOMETRY_VTABLE PhysxLightedGeometryVTable,
    _When_(DataSizeInBytes != 0, _In_reads_bytes_opt_(DataSizeInBytes)) PCVOID Data,
    _In_ SIZE_T DataSizeInBytes,
    _When_(DataSizeInBytes != 0, _Pre_satisfies_(_Curr_ != 0 && (_Curr_ & (_Curr_ - 1)) == 0 && DataSizeInBytes % _Curr_ == 0)) SIZE_T DataAlignment,
    _Out_ PPHYSX_LIGHTED_GEOMETRY *LightedGeometry
    )
{
    PPHYSX_LIGHTED_GEOMETRY AllocatedLightedGeometry;
    BOOL AllocationSuccessful;
    PVOID HeaderAllocation;
    PVOID DataAllocation;

    ASSERT(PhysxLightedGeometryVTable != NULL);
    ASSERT(DataSizeInBytes == 0 || 
           (Data != NULL && DataAlignment != 0 && 
           (DataAlignment & DataAlignment - 1) == 0 &&
           DataSizeInBytes % DataAlignment == 0));
    ASSERT(LightedGeometry != NULL);

    AllocationSuccessful = IrisAlignedAllocWithHeader(sizeof(PHYSX_LIGHTED_GEOMETRY),
                                                      _Alignof(PHYSX_LIGHTED_GEOMETRY),
                                                      &HeaderAllocation,
                                                      DataSizeInBytes,
                                                      DataAlignment,
                                                      &DataAllocation,
                                                      NULL);

    if (AllocationSuccessful == FALSE)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    AllocatedLightedGeometry = (PPHYSX_LIGHTED_GEOMETRY) HeaderAllocation;

    AllocatedLightedGeometry->VTable = PhysxLightedGeometryVTable;
    AllocatedLightedGeometry->Data = DataAllocation;

    if (DataSizeInBytes != 0)
    {
        memcpy(DataAllocation, Data, DataSizeInBytes);
    }

    *LightedGeometry = AllocatedLightedGeometry;

    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS
PhysxLightedGeometryTestRay(
    _In_ PCPHYSX_LIGHTED_GEOMETRY LightedGeometry,
    _In_ RAY Ray,
    _In_ PPHYSX_HIT_ALLOCATOR HitAllocator,
    _Out_ PHIT_LIST *HitList
    )
{
    ISTATUS Status;
    
    ASSERT(LightedGeometry != NULL);
    ASSERT(RayValidate(Ray) != FALSE);
    ASSERT(HitAllocator != NULL);
    ASSERT(HitList != NULL);
    
    Status = LightedGeometry->VTable->Header.TestRayRoutine(LightedGeometry->Data,
                                                            Ray,
                                                            HitAllocator,
                                                            HitList);

    return Status;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS 
PhysxLightedGeometryCheckBounds(
    _In_ PCPHYSX_LIGHTED_GEOMETRY LightedGeometry,
    _In_ PCMATRIX ModelToWorld,
    _In_ BOUNDING_BOX WorldAlignedBoundingBox,
    _Out_ PBOOL IsInsideBox
    )
{
    ISTATUS Status;
    
    ASSERT(LightedGeometry != NULL);
    ASSERT(BoundingBoxValidate(WorldAlignedBoundingBox) != FALSE);
    ASSERT(IsInsideBox != NULL);
    
    Status = LightedGeometry->VTable->Header.CheckBoundsRoutine(LightedGeometry->Data,
                                                                ModelToWorld,
                                                                WorldAlignedBoundingBox,
                                                                IsInsideBox);

    return Status;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS
PhysxLightedGeometryComputeNormal(
    _In_ PCPHYSX_LIGHTED_GEOMETRY LightedGeometry,
    _In_ UINT32 Face,
    _In_ POINT3 ModelHitPoint,
    _Out_ PVECTOR3 ModelSurfaceNormal
    )
{
    ISTATUS Status;
    
    ASSERT(LightedGeometry != NULL);
    ASSERT(PointValidate(ModelHitPoint) != FALSE);
    ASSERT(ModelSurfaceNormal != NULL);
    
    Status = LightedGeometry->VTable->Header.ComputeNormalRoutine(LightedGeometry->Data,
                                                                  Face,
                                                                  ModelHitPoint,
                                                                  ModelSurfaceNormal);

    return Status;
}

_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS
PhysxLightedGeometryGetMaterial(
    _In_ PCPHYSX_LIGHTED_GEOMETRY LightedGeometry,
    _In_ UINT32 Face,
    _Outptr_result_maybenull_ PCPHYSX_MATERIAL *Material
    )
{
    ISTATUS Status;
    
    ASSERT(LightedGeometry != NULL);
    ASSERT(Material != NULL);
    
    Status = LightedGeometry->VTable->Header.GetMaterialRoutine(LightedGeometry->Data,
                                                                Face,
                                                                Material);

    return Status;
}

_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS
PhysxLightedGeometryComputeSurfaceArea(
    _In_ PCPHYSX_LIGHTED_GEOMETRY LightedGeometry,
    _In_ UINT32 Face,
    _Out_ PFLOAT SurfaceArea
    )
{
    ISTATUS Status;
    
    ASSERT(LightedGeometry != NULL);
    ASSERT(SurfaceArea != NULL);
    
    Status = LightedGeometry->VTable->ComputeSurfaceAreaRoutine(LightedGeometry->Data,
                                                                Face,
                                                                SurfaceArea);

    return Status;
}

_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS
PhysxLightedGeometrySampleSurface(
    _In_ PCPHYSX_LIGHTED_GEOMETRY LightedGeometry,
    _In_ UINT32 Face,
    _Inout_ PRANDOM Rng,
    _Out_ PPOINT3 Sample
    )
{
    ISTATUS Status;
    
    ASSERT(LightedGeometry != NULL);
    ASSERT(Rng != NULL);
    ASSERT(Sample != NULL);
    
    Status = LightedGeometry->VTable->SampleSurfaceRoutine(LightedGeometry->Data,
                                                           Face,
                                                           Rng,
                                                           Sample);

    return Status;
}

SFORCEINLINE
VOID
PhysxLightedGeometryFree(
    _In_opt_ _Post_invalid_ PPHYSX_LIGHTED_GEOMETRY LightedGeometry
    )
{
    if (LightedGeometry == NULL)
    {
        return;
    }

    LightedGeometry->VTable->Header.FreeRoutine(LightedGeometry->Data);
    IrisAlignedFree(LightedGeometry);
}

#endif // _PHYSX_LIGHTED_GEOMETRY_IRIS_PHYSX_INTERNAL_
