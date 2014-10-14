/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    irisplusplus_surfacenormal.h

Abstract:

    This file contains the prototypes for the 
    Iris++ Surface Normal type.

--*/

#include <irisplusplus.h>

#ifndef _SURFACE_NORMAL_IRIS_PLUS_PLUS_
#define _SURFACE_NORMAL_IRIS_PLUS_PLUS_

namespace Iris {

//
// Types
//

class SurfaceNormal final {
public:
    IRISPLUSPLUSAPI
    Vector
    ModelNormal(
        void
        );

    IRISPLUSPLUSAPI
    Vector
    WorldNormal(
        void
        );

    IRISPLUSPLUSAPI
    Vector
    NormalizedModelNormal(
        void
        );

    IRISPLUSPLUSAPI
    Vector
    NormalizedWorldNormal(
        void
        );

private:
    SurfaceNormal(
        _In_ PSURFACE_NORMAL IrisSurfaceNormal
        );

    PSURFACE_NORMAL Data;
};

} // namespace Iris

#endif // _SURFACE_NORMAL_IRIS_PLUS_PLUS_