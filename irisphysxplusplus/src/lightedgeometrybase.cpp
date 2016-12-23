/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    lightedgeometrybase.cpp

Abstract:

    This file contains the definitions for the IrisPysx++
    LightedGeometryBase and GeometryBase types.

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
    _In_ UINT32 FaceHit,
    _In_ POINT3 ModelHitPoint,
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

static
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
LightedGeometryComputeSurfaceAreaAdapter(
    _In_opt_ PCVOID Context, 
    _In_ UINT32 Face,
    _Out_ PFLOAT SurfaceArea
    )
{
    assert(Context != nullptr);
    assert(SurfaceArea != nullptr);

    const LightedGeometryBase **LightedGeometryBasePtr = (const LightedGeometryBase**) Context;

    FLOAT Result = (*LightedGeometryBasePtr)->ComputeSurfaceArea(Face);

    *SurfaceArea = Result;

    return ISTATUS_SUCCESS;
}

static
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
LightedGeometrySampleSurfaceAdapter(
    _In_opt_ PCVOID Context, 
    _In_ UINT32 Face,
    _Inout_ PRANDOM Rng,
    _Out_ PPOINT3 Sample
    )
{
    assert(Context != nullptr);
    assert(Rng != nullptr);
    assert(Sample != nullptr);

    const LightedGeometryBase **LightedGeometryBasePtr = (const LightedGeometryBase**) Context;

    Iris::Point Result = (*LightedGeometryBasePtr)->SampleSurface(Face,
                                                                  IrisAdvanced::Random(Rng));

    *Sample = Result.AsPOINT3();

    return ISTATUS_SUCCESS;
}

const static PHYSX_LIGHTED_GEOMETRY_VTABLE InteropVTable {
    { GeometryTestRayAdapter,
      GeometryComputeNormalAdapter,
      GeometryCheckBoundsAdapter,
      GeometryGetMaterialAdapter,
      GeometryFreeAdapter },
    LightedGeometryComputeSurfaceAreaAdapter,
    LightedGeometrySampleSurfaceAdapter
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

    ISTATUS Success = PhysxGeometryAllocate(&InteropVTable.Header,
                                            &UnmanagedGeometryBasePtr,
                                            sizeof(GeometryBase*),
                                            alignof(GeometryBase*),
                                            &GeometryPtr);

    if (Success != ISTATUS_SUCCESS)
    {
        delete UnmanagedGeometryBasePtr;
        throw std::bad_alloc();
    }
    
    return Geometry(GeometryPtr, false);
}

SIZE_T
LightedGeometryBase::Create(
    _In_ AreaLightBuilder & Builder,
    _In_ std::unique_ptr<LightedGeometryBase> LightedGeometryBasePtr
    )
{
    if (!LightedGeometryBasePtr)
    {
        throw std::invalid_argument("LightedGeometryBasePtr");
    }

    LightedGeometryBase *UnmanagedLightedGeometryBasePtr = LightedGeometryBasePtr.release();
    SIZE_T LightedGeometryIndex;

    ISTATUS Success = PhysxAreaLightBuilderAddGeometry(Builder.AsPPHYSX_AREA_LIGHT_BUILDER(),
                                                       &InteropVTable,
                                                       &UnmanagedLightedGeometryBasePtr,
                                                       sizeof(LightedGeometryBase*),
                                                       alignof(LightedGeometryBase*),
                                                       &LightedGeometryIndex);

    if (Success != ISTATUS_SUCCESS)
    {
        delete UnmanagedLightedGeometryBasePtr;
        throw std::bad_alloc();
    }
    
    return LightedGeometryIndex;
}

} // namespace IrisPhysx
