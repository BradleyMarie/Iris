/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    sample_tracer.h

Abstract:

    Traces the sampled ray and generates a color.

--*/

#ifndef _IRIS_CAMERA_SAMPLE_TRACER_
#define _IRIS_CAMERA_SAMPLE_TRACER_

#include "iris_advanced/iris_advanced.h"

//
// Types
//

ISTATUS
(*PSAMPLE_TRACER_TRACE_ROUTINE)(
    _In_ const void *context,
    _In_ RAY ray,
    _Out_ PCOLOR3 color
    );

typedef struct _SAMPLE_TRACER SAMPLE_TRACER, *PSAMPLE_TRACER;
typedef const SAMPLE_TRACER *PCSAMPLE_TRACER;

typedef
ISTATUS
(*PSAMPLE_TRACER_LIFETIME_ROUTINE)(
    _Inout_opt_ void *context,
    _In_ PCSAMPLE_TRACER sample_tracer
    );

//
// Functions
//

ISTATUS
SampleTracerCreate(
    _In_ PSAMPLE_TRACER_TRACE_ROUTINE trace_routine,
    _In_ const void *sample_tracer_context,
    _In_ PSAMPLE_TRACER_LIFETIME_ROUTINE callback,
    _In_ void *callback_context
    );

#endif // _IRIS_CAMERA_PIXEL_SAMPLER_