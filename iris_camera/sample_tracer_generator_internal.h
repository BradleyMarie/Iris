/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    sample_tracer_generator_internal.h

Abstract:

    The internal routines for a sample tracer generator.

--*/

#ifndef _IRIS_CAMERA_SAMPLE_TRACER_GENERATOR_INTERNAL_
#define _IRIS_CAMERA_SAMPLE_TRACER_GENERATOR_INTERNAL_

#include "iris_camera/sample_tracer_generator_vtable.h"

//
// Types
//

struct _SAMPLE_TRACER_GENERATOR {
    PCSAMPLE_TRACER_GENERATOR_VTABLE vtable;
    void *data;
};

//
// Functions
//

ISTATUS
SampleTracerGeneratorGenerate(
    _In_ const struct _SAMPLE_TRACER_GENERATOR *sample_tracer_generator,
    _Out_ PSAMPLE_TRACER *sample_tracer
    )
{
    assert(sample_tracer_generator != NULL);
    assert(sample_tracer != NULL);

    ISTATUS status = sample_tracer_generator->vtable->generate_routine(
            sample_tracer_generator->data, sample_tracer);

    return status;
}

#endif // _IRIS_CAMERA_SAMPLE_TRACER_GENERATOR_INTERNAL_