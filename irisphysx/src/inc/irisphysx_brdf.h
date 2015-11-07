/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    irisphysx_brdf.h

Abstract:

    This file contains the definitions for the BRDF type.

--*/

#ifndef _BRDF_IRIS_PHYSX_INTERNAL_
#define _BRDF_IRIS_PHYSX_INTERNAL_

#include <irisphysxp.h>

//
// Types
//

struct _BRDF {
    PCBRDF_VTABLE VTable;
    SIZE_T ReferenceCount;
    PVOID Data;
};

#endif // _BRDF_IRIS_PHYSX_INTERNAL_