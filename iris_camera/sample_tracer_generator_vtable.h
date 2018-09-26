/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    sample_tracer_generator_vtable.h

Abstract:

    The vtable for a sample tracer generator.

--*/

#ifndef _IRIS_CAMERA_SAMPLE_TRACER_GENERATOR_VTABLE_
#define _IRIS_CAMERA_SAMPLE_TRACER_GENERATOR_VTABLE_

#include "iris_camera/sample_tracer.h"

typedef
ISTATUS
(*PSAMPLE_TRACER_GENERATOR_GENERATE_ROUTINE)(
    _In_ const void *context,
    _Out_ PSAMPLE_TRACER *sample_tracer
    );

typedef struct _SAMPLE_TRACER_GENERATOR_VTABLE {
    PSAMPLE_TRACER_GENERATOR_GENERATE_ROUTINE generate_routine;
    PFREE_ROUTINE free_routine;
} SAMPLE_TRACER_GENERATOR_VTABLE, *PSAMPLE_TRACER_GENERATOR_VTABLE;

typedef const SAMPLE_TRACER_GENERATOR_VTABLE *PCSAMPLE_TRACER_GENERATOR_VTABLE;

#endif // _IRIS_CAMERA_SAMPLE_TRACER_GENERATOR_VTABLE_