/*++

Copyright (c) 2018 Brad Weinberger

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

inline
static
ISTATUS
SampleTracerTrace(
    _In_ struct _SAMPLE_TRACER *tracer,
    _In_ PCRAY ray,
    _In_ PRANDOM rng,
    _In_ float_t epsilon
    )
{
    assert(tracer != NULL);
    assert(ray != NULL);
    assert(RayValidate(*ray));
    assert(rng != NULL);
    assert(isfinite(epsilon) && (float_t)0.0 <= epsilon);

    ISTATUS status = tracer->vtable->trace_routine(tracer->data,
                                                   ray,
                                                   rng,
                                                   epsilon);

    return status;
}

inline
static
ISTATUS
SampleTracerToneMap(
    _In_ struct _SAMPLE_TRACER *tracer,
    _Out_ PCOLOR3 color
    )
{
    assert(tracer != NULL);
    assert(color != NULL);

    ISTATUS status = tracer->vtable->tone_map_routine(tracer->data, color);

    return status;
}

#endif // _IRIS_CAMERA_SAMPLE_TRACER_INTERNAL_