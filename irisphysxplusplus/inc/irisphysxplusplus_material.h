/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    irisphysxplusplus_material.h

Abstract:

    This file contains the declarations for the 
    IrisPysx++ Material type.

--*/

#include <irisphysxplusplus.h>

#ifndef _MATERIAL_IRIS_PHYSX_PLUS_PLUS_
#define _MATERIAL_IRIS_PHYSX_PLUS_PLUS_

namespace IrisPhysx {

//
// Types
//

class Material final {
public:
    Material(
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

    Material(
        _In_ Material && ToMove
        )
    : Data(ToMove.Data)
    {
        ToMove.Data = nullptr;
    }
    
    _Ret_
    PPBR_MATERIAL
    AsPPBR_MATERIAL(
        void
        )
    {
        return Data;
    }
    
    _Ret_
    PCPBR_MATERIAL
    AsPCPBR_MATERIAL(
        void
        )
    {
        return Data;
    }
    
    MaterialReference
    AsMaterialReference(
        void
        ) const
    {
        return MaterialReference(Data);
    }

    std::tuple<BRDFReference, Iris::Vector>
    Sample(
        _In_ const Iris::Point & ModelHitPoint,
        _In_ const Iris::Vector & ModelSurfaceNormal,
        _In_ const Iris::Vector & WorldSurfaceNormal,
        _In_ PCVOID AdditionalData,
        _In_ const Iris::MatrixReference & ModelToWorld,
        _In_ BRDFAllocator Allocator
        ) const
    {
        PCPHYSX_BRDF ResultBrdf;
        VECTOR3 ResultVector;
        
        ISTATUS Status = PbrMaterialSample(Data,
                                           ModelHitPoint.AsPOINT3(),
                                           ModelSurfaceNormal.AsVECTOR3(),
                                           WorldSurfaceNormal.AsVECTOR3(),
                                           AdditionalData,
                                           ModelToWorld.AsPCMATRIX(),
                                           Allocator.AsPPHYSX_BRDF_ALLOCATOR(),
                                           &ResultVector,
                                           &ResultBrdf);

        if (Status != ISTATUS_SUCCESS)
        {
            throw std::runtime_error(Iris::ISTATUSToCString(Status));
        }
        
        return std::make_tuple(BRDFReference(ResultBrdf), Iris::Vector(ResultVector));
    }

    Material(
        _In_ const Material & ToCopy
        )
    : Data(ToCopy.Data)
    {
        PbrMaterialRetain(Data);
    }

    Material & 
    operator=(
        _In_ const Material & ToCopy
        )
    {
        if (this != &ToCopy)
        {
            PbrMaterialRetain(Data);
            Data = ToCopy.Data;
            PbrMaterialRelease(Data);
        }

        return *this;
    }
    
    ~Material(
        void
        )
    {
        PbrMaterialRelease(Data);    
    }
    
private:
    PPBR_MATERIAL Data;
};

} // namespace Iris

#endif // _MATERIAL_IRIS_PHYSX_PLUS_PLUS_
