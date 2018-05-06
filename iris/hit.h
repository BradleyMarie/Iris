/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    hit.h

Abstract:

    Hit data generated during a successful intersection.

--*/

#ifndef _IRIS_HIT_
#define _IRIS_HIT_

#include "iris/sal.h"

#include <float.h>
#include <stdint.h>

//
// Types
//

typedef struct _HIT {
    const void *data;
    float_t distance;
    uint32_t front_face;
    uint32_t back_face;
    _Field_size_bytes_opt_(additional_data_size) const void *additional_data;
    size_t additional_data_size;
} HIT, *PHIT;

typedef CONST HIT *PCHIT;

typedef struct _HIT_LIST {
    struct _HIT_LIST *next_hit;
    PCHIT hit;
} HIT_LIST, *PHIT_LIST;

typedef CONST HIT_LIST *PCHIT_LIST;

#endif // _IRIS_HIT_