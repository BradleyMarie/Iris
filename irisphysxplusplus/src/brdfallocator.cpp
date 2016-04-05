/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    brdfallocator.cpp

Abstract:

    This file contains the definitions for the 
    IrisPysx++ BRDFAllocator type.

--*/

#include <irisphysxplusplusp.h>
namespace IrisPhysx {

BRDFAllocator::BRDFAllocator(
    _In_ PPBR_BRDF_ALLOCATOR BrdfAllocator
    )
: Data(BrdfAllocator)
{ 
    if (BrdfAllocator == nullptr)
    {
        throw std::invalid_argument("BrdfAllocator");
    }
}

} // namespace IrisPhysx