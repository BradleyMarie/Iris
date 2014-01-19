/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    iristoolkit_triangle.h

Abstract:

    This file contains the prototypes for the TRIANGLE type.

--*/

#ifndef _TRIANGLE_IRIS_TOOLKIT_
#define _TRIANGLE_IRIS_TOOLKIT_

#include <iristoolkit.h>

//
// Types
//

typedef struct _BARYCENTRIC_COORDINATES {
    FLOAT B0;
    FLOAT B1;
    FLOAT B2;
} BARYCENTRIC_COORDINATES, *PBARYCENTRIC_COORDINATES;

typedef CONST BARYCENTRIC_COORDINATES *PCBARYCENTRIC_COORDINATES;

//
// Prototypes
//

_Check_return_
_Ret_maybenull_
IRISTOOLKITAPI
PDRAWING_SHAPE
TriangleAllocate(
    _In_ PCPOINT3 Vertex0,
    _In_ PCPOINT3 Vertex1,
    _In_ PCPOINT3 Vertex2,
    _In_opt_ PCSHADER FrontShader,
    _In_opt_ PCNORMAL FrontNormal,
    _In_opt_ PCSHADER BackShader,
    _In_opt_ PCNORMAL BackNormal,
    _Out_opt_ PVECTOR3 FrontFaceSurfaceNormal
    );

#endif // _TRIANGLE_IRIS_TOOLKIT_