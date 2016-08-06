/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    irisphysx_lightedgeometry.h

Abstract:

    This file contains the definitions for the PHYSX_LIGHTED_GEOMETRY type.

--*/

#ifndef _PHYSX_LIGHTED_GEOMETRY_IRIS_PHYSX_INTERNAL_
#define _PHYSX_LIGHTED_GEOMETRY_IRIS_PHYSX_INTERNAL_

#include <irisphysxp.h>

//
// Functions
//

VOID
LightedGeometryRetain(
    _In_ PPHYSX_LIGHTED_GEOMETRY LightedGeometry
    );

VOID
LightedGeometryRelease(
    _In_ _Post_invalid_ PPHYSX_LIGHTED_GEOMETRY LightedGeometry
    );

#endif // _PHYSX_LIGHTED_GEOMETRY_IRIS_PHYSX_INTERNAL_