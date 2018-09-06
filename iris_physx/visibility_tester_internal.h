/*++

Copyright (c) 2018 Brad Weinberger

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
// Functions
//

_Check_return_
_Success_(return != 0)
static
inline
bool
VisibilityTesterInitialize(
    _Out_ struct _VISIBILITY_TESTER *visibility_tester,
    _In_ PRAY_TRACER_TRACE_ROUTINE trace_routine,
    _In_opt_ const void *trace_context,
    _In_ float_t epsilon
    )
{
    assert(visibility_tester != NULL);
    assert(trace_routine != NULL);
    assert(isfinite(epsilon) && epsilon >= (float_t)0.0);

    ISTATUS status = RayTracerAllocate(&visibility_tester->ray_tracer);

    if (status != ISTATUS_SUCCESS)
    {
        return false;
    }

    visibility_tester->trace_routine = trace_routine;
    visibility_tester->trace_context = trace_context;
    visibility_tester->epsilon = epsilon;

    return true;
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