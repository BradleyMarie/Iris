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
    IRISPHYSXPLUSPLUSAPI
    MaterialReference(
        _In_ PCPBR_MATERIAL MaterialPtr
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
    
private:
    PCPBR_MATERIAL Data;
};

} // namespace Iris

#endif // _MATERIAL_REFERENCE_IRIS_PHYSX_PLUS_PLUS_