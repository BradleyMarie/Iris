/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    irisadvancedtoolkitplusplus_framebuffer.h

Abstract:

    This file contains the definitions for the 
    IrisAdvancedToolkit++ MultiplyWithCarryGenerator type.

--*/

#include <irisadvancedtoolkitplusplus.h>

#ifndef _MWC_IRIS_ADVANCED_TOOLKIT_PLUS_PLUS_HEADER_
#define _MWC_IRIS_ADVANCED_TOOLKIT_PLUS_PLUS_HEADER_

namespace IrisAdvancedToolkit {
namespace MultiplyWithCarryGenerator {

static
inline
IrisAdvanced::RandomGenerator
Create(
    void
    )
{
    PRANDOM_GENERATOR Generator;

    ISTATUS Status = MultiplyWithCarryRandomGeneratorAllocate(&Generator);

    if (Status != ISTATUS_SUCCESS)
    {
        assert(Status == ISTATUS_ALLOCATION_FAILED);
        throw std::bad_alloc();
    }

    return IrisAdvanced::RandomGenerator(Generator);
}

} // namespace MultiplyWithCarry
} // namespace IrisAdvancedToolkit

#endif // _MWC_IRIS_ADVANCED_TOOLKIT_PLUS_PLUS_HEADER_