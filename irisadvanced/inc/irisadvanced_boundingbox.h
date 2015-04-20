/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    irisadvanced_boundingbox.h

Abstract:

    This file contains the definitions for the BOUNDING_BOX type.

--*/

#ifndef _BOUNDING_BOX_IRIS_ADVANCED_
#define _BOUNDING_BOX_IRIS_ADVANCED_

#include <irisadvanced.h>

//
// Types
//

typedef struct _BOUNDING_BOX {
    POINT3 Corners[2];
} BOUNDING_BOX, *PBOUNDING_BOX;

typedef CONST BOUNDING_BOX *PCBOUNDING_BOX;

//
// Functions
//

SFORCEINLINE
BOUNDING_BOX
BoundingBoxCreate(
    _In_ POINT3 Corner0,
    _In_ POINT3 Corner1
    )
{
    BOUNDING_BOX Output;

    Output.Corners[0] = Corner0;
    Output.Corners[1] = Corner1;

    return Output;
}

SFORCEINLINE
BOOL
BoundingBoxContainsPoint(
    _In_ BOUNDING_BOX BoundingBox,
    _In_ POINT3 Point
    )
{
    if (BoundingBox.Corners[0].X <= Point.X &&
        Point.X <= BoundingBox.Corners[1].X &&
        BoundingBox.Corners[0].Y <= Point.Y &&
        Point.Y <= BoundingBox.Corners[1].Y &&
        BoundingBox.Corners[0].Z <= Point.Z &&
        Point.Z <= BoundingBox.Corners[1].Z)
    {
        return TRUE;
    }

    return FALSE;
}

#endif // _BOUNDING_BOX_IRIS_ADVANCED_