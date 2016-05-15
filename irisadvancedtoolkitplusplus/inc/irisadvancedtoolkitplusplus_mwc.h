/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    irisadvancedtoolkitplusplus_framebuffer.h

Abstract:

    This file contains the definitions for the 
    IrisAdvancedToolkit++ MultiplyWithCarry type.

--*/

#include <irisadvancedtoolkitplusplus.h>

#ifndef _MWC_IRIS_ADVANCED_TOOLKIT_PLUS_PLUS_HEADER_
#define _MWC_IRIS_ADVANCED_TOOLKIT_PLUS_PLUS_HEADER_

namespace IrisAdvancedToolkit {
namespace MultiplyWithCarry {

static
inline
IrisAdvanced::Random
Create(
    void
    )
{
    PRANDOM Rng;

    ISTATUS Status = MultiplyWithCarryRngAllocate(&Rng);

    if (Status != ISTATUS_SUCCESS)
    {
        assert(Status == ISTATUS_ALLOCATION_FAILED);
        throw std::bad_alloc();
    }

    return IrisAdvanced::Random(Rng);
}

} // namespace MultiplyWithCarry
} // namespace IrisAdvancedToolkit

#endif // _MWC_IRIS_ADVANCED_TOOLKIT_PLUS_PLUS_HEADER_