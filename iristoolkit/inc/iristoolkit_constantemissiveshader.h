/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    iristoolkit_constantemissiveshader.h

Abstract:

    This file contains the prototypes for the CONSTANT_EMISSIVE_SHADER type.

--*/

#ifndef _CONSTANT_EMISSIVE_SHADER_IRIS_TOOLKIT_
#define _CONSTANT_EMISSIVE_SHADER_IRIS_TOOLKIT_

#include <iristoolkit.h>

//
// Prototypes
//

_Check_return_
_Ret_maybenull_
IRISTOOLKITAPI
PEMISSIVE_SHADER
ConstantEmissiveShaderAllocate(
    _In_ PCOLOR3 Color
    );

#endif // _CONSTANT_EMISSIVE_SHADER_IRIS_TOOLKIT_