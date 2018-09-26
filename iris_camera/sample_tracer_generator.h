/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    sample_tracer_generator.h

Abstract:

    Generates a sample tracer.

--*/

#ifndef _IRIS_CAMERA_SAMPLE_TRACER_GENERATOR_
#define _IRIS_CAMERA_SAMPLE_TRACER_GENERATOR_

#include "iris_camera/sample_tracer_generator_vtable.h"

//
// Types
//

typedef struct _SAMPLE_TRACER_GENERATOR SAMPLE_TRACER_GENERATOR;
typedef SAMPLE_TRACER_GENERATOR *PSAMPLE_TRACER_GENERATOR;
typedef const SAMPLE_TRACER_GENERATOR *PCSAMPLE_TRACER_GENERATOR;

//
// Functions
//

ISTATUS
SampleTracerGeneratorAllocate(
    _In_ PCSAMPLE_TRACER_GENERATOR_VTABLE vtable,
    _In_reads_bytes_opt_(data_size) const void *data,
    _In_ size_t data_size,
    _In_ size_t data_alignment,
    _Out_ PSAMPLE_TRACER_GENERATOR *sample_tracer_generator
    );

void
SampleTracerGeneratorFree(
    _In_opt_ _Post_invalid_ PSAMPLE_TRACER_GENERATOR sample_tracer_generator
    );

#endif // _IRIS_CAMERA_SAMPLE_TRACER_GENERATOR_