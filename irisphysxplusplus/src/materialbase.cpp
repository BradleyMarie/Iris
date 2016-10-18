/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    materialbase.cpp

Abstract:

    This file contains the definitions for the 
    IrisPysx++ MaterialBase type.

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
MaterialSampleAdapter(
    _In_ PCVOID Context,
    _In_ POINT3 ModelHitPoint,
    _In_ VECTOR3 ModelSurfaceNormal,
    _In_ VECTOR3 WorldSurfaceNormal,
    _In_opt_ PCVOID AdditionalData,
    _In_opt_ PCMATRIX ModelToWorld,
    _Inout_ PPHYSX_BRDF_ALLOCATOR Allocator,
    _Out_ PVECTOR3 WorldShadingNormal,
    _Out_ PCPHYSX_BRDF *PbrBrdf
    )
{
    ASSERT(Context != NULL);
    ASSERT(PointValidate(ModelHitPoint) != FALSE);
    ASSERT(VectorValidate(ModelSurfaceNormal) != FALSE);
    ASSERT(VectorValidate(WorldSurfaceNormal) != FALSE);
    ASSERT(Allocator != NULL);
    ASSERT(PbrBrdf != NULL);

    const MaterialBase **MaterialBasePtr = (const MaterialBase**) Context;

    auto Result = (*MaterialBasePtr)->Sample(Iris::Point(ModelHitPoint),
                                             Iris::Vector(ModelSurfaceNormal),
                                             Iris::Vector(WorldSurfaceNormal),
                                             AdditionalData,
                                             Iris::MatrixReference(ModelToWorld),
                                             BRDFAllocator(Allocator));

    *WorldShadingNormal = std::get<1>(Result).AsVECTOR3();
    *PbrBrdf = std::get<0>(Result).AsPCPHYSX_BRDF();

    return ISTATUS_SUCCESS;
}

static
VOID
MaterialFreeAdapter(
    _In_ _Post_invalid_ PVOID Context
    )
{
    assert(Context != nullptr);

    MaterialBase **MaterialBasePtr = (MaterialBase**) Context;
    delete *MaterialBasePtr;
}

const static PHYSX_MATERIAL_VTABLE InteropVTable {
    MaterialSampleAdapter,
    MaterialFreeAdapter
};

//
// Functions
//

Material
MaterialBase::Create(
    _In_ std::unique_ptr<MaterialBase> MaterialBasePtr
    )
{
    if (!MaterialBasePtr)
    {
        throw std::invalid_argument("MaterialBasePtr");
    }
    
    MaterialBase *UnmanagedMaterialBasePtr = MaterialBasePtr.release();
    PPHYSX_MATERIAL MaterialPtr;

    ISTATUS Success = PhysxMaterialAllocate(&InteropVTable,
                                            &UnmanagedMaterialBasePtr,
                                            sizeof(MaterialBase*),
                                            alignof(MaterialBase*),
                                            &MaterialPtr);

    if (Success != ISTATUS_SUCCESS)
    {
        throw std::bad_alloc();
    }
    
    return Material(MaterialPtr, false);
}

} // namespace IrisPhysx
