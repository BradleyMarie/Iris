/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    iristoolkit_sphere.h

Abstract:

    This file contains the prototypes for the SPHERE type.

--*/

#ifndef _SPHERE_IRIS_TOOLKIT_
#define _SPHERE_IRIS_TOOLKIT_

#include <iristoolkit.h>

//
// Prototypes
//

_Check_return_
_Ret_maybenull_
IRISTOOLKITAPI
PDRAWING_SHAPE
SphereAllocate(
    _In_ POINT3 Center,
    _In_ FLOAT Radius,
    _In_opt_ PTEXTURE FrontTexture,
    _In_opt_ PNORMAL FrontNormal,
    _In_opt_ PTEXTURE BackTexture,
    _In_opt_ PNORMAL BackNormal
    );

#endif // _SPHERE_IRIS_TOOLKIT_