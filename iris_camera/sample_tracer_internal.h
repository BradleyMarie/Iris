/*++

Copyright (c) 2020 Brad Weinberger

Module Name:

    sample_tracer_internal.h

Abstract:

    The internal routines for a sample tracer.

--*/

#ifndef _IRIS_CAMERA_SAMPLE_TRACER_INTERNAL_
#define _IRIS_CAMERA_SAMPLE_TRACER_INTERNAL_

#include "iris_camera/sample_tracer_vtable.h"

//
// Types
//

struct _SAMPLE_TRACER {
    PCSAMPLE_TRACER_VTABLE vtable;
    void *data;
};

//
// Functions
//

static
inline
ISTATUS
SampleTracerTrace(
    _In_ struct _SAMPLE_TRACER *tracer,
    _In_ PCRAY_DIFFERENTIAL ray_differential,
    _In_ PRANDOM rng,
    _In_ float_t epsilon,
    _Out_ PCOLOR3 color
    )
{
    assert(tracer != NULL);
    assert(ray_differential != NULL);
    assert(RayDifferentialValidate(*ray_differential));
    assert(rng != NULL);
    assert(isfinite(epsilon));
    assert((float_t)0.0 <= epsilon);
    assert(color != NULL);

    ISTATUS status = tracer->vtable->trace_routine(tracer->data,
                                                   ray_differential,
                                                   rng,
                                                   epsilon,
                                                   color);

    return status;
}

static
inline
ISTATUS
SampleTracerDuplicate(
    _In_ const struct _SAMPLE_TRACER *tracer,
    _Out_ struct _SAMPLE_TRACER **duplicate
    )
{
    assert(tracer != NULL);
    assert(duplicate != NULL);

    ISTATUS status = tracer->vtable->duplicate_routine(tracer->data, duplicate);

    return status;
}

#endif // _IRIS_CAMERA_SAMPLE_TRACER_INTERNAL_