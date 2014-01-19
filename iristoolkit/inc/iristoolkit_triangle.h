/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    iristoolkit_triangle.h

Abstract:

    This file contains the definitions for the TRIANGLE type.

--*/

#ifndef _TRIANGLE_IRIS_TOOLKIT_
#define _TRIANGLE_IRIS_TOOLKIT_

#include <iristoolkit.h>

_Check_return_
_Ret_maybenull_
IRISTOOLKITAPI
PDRAWING_SHAPE
TriangleAllocate(
    _In_ PCPOINT Vertex0,
    _In_ PCPOINT Vertex1,
    _In_ PCPOINT Vertex2,
    _In_opt_ PCSHADER *FrontShader,
    _In_opt_ PCNORMAL *FrontNormal,
    _In_opt_ PCSHADER *BackShader,
    _In_opt_ PCNORMAL *BackNormal
    );

#endif // _TRIANGLE_IRIS_TOOLKIT_