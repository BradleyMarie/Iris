/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    iriscameratoolkitplusplus_mwcgenerator.h

Abstract:

    This file contains the definition for the MultiplyWithCarryGenerator type.

--*/

#include <irisphysxtoolkitplusplus.h>

#ifndef _MWC_GENERATOR_IRIS_CAMERA_TOOLKIT_PLUS_PLUS_
#define _MWC_GENERATOR_IRIS_CAMERA_TOOLKIT_PLUS_PLUS_

namespace IrisCameraToolkit {
namespace MultiplyWithCarryGenerator {

//
// Functions
//

static
inline
IrisCamera::RandomGenerator
Create(
    void
    )
{
    PRANDOM_GENERATOR RandomGeneratorPtr;
    ISTATUS Status = MultiplyWithCarryGeneratorAllocate(&RandomGeneratorPtr);

    switch (Status)
    {
        case ISTATUS_SUCCESS:
            break;
        case ISTATUS_ALLOCATION_FAILED:
            throw std::bad_alloc();
    }

    assert(Status == ISTATUS_SUCCESS);
    return IrisCamera::RandomGenerator(RandomGeneratorPtr);
}

} // namespace MultiplyWithCarryGenerator
} // namespace IrisCameraToolkit

#endif // _MWC_GENERATOR_IRIS_CAMERA_TOOLKIT_PLUS_PLUS_
