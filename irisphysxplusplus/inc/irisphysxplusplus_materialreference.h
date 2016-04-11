/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    irisphysxplusplus_materialreference.h

Abstract:

    This file contains the declarations for the 
    IrisPysx++ MaterialReference type.

--*/

#include <irisphysxplusplus.h>

#ifndef _MATERIAL_REFERENCE_IRIS_PHYSX_PLUS_PLUS_
#define _MATERIAL_REFERENCE_IRIS_PHYSX_PLUS_PLUS_

namespace IrisPhysx {

//
// Types
//

class MaterialReference final {
public:
    MaterialReference(
        _In_ PCPBR_MATERIAL MaterialPtr
        )
    : Data(MaterialPtr)
    { 
        if (MaterialPtr == nullptr)
        {
            throw std::invalid_argument("MaterialPtr");
        }
    }
    
    _Ret_
    BRDFReference
    Sample(
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

private:
    PCPBR_MATERIAL Data;
};

} // namespace Iris

#endif // _MATERIAL_REFERENCE_IRIS_PHYSX_PLUS_PLUS_