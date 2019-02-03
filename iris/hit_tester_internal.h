/*++

Copyright (c) 2019 Brad Weinberger

Module Name:

    hit_tester_internal.h

Abstract:

    Intersection tester.

--*/

#ifndef _IRIS_HIT_TESTER_INTERNAL_
#define _IRIS_HIT_TESTER_INTERNAL_

#include "iris/full_hit_context.h"
#include "iris/hit_allocator_internal.h"

//
// Types
//

struct _HIT_TESTER {
    struct _HIT_ALLOCATOR hit_allocator;
    PCFULL_HIT_CONTEXT closest_hit;
    RAY world_ray;
    float_t minimum_distance;
};

//
// Functions
//

_Check_return_
_Success_(return != 0)
static
inline
bool
HitTesterInitialize(
    _Out_ struct _HIT_TESTER *hit_tester
    )
{
    assert(hit_tester != NULL);

    HitAllocatorInitialize(&hit_tester->hit_allocator);

    hit_tester->minimum_distance = (float_t)0.0;

    return true;
}

static
inline
void
HitTesterReset(
    _Inout_ struct _HIT_TESTER *hit_tester,
    _In_ RAY world_ray,
    _In_ float_t minimum_distance
    )
{
    assert(hit_tester != NULL);
    assert(RayValidate(world_ray));
    assert(isfinite(minimum_distance));
    assert((float_t)0.0 <= minimum_distance);

    HitAllocatorFreeAll(&hit_tester->hit_allocator);

    hit_tester->closest_hit = NULL;
    hit_tester->world_ray = world_ray;
    hit_tester->minimum_distance = minimum_distance;
}

static
inline
void
HitTesterDestroy(
    _In_ _Post_invalid_ struct _HIT_TESTER *hit_tester
    )
{
    assert(hit_tester != NULL);

    HitAllocatorDestroy(&hit_tester->hit_allocator);
}

#endif // _IRIS_HIT_TESTER_INTERNAL_