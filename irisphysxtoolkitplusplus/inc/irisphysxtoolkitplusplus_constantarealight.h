/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    irisphysxtoolkitplusplus_constantarealight.h

Abstract:

    This file contains the definition for the ConstantAreaLight type.

--*/

#include <irisphysxtoolkitplusplus.h>

#ifndef _CONSTANT_AREA_LIGHT_IRIS_PHYSX_TOOLKIT_PLUS_PLUS_
#define _CONSTANT_AREA_LIGHT_IRIS_PHYSX_TOOLKIT_PLUS_PLUS_

namespace IrisPhysxToolkit {
namespace ConstantAreaLight {

//
// Functions
//

static
inline
SIZE_T
Create(
    _In_ IrisPhysx::AreaLightBuilder & Builder,
    _In_ IrisSpectrum::Spectrum Spectrum
    )
{
    SIZE_T AreaLightIndex;
    ISTATUS Status = PhysxConstantAreaLightAllocate(Builder.AsPPHYSX_AREA_LIGHT_BUILDER(),
                                                    Spectrum.AsPSPECTRUM(),
                                                    &AreaLightIndex);

    switch (Status)
    {
        case ISTATUS_SUCCESS:
            break;
        case ISTATUS_ALLOCATION_FAILED:
            throw std::bad_alloc();
    }

    assert(Status == ISTATUS_SUCCESS);
    return AreaLightIndex;
}

} // namespace ConstantAreaLight
} // namespace IrisPhysxToolkit

#endif // _CONSTANT_AREA_LIGHT_IRIS_PHYSX_TOOLKIT_PLUS_PLUS_
