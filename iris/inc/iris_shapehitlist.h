/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    iris_shapehit.h

Abstract:

    This module implements the SHAPE_HIT_LIST public functions.

--*/

#ifndef _IRIS_SHAPE_HIT_LIST_
#define _IRIS_SHAPE_HIT_LIST_

#include <iris.h>

typedef struct _SHAPE_HIT_LIST SHAPE_HIT_LIST, *PSHAPE_HIT_LIST;

_Success_(return == ISTATUS_SUCCESS)
ISTATUS
ShapeHitListAdd(
    _Inout_ PSHAPE_HIT_LIST ShapeHitList,
    _In_ FLOAT Distance,
    _In_ INT32 FaceHit,
    _In_ SIZE_T AdditionalDataSize,
    _Outptr_opt_result_bytebuffer_(AdditionalDataSize) PVOID *AdditionalData
    );

#endif // _IRIS_SHAPE_HIT_LIST_