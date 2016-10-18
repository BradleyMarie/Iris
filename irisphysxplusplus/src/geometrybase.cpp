/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    geometrybase.cpp

Abstract:

    This file contains the definitions for the 
    IrisPysx++ GeometryBase type.

--*/

#include <irisphysxplusplusp.h>
namespace IrisPhysx {

//
// Adapter Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
static
ISTATUS
GeometryTestRayAdapter(
    _In_ PCVOID Context,
    _In_ RAY ModelRay,
    _In_ PPHYSX_HIT_ALLOCATOR Allocator,
    _Out_ PHIT_LIST *HitList
    )
{
    assert(Context != nullptr);
    assert(RayValidate(ModelRay) != FALSE);
    assert(Allocator != nullptr);
    assert(HitList != nullptr);

    const GeometryBase **GeometryBasePtr = (const GeometryBase**) Context;

    PHIT_LIST Result = (*GeometryBasePtr)->TestRay(Iris::Ray(ModelRay), HitAllocator(Allocator));

    *HitList = Result;
    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
static
ISTATUS
GeometryComputeNormalAdapter(
    _In_ PCVOID Context,
    _In_ POINT3 ModelHitPoint,
    _In_ UINT32 FaceHit,
    _Out_ PVECTOR3 SurfaceNormal
    )
{
    assert(Context != nullptr);
    assert(PointValidate(ModelHitPoint) != FALSE);
    assert(SurfaceNormal != nullptr);

    const GeometryBase **GeometryBasePtr = (const GeometryBase**) Context;

    Iris::Vector Result = (*GeometryBasePtr)->ComputeNormal(Iris::Point(ModelHitPoint), FaceHit);

    *SurfaceNormal = Result.AsVECTOR3();
    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
static
ISTATUS
GeometryCheckBoundsAdapter(
    _In_ PCVOID Context,
    _In_ PCMATRIX ModelToWorld,
    _In_ BOUNDING_BOX WorldAlignedBoundingBox,
    _Out_ PBOOL IsInsideBox
    )
{
    assert(Context != nullptr);
    assert(BoundingBoxValidate(WorldAlignedBoundingBox) != FALSE);
    assert(IsInsideBox != nullptr);

    const GeometryBase **GeometryBasePtr = (const GeometryBase**) Context;

    bool Result = (*GeometryBasePtr)->CheckBounds(Iris::MatrixReference(ModelToWorld), 
                                                  WorldAlignedBoundingBox);

    *IsInsideBox = (Result) ? TRUE : FALSE;
    return ISTATUS_SUCCESS;
}

_Success_(return == ISTATUS_SUCCESS)
static
ISTATUS
GeometryGetMaterialAdapter(
    _In_opt_ PCVOID Context, 
    _In_ UINT32 FaceHit,
    _Outptr_result_maybenull_ PCPHYSX_MATERIAL *Material
    )
{
    assert(Context != nullptr);
    assert(Material != nullptr);

    const GeometryBase **GeometryBasePtr = (const GeometryBase**) Context;
    
    std::optional<MaterialReference> MaterialOpt = (*GeometryBasePtr)->GetMaterial(FaceHit);

    if (MaterialOpt)
    {
        *Material = MaterialOpt->AsPCPHYSX_MATERIAL();
    }
    else
    {
        *Material = nullptr;
    }

    return ISTATUS_SUCCESS;
}

static
VOID
GeometryFreeAdapter(
    _In_ _Post_invalid_ PVOID Context
    )
{
    assert(Context != nullptr);

    GeometryBase **GeometryBasePtr = (GeometryBase**) Context;
    delete *GeometryBasePtr;
}

const static PHYSX_GEOMETRY_VTABLE InteropVTable {
    GeometryTestRayAdapter,
    GeometryComputeNormalAdapter,
    GeometryCheckBoundsAdapter,
    GeometryGetMaterialAdapter,
    GeometryFreeAdapter
};

//
// Functions
//

Geometry
GeometryBase::Create(
    _In_ std::unique_ptr<GeometryBase> GeometryBasePtr
    )
{
    if (!GeometryBasePtr)
    {
        throw std::invalid_argument("GeometryBasePtr");
    }
    
    GeometryBase *UnmanagedGeometryBasePtr = GeometryBasePtr.release();
    PPHYSX_GEOMETRY GeometryPtr;

    ISTATUS Success = PhysxGeometryAllocate(&InteropVTable,
                                            &UnmanagedGeometryBasePtr,
                                            sizeof(GeometryBase*),
                                            alignof(GeometryBase*),
                                            &GeometryPtr);

    if (Success != ISTATUS_SUCCESS)
    {
        throw std::bad_alloc();
    }
    
    return Geometry(GeometryPtr, false);
}

} // namespace IrisPhysx
