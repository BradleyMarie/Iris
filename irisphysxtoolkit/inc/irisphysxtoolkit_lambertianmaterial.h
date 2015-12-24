/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    irisphysxtoolkit_lambertianmaterial.h

Abstract:

    This file contains the definitions for the Lambetian material types.

--*/

#ifndef _LAMBERTIAN_MATERIAL_IRIS_PHYSX_TOOLKIT_
#define _LAMBERTIAN_MATERIAL_IRIS_PHYSX_TOOLKIT_

#include <irisphysxtoolkit.h>

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISPHYSXTOOLKITAPI
ISTATUS
LambertianMaterialAllocate(
    _In_ PREFLECTOR Reflectance,
    _Out_ PMATERIAL *Material
    );

#endif // _LAMBERTIAN_MATERIAL_IRIS_PHYSX_TOOLKIT_