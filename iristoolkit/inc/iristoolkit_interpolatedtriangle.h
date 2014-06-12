/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    iristoolkit_interpolatedtriangle.h

Abstract:

    This file contains the prototypes for the INTERPOLATED_TRIANGLE types.

--*/

#ifndef _INTERPOLATED_TRIANGLE_IRIS_TOOLKIT_
#define _INTERPOLATED_TRIANGLE_IRIS_TOOLKIT_

#include <iristoolkit.h>

_Check_return_
_Ret_maybenull_
IRISTOOLKITAPI
PEMISSIVE_SHADER
InterpolatedTriangleEmissiveShaderAllocate(
    _In_ PCEMISSIVE_SHADER Shader0,
    _In_ PCEMISSIVE_SHADER Shader1,
    _In_ PCEMISSIVE_SHADER Shader2
    );

_Check_return_
_Ret_maybenull_
IRISTOOLKITAPI
PDIRECT_SHADER
InterpolatedTriangleDirectShaderAllocate(
    _In_ PCDIRECT_SHADER Shader0,
    _In_ PCDIRECT_SHADER Shader1,
    _In_ PCDIRECT_SHADER Shader2
    );

_Check_return_
_Ret_maybenull_
IRISTOOLKITAPI
PINDIRECT_SHADER
InterpolatedTriangleIndirectShaderAllocate(
    _In_ PCINDIRECT_SHADER Shader0,
    _In_ PCINDIRECT_SHADER Shader1,
    _In_ PCINDIRECT_SHADER Shader2
    );

_Check_return_
_Ret_maybenull_
IRISTOOLKITAPI
PTRANSLUCENT_SHADER
InterpolatedTriangleTranslucentShaderAllocate(
    _In_ PTRANSLUCENT_SHADER Shader0,
    _In_ PTRANSLUCENT_SHADER Shader1,
    _In_ PTRANSLUCENT_SHADER Shader2
    );

_Check_return_
_Ret_maybenull_
IRISTOOLKITAPI
PNORMAL
InterpolatedTriangleNormalAllocate(
    _In_ PCNORMAL Normal0,
    _In_ PCNORMAL Normal1,
    _In_ PCNORMAL Normal2
    );

#endif // _INTERPOLATED_TRIANGLE_IRIS_TOOLKIT_