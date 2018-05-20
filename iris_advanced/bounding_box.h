/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    bounding_box.h

Abstract:

    An axis-aligned bounding box.

--*/

#ifndef _IRIS_ADVANCED_BOUNDING_BOX_
#define _IRIS_ADVANCED_BOUNDING_BOX_

#include "iris/iris.h"

//
// Types
//

typedef struct _BOUNDING_BOX {
    POINT3 corners[2];
} BOUNDING_BOX, *PBOUNDING_BOX;

typedef const BOUNDING_BOX *PCBOUNDING_BOX;

//
// Functions
//

static
inline
BOUNDING_BOX
BoundingBoxCreate(
    _In_ POINT3 corner0,
    _In_ POINT3 corner1
    )
{
    assert(PointValidate(corner0));
    assert(PointValidate(corner1));

    BOUNDING_BOX result;
    result.corners[0] = corner0;
    result.corners[1] = corner1;

    return result;
}

static
inline
bool
BoundingBoxValidate(
    _In_ BOUNDING_BOX box
    )
{
    if (!PointValidate(box.corners[0]) ||
        !PointValidate(box.corners[1]))
    {
        return false;
    }
    
    return true;
}

#endif // _IRIS_ADVANCED_BOUNDING_BOX_