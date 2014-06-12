/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    checkerboard.h

Abstract:

    This file contains the prototypes for the CHECKERBOARD_TEXTURE type.

--*/

#ifndef _CHECKERBOARD_TEXTURE_IRIS_TOOLKIT_
#define _CHECKERBOARD_TEXTURE_IRIS_TOOLKIT_

#include <iristoolkit.h>

//
// Prototypes
//

_Check_return_
_Ret_maybenull_
PTEXTURE
XZCheckerboardTextureAllocate(
    _In_opt_ PCEMISSIVE_SHADER EmissiveShader0,
    _In_opt_ PCDIRECT_SHADER DirectShader0,
    _In_opt_ PCINDIRECT_SHADER IndirectShader0,
    _In_opt_ PTRANSLUCENT_SHADER TranslucentShader0,
    _In_opt_ PCEMISSIVE_SHADER EmissiveShader1,
    _In_opt_ PCDIRECT_SHADER DirectShader1,
    _In_opt_ PCINDIRECT_SHADER IndirectShader1,
    _In_opt_ PTRANSLUCENT_SHADER TranslucentShader1
    );

#endif // _CHECKERBOARD_TEXTURE_IRIS_TOOLKIT_