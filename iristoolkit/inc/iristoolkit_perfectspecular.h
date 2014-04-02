/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    iristoolkit_perfectspecular.h

Abstract:

    This file contains the prototypes for the 
    PERFECT_SPECULAR_INDIRECT_SHADER type.

--*/

#ifndef _PERFECT_SPECULAR_INDIRECT_SHADER_IRIS_TOOLKIT_
#define _PERFECT_SPECULAR_INDIRECT_SHADER_IRIS_TOOLKIT_

#include <iristoolkit.h>

//
// Prototypes
//

_Check_return_
_Ret_maybenull_
IRISTOOLKITAPI
PINDIRECT_SHADER
PerfectSpecularIndirectShaderAllocate(
    _In_ PCOLOR3 Reflectance
    );

#endif // _PERFECT_SPECULAR_INDIRECT_SHADER_IRIS_TOOLKIT_