/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    irisplusplus_raytracerowner.h

Abstract:

    This file contains the definitions for the 
    Iris++ RayTracerOwner type.

--*/

#include <irisplusplus.h>

#ifndef _RAYTRACER_OWNER_IRIS_PLUS_PLUS_
#define _RAYTRACER_OWNER_IRIS_PLUS_PLUS_

namespace Iris {

//
// Types
//

class RayTracerOwner final {
public:
    IRISPLUSPLUSAPI
    RayTracerOwner(
        void
        );

    IRISPLUSPLUSAPI
    void
    Trace(
        _In_ Ray ToTrace,
        _In_ std::function<void(RayTracer &)> TraceRoutine
        );
        
    IRISPLUSPLUSAPI
    ~RayTracerOwner(
        void
        );

private:
    PRAYTRACER_OWNER Data;
};

} // namespace Iris

#endif // _RAYTRACER_OWNER_IRIS_PLUS_PLUS_