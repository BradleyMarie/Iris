/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    iris_hittester.h

Abstract:

    Intersection tester.

--*/

#ifndef _IRIS_HIT_TESTER_INTERNAL_
#define _IRIS_HIT_TESTER_INTERNAL_

#include "common/const_pointer_list.h"
#include "iris/hit_allocator_internal.h"
#include "iris/shared_hit_context_allocator.h"

//
// Types
//

struct _HIT_TESTER {
    struct _HIT_ALLOCATOR hit_allocator;
    SHARED_HIT_CONTEXT_ALLOCATOR context_allocator;
    CONSTANT_POINTER_LIST hit_list;
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

    bool success =
        SharedHitContextAllocatorInitialize(&hit_tester->context_allocator);
    
    if (!success)
    {
        return false;
    }

    success = ConstantPointerListInitialize(&hit_tester->hit_list);

    if (!success)
    {
        SharedHitContextAllocatorDestroy(&hit_tester->context_allocator);
        return false;
    }

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
    SharedHitContextAllocatorFreeAll(&hit_tester->context_allocator);
    ConstantPointerListClear(&hit_tester->hit_list);

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
    SharedHitContextAllocatorDestroy(&hit_tester->context_allocator);
    ConstantPointerListDestroy(&hit_tester->hit_list);
}

#endif // _IRIS_HIT_TESTER_INTERNAL_