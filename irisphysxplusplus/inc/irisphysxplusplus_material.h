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
    
    _Ret_
    BRDFReference
    Sample(
        _In_ const Iris::Point & ModelHitPoint,
        _In_ PCVOID AdditionalData,
        _In_ const Iris::Vector & SurfaceNormal,
        _In_ const Iris::MatrixReference & ModelToWorld,
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