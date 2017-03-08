/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    iriscamera_sampletracer.h

Abstract:

    This file contains the internal definitions for the SAMPLE_TRACER type.

--*/

#ifndef _SAMPLE_TRACER_IRIS_CAMERA_INTERNAL_
#define _SAMPLE_TRACER_IRIS_CAMERA_INTERNAL_

#include <iriscamera.h>

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
SampleTracerGenerateThreadStateAndCallback(
    _In_ PCSAMPLE_TRACER SampleTracer,
    _In_ PRANDOM Rng,
    _Inout_ PSAMPLE_TRACER_CALLBACK Callback
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
SampleTracerTrace(
    _In_ PCSAMPLE_TRACER SampleTracer,
    _In_opt_ PCVOID ThreadState,
    _In_ RAY WorldRay,
    _In_ PRANDOM Rng,
    _Out_ PCOLOR3 Color
    );

#endif // _SAMPLE_TRACER_IRIS_CAMERA_INTERNAL_