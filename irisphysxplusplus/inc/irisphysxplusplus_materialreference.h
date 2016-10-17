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
    PCPBR_MATERIAL
    AsPCPBR_MATERIAL(
        void
        )
    {
        return Data;
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

private:
    PCPBR_MATERIAL Data;
};

} // namespace Iris

#endif // _MATERIAL_REFERENCE_IRIS_PHYSX_PLUS_PLUS_
