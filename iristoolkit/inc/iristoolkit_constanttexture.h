/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    iristoolkit_constanttexture.h

Abstract:

    This file contains the prototypes for the CONSTANT_TEXTURE type.

--*/

#ifndef _CONSTANT_TEXTURE_IRIS_TOOLKIT_
#define _CONSTANT_TEXTURE_IRIS_TOOLKIT_

#include <iristoolkit.h>

//
// Prototypes
//

_Check_return_
_Ret_maybenull_
IRISTOOLKITAPI
PTEXTURE
ConstantTextureAllocate(
    _In_opt_ PCEMISSIVE_SHADER EmissiveShader,
    _In_opt_ PCDIRECT_SHADER DirectShader,
    _In_opt_ PCINDIRECT_SHADER IndirectShader,
    _In_opt_ PCTRANSLUCENT_SHADER TranslucentShader
    );

#endif // _CONSTANT_TEXTURE_IRIS_TOOLKIT_