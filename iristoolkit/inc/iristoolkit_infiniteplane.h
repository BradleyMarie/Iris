/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    iristoolkit_infiniteplane.h

Abstract:

    This file contains the prototypes for the INFINITE_PLANE type.

--*/

#ifndef _INFINITE_PLANE_IRIS_TOOLKIT_
#define _INFINITE_PLANE_IRIS_TOOLKIT_

#include <iristoolkit.h>

//
// Prototypes
//

_Check_return_
_Ret_maybenull_
IRISTOOLKITAPI
PDRAWING_SHAPE
InfinitePlaneAllocate(
    _In_ PCPOINT3 Vertex,
    _In_ PCVECTOR3 SurfaceNormal,
    _In_opt_ PCTEXTURE FrontTexture,
    _In_opt_ PNORMAL FrontNormal,
    _In_opt_ PCTEXTURE BackTexture,
    _In_opt_ PNORMAL BackNormal
    );

#endif // _INFINITE_PLANE_IRIS_TOOLKIT_