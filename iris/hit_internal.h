/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    hit_internal.h

Abstract:

    Expanded internal represntation of a hit.

--*/

#ifndef _IRIS_HIT_INTERNAL_
#define _IRIS_HIT_INTERNAL_

#include "iris/hit.h"
#include "iris/shared_hit_context.h"

//
// Types
//

typedef struct _INTERNAL_HIT {
    HIT hit;
    PCSHARED_HIT_DATA shared_hit_context;
    POINT3 model_hit_point;
    bool model_hit_point_valid;
} INTERNAL_HIT, *PINTERNAL_HIT;

typedef CONST INTERNAL_HIT *PCINTERNAL_HIT;

#endif // _IRIS_HIT_INTERNAL_