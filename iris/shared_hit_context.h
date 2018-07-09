/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    shared_hit_context.h

Abstract:

    Container for data which may be common to more than one hit.

--*/

#ifndef _IRIS_SHARED_HIT_CONTEXT_
#define _IRIS_SHARED_HIT_CONTEXT_

#include "iris/matrix.h"
#include "iris/ray.h"

//
// Types
//

typedef struct _SHARED_HIT_CONTEXT {
    PCMATRIX model_to_world;
    bool premultiplied;
} SHARED_HIT_CONTEXT, *PSHARED_HIT_CONTEXT;

typedef const SHARED_HIT_CONTEXT *PCSHARED_HIT_CONTEXT;

#endif // _IRIS_SHARED_HIT_CONTEXT_