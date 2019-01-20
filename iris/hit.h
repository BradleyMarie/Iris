/*++

Copyright (c) 2019 Brad Weinberger

Module Name:

    hit.h

Abstract:

    A hit during intersection testing. Opaquely associated with the rest of the
    context of the hit.

--*/

#ifndef _IRIS_HIT_
#define _IRIS_HIT_

#if __cplusplus 
#include <math.h>
#else
#include <tgmath.h>
#endif // __cplusplus

//
// Types
//

typedef struct _HIT {
    struct _HIT *next;
    float_t distance;
} HIT, *PHIT;

typedef const HIT *PCHIT;

#endif // _IRIS_HIT_