/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    lightedgeometry.c

Abstract:

    This file contains the definitions for the LIGHTED_GEOMETRY type.

--*/

#include <irisphysxp.h>

//
// Types
//

struct _PHYSX_LIGHTED_GEOMETRY {
    _Field_size_(NumberOfLights) PPBR_LIGHT *Lights;
    SIZE_T NumberOfLights;
    _Field_size_(NumberOfGeometry) PPBR_GEOMETRY *Geometry;
    SIZE_T NumberOfGeometry;
    SIZE_T ReferenceCount;
};

//
// Extern Variables
//

//
// Static Functions
//

//
// Functions
//

VOID
LightedGeometryRetain(
    _In_ PPHYSX_LIGHTED_GEOMETRY LightedGeometry
    )
{
    if (LightedGeometry == NULL)
    {
        return;
    }

    LightedGeometry->ReferenceCount += 1;
}

VOID
LightedGeometryRelease(
    _In_ _Post_invalid_ PPHYSX_LIGHTED_GEOMETRY LightedGeometry
    )
{
    SIZE_T ReferenceCount;

    if (LightedGeometry == NULL)
    {
        return;
    }

    ReferenceCount = (LightedGeometry->ReferenceCount -= 1);

    if (ReferenceCount == 0)
    {

    }
}