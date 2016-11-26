/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    irisphysxplusplus_materialbase.h

Abstract:

    This file contains the declarations for the 
    IrisPysx++ MaterialBase type.

--*/

#include <irisphysxplusplus.h>

#ifndef _MATERIAL_BASE_IRIS_PHYSX_PLUS_PLUS_
#define _MATERIAL_BASE_IRIS_PHYSX_PLUS_PLUS_

namespace IrisPhysx {

//
// Types
//

class MaterialBase {
protected:
    IRISPHYSXPLUSPLUSAPI
    static
    Material
    Create(
        _In_ std::unique_ptr<MaterialBase> MaterialBasePtr
        );

public:
    MaterialBase(
        void
        )
    { }
    
    virtual
    std::tuple<BRDFReference, Iris::Vector>
    Sample(
        _In_ const Iris::Point & ModelHitPoint,
        _In_ const Iris::Vector & ModelSurfaceNormal,
        _In_ const Iris::Vector & WorldSurfaceNormal,
        _In_ PCVOID AdditionalData,
        _In_ const Iris::MatrixReference & ModelToWorld,
        _In_ BRDFAllocator Allocator
        ) const = 0;
    
    MaterialBase(
        _In_ const MaterialBase & ToCopy
        ) = delete;
        
    MaterialBase &
    operator=(
        _In_ const MaterialBase & ToCopy
        ) = delete;

    virtual
    ~MaterialBase(
        void
        )
    { }
};

} // namespace IrisPhysx

#endif // _MATERIAL_BASE_IRIS_PHYSX_PLUS_PLUS_