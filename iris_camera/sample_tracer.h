/*++

Copyright (c) 2019 Brad Weinberger

Module Name:

    sample_tracer.h

Abstract:

    Traces the sampled ray and generates a color.

--*/

#ifndef _IRIS_CAMERA_SAMPLE_TRACER_
#define _IRIS_CAMERA_SAMPLE_TRACER_

#include "iris_camera/sample_tracer_vtable.h"

//
// Functions
//

ISTATUS
SampleTracerAllocate(
    _In_ PCSAMPLE_TRACER_VTABLE vtable,
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