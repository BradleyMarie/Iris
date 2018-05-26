/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    hit_context.h

Abstract:

    Full context of a hit generated during a successful intersection.

--*/

#ifndef _IRIS_HIT_CONTEXT_
#define _IRIS_HIT_CONTEXT_

#include "common/sal.h"

#include <stdint.h>
#include <stddef.h>

#if __cplusplus 
#include <math.h>
#else
#include <tgmath.h>
#endif // __cplusplus

//
// Types
//

typedef struct _HIT_CONTEXT {
    const void *data;
    float_t distance;
    uint32_t front_face;
    uint32_t back_face;
    _Field_size_bytes_opt_(additional_data_size) const void *additional_data;
    size_t additional_data_size;
} HIT_CONTEXT, *PHIT_CONTEXT;

typedef const HIT_CONTEXT *PCHIT_CONTEXT;

#endif // _IRIS_HIT_CONTEXT_