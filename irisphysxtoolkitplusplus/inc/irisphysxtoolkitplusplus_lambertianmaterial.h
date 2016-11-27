/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    irisphysxtoolkitplusplus_lambertianmaterial.h

Abstract:

    This file contains the definition for the LambertianMaterial type.

--*/

#include <irisphysxtoolkitplusplus.h>

#ifndef _LAMBERTIAN_MATERIAL_IRIS_PHYSX_TOOLKIT_PLUS_PLUS_
#define _LAMBERTIAN_MATERIAL_IRIS_PHYSX_TOOLKIT_PLUS_PLUS_

namespace IrisPhysxToolkit {
namespace LambertianMaterial {

//
// Functions
//

static
inline
IrisPhysx::Material
Create(
    _In_ IrisSpectrum::Reflector Reflector
    )
{
    PPHYSX_MATERIAL Material;
    ISTATUS Status = LambertianMaterialAllocate(Reflector.AsPREFLECTOR(),
                                                &Material);

    switch (Status)
    {
        case ISTATUS_SUCCESS:
            break;
        case ISTATUS_ALLOCATION_FAILED:
            throw std::bad_alloc();
    }

    assert(Status == ISTATUS_SUCCESS);
    return IrisPhysx::Material(Material, false);
}

} // namespace LambertianMaterial
} // namespace IrisPhysxToolkit

#endif // _LAMBERTIAN_MATERIAL_IRIS_PHYSX_TOOLKIT_PLUS_PLUS_
