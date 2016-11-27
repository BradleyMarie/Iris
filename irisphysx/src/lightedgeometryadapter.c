/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    lightedgeometryadapter.c

Abstract:

    This file contains the definitions for the PHYSX_LIGHTED_GEOMETRY_ADAPTER type.

--*/

#include <irisphysxp.h>

//
// Macros
//

#define PHYSX_LIGHTED_GEOMETRY_MAX_ATTACHED_LIGHTS \
    (SIZE_MAX / sizeof(PHYSX_LIGHTED_GEOMETRY))

//
// Types
//

typedef struct _PHYSX_ATTACHED_LIGHT {
    PCPHYSX_LIGHT Light;
    UINT32 Face;
} PHYSX_ATTACHED_LIGHT, *PPHYSX_ATTACHED_LIGHT;

typedef CONST PHYSX_ATTACHED_LIGHT *PCPHYSX_ATTACHED_LIGHT;

typedef struct _PHYSX_LIGHTED_GEOMETRY_ADAPTER {
    PPHYSX_LIGHTED_GEOMETRY LightedGeometry;
    _Field_size_(NumberOfLights) PPHYSX_ATTACHED_LIGHT Lights;
    SIZE_T NumberOfLights;
} PHYSX_LIGHTED_GEOMETRY_ADAPTER, *PPHYSX_LIGHTED_GEOMETRY_ADAPTER;

typedef CONST PHYSX_LIGHTED_GEOMETRY_ADAPTER *PCPHYSX_LIGHTED_GEOMETRY_ADAPTER;

//
// Lighted Geometry Static Functions
//

_Success_(return == ISTATUS_SUCCESS)
STATIC
ISTATUS 
PhysxLightedGeometryAdapterGetMaterial(
    _In_ PCVOID Context, 
    _In_ UINT32 FaceHit,
    _Out_opt_ PCPHYSX_MATERIAL *Material
    )
{
    PCPHYSX_LIGHTED_GEOMETRY_ADAPTER LightedGeometryAdapter;
    ISTATUS Status;

    ASSERT(Context != NULL);
    
    LightedGeometryAdapter = (PCPHYSX_LIGHTED_GEOMETRY_ADAPTER) Context;
    
    Status = PhysxLightedGeometryGetMaterial(LightedGeometryAdapter->LightedGeometry,
                                             FaceHit,
                                             Material);

    return Status;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
STATIC
ISTATUS
PhysxLightedGeometryAdapterComputeNormal(
    _In_ PCVOID Context,
    _In_ UINT32 FaceHit,
    _In_ POINT3 ModelHitPoint,
    _Out_ PVECTOR3 SurfaceNormal
    )
{
    PCPHYSX_LIGHTED_GEOMETRY_ADAPTER LightedGeometryAdapter;
    ISTATUS Status;

    ASSERT(Context != NULL);
    ASSERT(SurfaceNormal != NULL);
    
    LightedGeometryAdapter = (PCPHYSX_LIGHTED_GEOMETRY_ADAPTER) Context;
    
    Status = PhysxLightedGeometryComputeNormal(LightedGeometryAdapter->LightedGeometry,
                                               FaceHit,
                                               ModelHitPoint,
                                               SurfaceNormal);
                                   
    return Status;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
STATIC
ISTATUS 
PhysxLightedGeometryAdapterTestBounds(
    _In_ PCVOID Context,
    _In_ PCMATRIX ModelToWorld,
    _In_ BOUNDING_BOX WorldAlignedBoundingBox,
    _Out_ PBOOL IsInsideBox
    )
{
    PCPHYSX_LIGHTED_GEOMETRY_ADAPTER LightedGeometryAdapter;
    ISTATUS Status;

    ASSERT(Context != NULL);
    ASSERT(IsInsideBox != NULL);
    
    LightedGeometryAdapter = (PCPHYSX_LIGHTED_GEOMETRY_ADAPTER) Context;
    
    Status = PhysxLightedGeometryCheckBounds(LightedGeometryAdapter->LightedGeometry,
                                             ModelToWorld,
                                             WorldAlignedBoundingBox,
                                             IsInsideBox);
    
    return Status;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
STATIC
ISTATUS 
PhysxLightedGeometryAdapterTestRay(
    _In_opt_ PCVOID Context, 
    _In_ RAY Ray,
    _Inout_ PPHYSX_HIT_ALLOCATOR HitAllocator,
    _Outptr_result_maybenull_ PHIT_LIST *HitList
    )
{
    PCPHYSX_LIGHTED_GEOMETRY_ADAPTER LightedGeometryAdapter;
    ISTATUS Status;

    ASSERT(Context != NULL);
    ASSERT(RayValidate(Ray) != FALSE);
    ASSERT(HitAllocator != NULL);
    ASSERT(HitList != NULL);
    
    LightedGeometryAdapter = (PCPHYSX_LIGHTED_GEOMETRY_ADAPTER) Context;

    Status = PhysxLightedGeometryTestRay(LightedGeometryAdapter->LightedGeometry,
                                         Ray,
                                         HitAllocator,
                                         HitList);

    return Status;
}

STATIC
VOID
PhysxLightedGeometryAdapterFree(
    _In_ _Post_invalid_ PVOID Context
    )
{
    PCPHYSX_LIGHTED_GEOMETRY_ADAPTER LightedGeometryAdapter;

    ASSERT(Context != NULL);

    LightedGeometryAdapter = (PCPHYSX_LIGHTED_GEOMETRY_ADAPTER) Context;

    PhysxLightedGeometryFree(LightedGeometryAdapter->LightedGeometry);
    free(LightedGeometryAdapter->Lights);
}

//
// Static Variables
//

CONST PHYSX_GEOMETRY_VTABLE LightedGeometryAdapterVTable = {
    PhysxLightedGeometryAdapterTestRay,
    PhysxLightedGeometryAdapterComputeNormal,
    PhysxLightedGeometryAdapterTestBounds,
    PhysxLightedGeometryAdapterGetMaterial,
    PhysxLightedGeometryAdapterFree
};

//
// Static Functions
//

int 
PhysxLightedGeometryAdapterCompareLights(
    _In_ const void * Ptr0,
    _In_ const void * Ptr1
    )
{
    PCPHYSX_ATTACHED_LIGHT AttachedLight0;
    PCPHYSX_ATTACHED_LIGHT AttachedLight1;

    AttachedLight0 = (PCPHYSX_ATTACHED_LIGHT) Ptr0;
    AttachedLight1 = (PCPHYSX_ATTACHED_LIGHT) Ptr1;

    if (AttachedLight0->Face < AttachedLight1->Face)
    {
        return -1;
    }

    if (AttachedLight0->Face > AttachedLight1->Face)
    {
        return 1;
    }

    return 0;
}

//
// Internal Functions
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
    _Out_ PPHYSX_GEOMETRY *Geometry
    )
{
    PPHYSX_GEOMETRY AllocatedGeometry;
    PPHYSX_LIGHTED_GEOMETRY AllocatedLightedGeometry;
    PPHYSX_LIGHTED_GEOMETRY_ADAPTER AllocatedLightedGeometryAdapter;
    PVOID DataAllocation;
    PHYSX_LIGHTED_GEOMETRY_ADAPTER LightedGeometryAdapterData;
    ISTATUS Status;

    ASSERT(ReferenceCount != NULL);
    ASSERT(ReferenceCount != 0);
    ASSERT(PhysxLightedGeometryVTable != NULL);
    ASSERT(DataSizeInBytes == 0 || 
           (Data != NULL && DataAlignment != 0 && 
           (DataAlignment & DataAlignment - 1) == 0 &&
           DataSizeInBytes % DataAlignment == 0));
    ASSERT(LightedGeometry != NULL);
    ASSERT(LightedGeometryAdapter != NULL);
    ASSERT(Geometry != NULL);

    if (PHYSX_LIGHTED_GEOMETRY_MAX_ATTACHED_LIGHTS < NumberOfAttachedLights)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    LightedGeometryAdapterData.NumberOfLights = 0;
    LightedGeometryAdapterData.LightedGeometry = NULL;

    LightedGeometryAdapterData.Lights = malloc(sizeof(PHYSX_ATTACHED_LIGHT) * NumberOfAttachedLights);

    if (LightedGeometryAdapterData.Lights == NULL)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    Status = PhysxGeometryAllocateInternal(&LightedGeometryAdapterVTable,
                                           ReferenceCount,
                                           &LightedGeometryAdapterData,
                                           sizeof(PHYSX_LIGHTED_GEOMETRY_ADAPTER),
                                           _Alignof(PHYSX_LIGHTED_GEOMETRY_ADAPTER),
                                           &AllocatedGeometry);

    if (Status != ISTATUS_SUCCESS)
    {
        ASSERT(Status == ISTATUS_ALLOCATION_FAILED);
        free(LightedGeometryAdapterData.Lights);
        return Status;
    }

    //
    // The nested geometry must be allocated last. We should never call the
    // destructor of the nested geometry from this routine as it may have 
    // side affects.
    //

    Status = PhysxLightedGeometryAllocate(PhysxLightedGeometryVTable,
                                          Data,
                                          DataSizeInBytes,
                                          DataAlignment,
                                          &AllocatedLightedGeometry);

    if (Status != ISTATUS_SUCCESS)
    {
        ASSERT(Status == ISTATUS_ALLOCATION_FAILED);
        PhysxGeometryFree(AllocatedGeometry);
        return Status;
    }

    DataAllocation = PhysxGeometryGetMutableData(AllocatedGeometry);
    AllocatedLightedGeometryAdapter = (PPHYSX_LIGHTED_GEOMETRY_ADAPTER) DataAllocation;
    AllocatedLightedGeometryAdapter->LightedGeometry = AllocatedLightedGeometry;

    *LightedGeometry = AllocatedLightedGeometry;
    *LightedGeometryAdapter = AllocatedLightedGeometryAdapter;
    *Geometry = AllocatedGeometry;

    return ISTATUS_SUCCESS;
}

VOID
PhysxLightedGeometryAdapterAttachLight(
    _Inout_ PPHYSX_LIGHTED_GEOMETRY_ADAPTER LightedGeometryAdapter,
    _In_ PCPHYSX_LIGHT Light,
    _In_ UINT32 Face
    )
{
    ASSERT(LightedGeometryAdapter != NULL);
    ASSERT(Light != NULL);

    LightedGeometryAdapter->Lights[LightedGeometryAdapter->NumberOfLights].Light = Light;
    LightedGeometryAdapter->Lights[LightedGeometryAdapter->NumberOfLights].Face = Face;

    LightedGeometryAdapter->NumberOfLights += 1;
}

VOID
PhysxLightedGeometryAdapterFinalize(
    _Inout_ PPHYSX_LIGHTED_GEOMETRY_ADAPTER LightedGeometryAdapter
    )
{   
    ASSERT(LightedGeometryAdapter != NULL);
    
    qsort(LightedGeometryAdapter->Lights,
          LightedGeometryAdapter->NumberOfLights,
          sizeof(PHYSX_ATTACHED_LIGHT),
          PhysxLightedGeometryAdapterCompareLights);
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
PhysxLightedGeometryAdapterGetLight(
    _In_ PCPHYSX_GEOMETRY Geometry,
    _In_ UINT32 Face,
    _Outptr_result_maybenull_ PCPHYSX_LIGHT *Light
    )
{
    PCPHYSX_LIGHTED_GEOMETRY_ADAPTER LightedGeometryAdapter;
    PHYSX_ATTACHED_LIGHT Key;
    PCPHYSX_ATTACHED_LIGHT AttachedLight;
    void* SearchResult;
    PCVOID Data;

    ASSERT(Geometry != NULL);
    ASSERT(Light != NULL);

    Data = PhysxGeometryGetData(Geometry);
    LightedGeometryAdapter = (PCPHYSX_LIGHTED_GEOMETRY_ADAPTER) Data;

    Key.Face = Face;
    Key.Light = NULL;

    SearchResult = bsearch(&Key,
                           LightedGeometryAdapter->Lights,
                           LightedGeometryAdapter->NumberOfLights,
                           sizeof(PHYSX_ATTACHED_LIGHT),
                           PhysxLightedGeometryAdapterCompareLights);

    if (SearchResult == NULL)
    {
        *Light = NULL;
        return ISTATUS_SUCCESS;
    }

    AttachedLight = (PCPHYSX_ATTACHED_LIGHT) SearchResult;
    *Light = AttachedLight->Light;

    return ISTATUS_SUCCESS;
}

_Success_(return == ISTATUS_SUCCESS)
ISTATUS
PhysxLightedGeometryAdapterGetLightedGeometry(
    _In_ PCPHYSX_GEOMETRY Geometry,
    _Outptr_ PCPHYSX_LIGHTED_GEOMETRY *LightedGeometry
    )
{
    PCVOID Data;
    PCPHYSX_LIGHTED_GEOMETRY_ADAPTER LightedGeometryAdapter;
    
    ASSERT(Geometry != NULL);
    ASSERT(PhysxGeometryGetVTable(Geometry) == &LightedGeometryAdapterVTable);
    ASSERT(LightedGeometry != NULL);

    Data = PhysxGeometryGetData(Geometry);
    LightedGeometryAdapter = (PCPHYSX_LIGHTED_GEOMETRY_ADAPTER) Data;

    *LightedGeometry = LightedGeometryAdapter->LightedGeometry;

    return ISTATUS_SUCCESS;
}
