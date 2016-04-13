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
    _In_opt_ PCVOID AdditionalData,
    _In_ VECTOR3 ShapeSurfaceNormal,
    _In_opt_ PCMATRIX ModelToWorld,
    _Inout_ PPBR_BRDF_ALLOCATOR Allocator,
    _Out_ PCPBR_BRDF *PbrBrdf
    )
{
    ASSERT(Context != NULL);
    ASSERT(PointValidate(ModelHitPoint) != FALSE);
    ASSERT(VectorValidate(SurfaceNormal) != FALSE);
    ASSERT(Allocator != NULL);
    ASSERT(PbrBrdf != NULL);

    const MaterialBase **MaterialBasePtr = (const MaterialBase**) Context;

    BRDFReference Result = (*MaterialBasePtr)->Sample(Iris::Point(ModelHitPoint),
                                                      AdditionalData,
                                                      Iris::Vector(ShapeSurfaceNormal),
                                                      Iris::MatrixReference(ModelToWorld),
                                                      BRDFAllocator(Allocator));

    *PbrBrdf = Result.AsPCPBR_BRDF();
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

const static PBR_MATERIAL_VTABLE InteropVTable {
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
    PPBR_MATERIAL MaterialPtr;

    ISTATUS Success = PbrMaterialAllocate(&InteropVTable,
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