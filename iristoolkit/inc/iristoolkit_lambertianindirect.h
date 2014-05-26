/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    iristoolkit_lambertianindirect.h

Abstract:

    This file contains the prototypes for the 
    LAMBERTIAN_INDIRECT_SHADER type.

--*/

#ifndef _LAMBERTIAN_INDIRECT_SHADER_IRIS_TOOLKIT_
#define _LAMBERTIAN_INDIRECT_SHADER_IRIS_TOOLKIT_

#include <iristoolkit.h>

//
// Prototypes
//

_Check_return_
_Ret_maybenull_
IRISTOOLKITAPI
PINDIRECT_SHADER
LambertianIndirectShaderAllocate(
    _In_ PCOLOR3 Reflectance
    );

#endif // _LAMBERTIAN_INDIRECT_SHADER_IRIS_TOOLKIT_