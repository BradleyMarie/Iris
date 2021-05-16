/*++

Copyright (c) 2021 Brad Weinberger

Module Name:

    visibility_tester_internal.h

Abstract:

    Internal headers for visibility_tester interface.

--*/

#ifndef _IRIS_PHYSX_VISIBILITY_TESTER_INTERNAL_
#define _IRIS_PHYSX_VISIBILITY_TESTER_INTERNAL_

#include "iris/iris.h"

//
// Types
//

struct _VISIBILITY_TESTER {
    PRAY_TRACER ray_tracer;
    PRAY_TRACER_TRACE_ROUTINE trace_routine;
    const void *trace_context;
    float_t epsilon;
};

//
// Extern Functions
//

extern
ISTATUS
VisibilityTesterProcessHit(
    _Inout_opt_ void *context,
    _In_ PCHIT_CONTEXT hit_context
    );

//
// Functions
//

_Check_return_
_Success_(return != 0)
static
inline
bool
VisibilityTesterInitialize(
    _Out_ struct _VISIBILITY_TESTER *visibility_tester
    )
{
    assert(visibility_tester != NULL);

    ISTATUS status = RayTracerAllocate(&visibility_tester->ray_tracer);

    if (status != ISTATUS_SUCCESS)
    {
        return false;
    }

    visibility_tester->trace_routine = NULL;
    visibility_tester->trace_context = NULL;
    visibility_tester->epsilon = (float_t)0.0;

    return true;
}

static
inline
void
VisibilityTesterConfigure(
    _Inout_ struct _VISIBILITY_TESTER *visibility_tester,
    _In_ PRAY_TRACER_TRACE_ROUTINE trace_routine,
    _In_opt_ const void *trace_context,
    _In_ float_t epsilon
    )
{
    assert(visibility_tester != NULL);
    assert(trace_routine != NULL);
    assert(isfinite(epsilon) && epsilon >= (float_t)0.0);

    visibility_tester->trace_routine = trace_routine;
    visibility_tester->trace_context = trace_context;
    visibility_tester->epsilon = epsilon;
}

static
inline
ISTATUS
VisibilityTesterTestInline(
    _Inout_ PVISIBILITY_TESTER visibility_tester,
    _In_ RAY ray,
    _In_ float_t distance_to_object,
    _Out_ bool *visible
    )
{
    assert(visibility_tester != NULL);
    assert(RayValidate(ray));
    assert((float_t)0.0 <= distance_to_object);
    assert(visible != NULL);

    if (distance_to_object <= visibility_tester->epsilon)
    {
        return ISTATUS_SUCCESS;
    }

    float_t farthest_hit = distance_to_object - visibility_tester->epsilon;

    if (farthest_hit <= visibility_tester->epsilon)
    {
        return ISTATUS_SUCCESS;
    }

    *visible = true;
    ISTATUS status = RayTracerTraceClosestHit(visibility_tester->ray_tracer,
                                              ray,
                                              visibility_tester->epsilon,
                                              farthest_hit,
                                              visibility_tester->trace_routine,
                                              visibility_tester->trace_context,
                                              VisibilityTesterProcessHit,
                                              visible);

    return status;
}

static
inline
ISTATUS
VisibilityTesterTestAnyDistanceInline(
    _Inout_ PVISIBILITY_TESTER visibility_tester,
    _In_ RAY ray,
    _Out_ bool *visible
    )
{
    assert(visibility_tester != NULL);
    assert(RayValidate(ray));
    assert(visible != NULL);

    *visible = true;
    ISTATUS status = RayTracerTraceClosestHit(visibility_tester->ray_tracer,
                                              ray,
                                              visibility_tester->epsilon,
                                              INFINITY,
                                              visibility_tester->trace_routine,
                                              visibility_tester->trace_context,
                                              VisibilityTesterProcessHit,
                                              visible);

    return status;
}

static
inline
void
VisibilityTesterDestroy(
    _Inout_ struct _VISIBILITY_TESTER *visibility_tester
    )
{
    assert(visibility_tester != NULL);

    RayTracerFree(visibility_tester->ray_tracer);
}

#endif // _IRIS_PHYSX_VISIBILITY_TESTER_INTERNAL_