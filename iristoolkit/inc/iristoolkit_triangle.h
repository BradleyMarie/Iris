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
// Macros
//

#define IRIS_TOOLKIT_TRIANGLE_VERTICES 3

//
// Types
//

typedef struct _BARYCENTRIC_COORDINATES {
    FLOAT Coordinates[IRIS_TOOLKIT_TRIANGLE_VERTICES];
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
    _In_opt_ PCTEXTURE FrontTexture,
    _In_opt_ PNORMAL FrontNormal,
    _In_opt_ PCTEXTURE BackTexture,
    _In_opt_ PNORMAL BackNormal
    );

_Check_return_
_Ret_maybenull_
_Success_(return != NULL)
IRISTOOLKITAPI
PDRAWING_SHAPE
FlatTriangleAllocate(
    _In_ PCPOINT3 Vertex0,
    _In_ PCPOINT3 Vertex1,
    _In_ PCPOINT3 Vertex2,
    _In_opt_ PCTEXTURE FrontTexture,
    _In_opt_ PCTEXTURE BackTexture,
    _Out_opt_ PNORMAL *FrontNormal,
    _Out_opt_ PNORMAL *BackNormal
    );

#endif // _TRIANGLE_IRIS_TOOLKIT_