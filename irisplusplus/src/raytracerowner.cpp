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

void
RayTracerOwner::Trace(
    _In_ Ray ToTrace,
    _In_ std::function<void(RayTracer &)> TraceRoutine
    )
{
    PRAYTRACER RayTracerPtr;
    
    ISTATUS Status = RayTracerOwnerGetRayTracer(Data,
                                                ToTrace.AsRAY(), 
                                                &RayTracerPtr);
    
    switch (Status)
    {
        case ISTATUS_SUCCESS:
            break;
        case ISTATUS_INVALID_ARGUMENT_01:
            throw std::invalid_argument("ToTrace");
            break;
        default:
            ASSERT(false);
    }
    
    RayTracer Tracer(RayTracerPtr);

    TraceRoutine(Tracer);
}

RayTracerOwner::~RayTracerOwner(
    void
    )
{
    RayTracerOwnerFree(Data);
}

} // namespace Iris