/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    sample_tracer.h

Abstract:

    Traces the sampled ray and generates a color.

--*/

#ifndef _IRIS_CAMERA_SAMPLE_TRACER_
#define _IRIS_CAMERA_SAMPLE_TRACER_

#include "iris_camera/sample_tracer_vtable.h"

//
// Types
//

typedef struct _SAMPLE_TRACER SAMPLE_TRACER, *PSAMPLE_TRACER;
typedef const SAMPLE_TRACER *PCSAMPLE_TRACER;

//
// Functions
//

ISTATUS
SampleTracerAllocate(
    _In_ PSAMPLE_TRACER_TRACE_ROUTINE trace_routine,
    _In_ PSAMPLE_TRACER_TONE_MAP_ROUTINE tone_map_routine,
    _In_reads_bytes_opt_(data_size) const void *data,
    _In_ size_t data_size,
    _In_ size_t data_alignment,
    _Out_ PSAMPLE_TRACER *sample_tracer
    );

void
SampleTracerFree(
    _In_opt_ _Post_invalid_ PSAMPLE_TRACER sample_tracer
    );

#endif // _IRIS_CAMERA_PIXEL_SAMPLER_