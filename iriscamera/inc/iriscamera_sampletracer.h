/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    iriscamera_sampletracer.h

Abstract:

    This file contains the definitions for the SAMPLE_TRACER type.

--*/

#ifndef _SAMPLE_TRACER_IRIS_CAMERA_
#define _SAMPLE_TRACER_IRIS_CAMERA_

#include <iriscamera.h>

//
// Types
//

typedef
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
(*PSAMPLE_TRACER_TRACE_ROUTINE)(
    _In_ PVOID Context,
    _In_ RAY WorldRay,
    _In_ PRANDOM Rng,
    _Out_ PCOLOR3 Color
    );

typedef struct _SAMPLE_TRACER_VTABLE {
    PSAMPLE_TRACER_TRACE_ROUTINE TraceRoutine;
    PFREE_ROUTINE FreeRoutine;
} SAMPLE_TRACER_VTABLE, *PSAMPLE_TRACER_VTABLE;

typedef CONST SAMPLE_TRACER_VTABLE *PCSAMPLE_TRACER_VTABLE;

typedef struct _SAMPLE_TRACER SAMPLE_TRACER, *PSAMPLE_TRACER;
typedef CONST SAMPLE_TRACER *PCSAMPLE_TRACER;

//
// Functions
//

_Success_(return == ISTATUS_SUCCESS)
IRISCAMERAAPI
ISTATUS
SampleTracerTrace(
    _In_ PSAMPLE_TRACER SampleTracer,
    _In_ RAY WorldRay,
    _In_ PRANDOM Rng,
    _Out_ PCOLOR3 Color
    );

#endif // _SAMPLE_TRACER_IRIS_CAMERA_