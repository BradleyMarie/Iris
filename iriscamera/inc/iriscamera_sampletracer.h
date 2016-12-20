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
    _In_ PCVOID Context,
    _In_ RAY WorldRay,
    _In_ PRANDOM_REFERENCE Rng,
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

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISCAMERAAPI
ISTATUS
SampleTracerAllocate(
    _In_ PCSAMPLE_TRACER_VTABLE SampleTracerVTable,
    _When_(DataSizeInBytes != 0, _In_reads_bytes_opt_(DataSizeInBytes)) PCVOID Data,
    _In_ SIZE_T DataSizeInBytes,
    _When_(DataSizeInBytes != 0, _Pre_satisfies_(_Curr_ != 0 && (_Curr_ & (_Curr_ - 1)) == 0 && DataSizeInBytes % _Curr_ == 0)) SIZE_T DataAlignment,
    _Out_ PSAMPLE_TRACER *SampleTracer
    );

_Success_(return == ISTATUS_SUCCESS)
IRISCAMERAAPI
ISTATUS
SampleTracerTrace(
    _In_ PCSAMPLE_TRACER SampleTracer,
    _In_ RAY WorldRay,
    _In_ PRANDOM_REFERENCE Rng,
    _Out_ PCOLOR3 Color
    );

IRISCAMERAAPI
VOID
SampleTracerFree(
    _In_opt_ _Post_invalid_ PSAMPLE_TRACER SampleTracer
    );

#endif // _SAMPLE_TRACER_IRIS_CAMERA_