/*++

Copyright (c) 2020 Brad Weinberger

Module Name:

    sample_tracer_vtable.h

Abstract:

    The vtable for a sample tracer.

--*/

#ifndef _IRIS_CAMERA_SAMPLE_TRACER_VTABLE_
#define _IRIS_CAMERA_SAMPLE_TRACER_VTABLE_

#include "common/free_routine.h"
#include "iris_advanced/iris_advanced.h"

//
// Types
//

typedef struct _SAMPLE_TRACER SAMPLE_TRACER, *PSAMPLE_TRACER;
typedef const SAMPLE_TRACER *PCSAMPLE_TRACER;

typedef
ISTATUS
(*PSAMPLE_TRACER_TRACE_ROUTINE)(
    _In_opt_ void *context,
    _In_ PCRAY_DIFFERENTIAL ray_differential,
    _In_ PRANDOM rng,
    _In_ float_t epsilon,
    _Out_ PCOLOR3 color
    );

typedef
ISTATUS
(*PSAMPLE_TRACER_DUPLICATE)(
    _In_opt_ const void *context,
    _Out_ PSAMPLE_TRACER *duplicate
    );

typedef struct _SAMPLE_TRACER_VTABLE {
    PSAMPLE_TRACER_TRACE_ROUTINE trace_routine;
    PSAMPLE_TRACER_DUPLICATE duplicate_routine;
    PFREE_ROUTINE free_routine;
} SAMPLE_TRACER_VTABLE, *PSAMPLE_TRACER_VTABLE;

typedef const SAMPLE_TRACER_VTABLE *PCSAMPLE_TRACER_VTABLE;

#endif // _IRIS_CAMERA_SAMPLE_TRACER_VTABLE_