/*++

Copyright (c) 2015 Brad Weinberger

Module Name:

    raytracerowner.cpp

Abstract:

    This file contains the definitions for the 
    Iris++ RayTracerOwner type.

--*/

#include <irisplusplus.h>

namespace Iris {

//
// Functions
//

RayTracerOwner::RayTracerOwner(
    void
    )
{
    ISTATUS Status = RayTracerOwnerAllocate(&Data);
    
    switch (Status)
    {
        case ISTATUS_SUCCESS:
            break;
        case ISTATUS_ALLOCATION_FAILED:
            throw std::bad_alloc();
            break;
        default:
            ASSERT(false);
    }
}

RayTracerOwner::~RayTracerOwner(
    void
    )
{
    RayTracerOwnerFree(Data);
}

} // namespace Iris