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
        _In_ PPHYSX_MATERIAL MaterialPtr,
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
            PhysxMaterialRetain(MaterialPtr);
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
    PPHYSX_MATERIAL
    AsPPHYSX_MATERIAL(
        void
        )
    {
        return Data;
    }
    
    _Ret_
    PCPHYSX_MATERIAL
    AsPCPHYSX_MATERIAL(
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
        
        ISTATUS Status = PhysxMaterialSample(Data,
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
        PhysxMaterialRetain(Data);
    }

    Material & 
    operator=(
        _In_ const Material & ToCopy
        )
    {
        if (this != &ToCopy)
        {
            PhysxMaterialRetain(Data);
            Data = ToCopy.Data;
            PhysxMaterialRelease(Data);
        }

        return *this;
    }
    
    ~Material(
        void
        )
    {
        PhysxMaterialRelease(Data);
    }
    
private:
    PPHYSX_MATERIAL Data;
};

} // namespace Iris

#endif // _MATERIAL_IRIS_PHYSX_PLUS_PLUS_
