/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    material.cpp

Abstract:

    This file contains the definitions for the 
    IrisPysx++ Material type.

--*/

#include <irisphysxplusplusp.h>
namespace IrisPhysx {

Material::Material(
    _In_ PPBR_MATERIAL MaterialPtr,
    _In_ bool Retain
    )
: Data(MaterialPtr)
{ 
    if (MaterialPtr == nullptr)
    {
        throw std::invalid_argument("MaterialPtr");
    }
    
    if (Retain)
    {
        PbrMaterialRetain(MaterialPtr);
    }
}

BRDFReference
Material::Sample(
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