/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    iristoolkit_spherenormal.h

Abstract:

    This file contains the prototypes for the SPHERE_NORMAL type.

--*/

#ifndef _SPHERE_NORMAL_IRIS_TOOLKIT_
#define _SPHERE_NORMAL_IRIS_TOOLKIT_

#include <iristoolkit.h>

//
// Prototypes
//

_Check_return_
_Ret_maybenull_
IRISTOOLKITAPI
PNORMAL
SphereNormalAllocate(
    _In_ POINT3 Center,
    _In_ BOOL Front
    );

#endif // _SPHERE_NORMAL_IRIS_TOOLKIT_