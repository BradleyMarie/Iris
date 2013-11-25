/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    iris_shapehit.h

Abstract:

    This module implements the SHAPE_HIT_LIST internal functions.

--*/

#ifndef _IRIS_SHAPE_HIT_LIST_INTERNAL_
#define _IRIS_SHAPE_HIT_LIST_INTERNAL_

#include <irisp.h>

//
// Types
//

struct _SHAPE_HIT_LIST {
    _Field_size_opt_(ListCapacity) PSHAPE_HIT List;
    SIZE_T HitsInitialized;
    SIZE_T ListCapacity;
    SIZE_T ListSize;
};

//
// Functions
//

SFORCEINLINE
VOID
ShapeHitListInitialize(
    _Out_ PSHAPE_HIT_LIST ShapeHitList
    )
{
    ShapeHitList->List = NULL;
    ShapeHitList->ListCapacity = 0;
    ShapeHitList->HitsInitialized = 0;
    ShapeHitList->ListSize = 0;
}

SFORCEINLINE
VOID
ShapeHitListDestroy(
    _Inout_ PSHAPE_HIT_LIST ShapeHitList
    )
{
    SIZE_T Index;

	ASSERT(ShapeHitList != NULL);

    for (Index = 0; Index < ShapeHitList->HitsInitialized; Index++)
    {
        ShapeHitDestroy(ShapeHitList->List + Index);
    }

    free(ShapeHitList->List);

    ShapeHitList->List = NULL;
    ShapeHitList->HitsInitialized = 0;
    ShapeHitList->ListCapacity = 0;
    ShapeHitList->ListSize = 0;
}

SFORCEINLINE
VOID
ShapeHitListClear(
    _Inout_ PSHAPE_HIT_LIST ShapeHitList
    )
{
    ASSERT(ShapeHitList != NULL);

    ShapeHitList->ListSize = 0;
}

#endif // _IRIS_SHAPE_HIT_LIST_INTERNAL_