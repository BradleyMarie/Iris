/*++

Copyright (c) 2021 Brad Weinberger

Module Name:

    full_hit_context.h

Abstract:

    A container for all of the data collected at a successful intersction.

--*/

#ifndef _IRIS_FULL_HIT_CONTEXT_
#define _IRIS_FULL_HIT_CONTEXT_

#include "common/dynamic_allocator.h"
#include "iris/hit.h"
#include "iris/hit_context.h"
#include "iris/point.h"
#include "iris/matrix.h"

//
// Types
//

typedef struct _FULL_HIT_CONTEXT {
    HIT hit;
    HIT_CONTEXT context;
    PDYNAMIC_ALLOCATION allocation_handle;
    PCMATRIX model_to_world;
    bool premultiplied;
    POINT3 model_hit_point;
    bool model_hit_point_valid;
} FULL_HIT_CONTEXT, *PFULL_HIT_CONTEXT;

typedef const FULL_HIT_CONTEXT *PCFULL_HIT_CONTEXT;

#endif // _IRIS_FULL_HIT_CONTEXT_