/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    materialreference.cpp

Abstract:

    This file contains the definitions for the 
    IrisPysx++ MaterialReference type.

--*/

#include <irisphysxplusplusp.h>
namespace IrisPhysx {

MaterialReference::MaterialReference(
    _In_ PCPBR_MATERIAL MaterialPtr
    )
: Data(MaterialPtr)
{ 
    if (MaterialPtr == nullptr)
    {
        throw std::invalid_argument("MaterialPtr");
    }
}

BRDFReference
MaterialReference::Sample(
    _In_ const Iris::Point & ModelHitPoint,
    _In_ PCVOID AdditionalData,
    _In_ const Iris::Vector & SurfaceNormal,
    _In_ Iris::MatrixReference ModelToWorld,
    _In_ BRDFAllocator Allocator
    ) const
{
    PCPBR_BRDF Result;
    
    ISTATUS Status = PbrMaterialSample(Data,
                                       ModelHitPoint.AsPOINT3(),
                                       AdditionalData,
                                       SurfaceNormal.AsVECTOR3(),
                                       ModelToWorld.AsPCMATRIX(),
                                       Allocator.AsPPBR_BRDF_ALLOCATOR(),
                                       &Result);

    if (Status != ISTATUS_SUCCESS)
    {
        throw std::runtime_error(Iris::ISTATUSToCString(Status));
    }
    
    return BRDFReference(Result);
}

} // namespace IrisPhysx