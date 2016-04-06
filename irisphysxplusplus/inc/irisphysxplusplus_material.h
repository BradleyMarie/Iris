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
    IRISPHYSXPLUSPLUSAPI
    Material(
        _In_ PPBR_MATERIAL MaterialPtr,
        _In_ bool Retain
        );
    
    _Ret_
    BRDFReference
    Sample(
        _In_ const Iris::Point & ModelHitPoint,
        _In_ PCVOID AdditionalData,
        _In_ const Iris::Vector & SurfaceNormal,
        _In_ Iris::MatrixReference ModelToWorld,
        _In_ BRDFAllocator Allocator
        ) const;
    
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