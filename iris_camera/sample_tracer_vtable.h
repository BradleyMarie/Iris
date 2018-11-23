/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    sample_tracer_vtable.h

Abstract:

    The vtable for a sample tracer.

--*/

#ifndef _IRIS_CAMERA_SAMPLE_TRACER_VTABLE_
#define _IRIS_CAMERA_SAMPLE_TRACER_VTABLE_

#include "common/free_routine.h"
#include "iris_advanced/iris_advanced.h"

typedef
ISTATUS
(*PSAMPLE_TRACER_TRACE_ROUTINE)(
    _In_opt_ void *context,
    _In_ PCRAY ray
    );

typedef
ISTATUS
(*PSAMPLE_TRACER_TONE_MAP_ROUTINE)(
    _In_opt_ void *context,
    _Out_ PCOLOR3 color
    );

typedef struct _SAMPLE_TRACER_VTABLE {
    PSAMPLE_TRACER_TRACE_ROUTINE trace_routine;
    PSAMPLE_TRACER_TONE_MAP_ROUTINE tone_map_routine;
    PFREE_ROUTINE free_routine;
} SAMPLE_TRACER_VTABLE, *PSAMPLE_TRACER_VTABLE;

typedef const SAMPLE_TRACER_VTABLE *PCSAMPLE_TRACER_VTABLE;

#endif // _IRIS_CAMERA_SAMPLE_TRACER_VTABLE_