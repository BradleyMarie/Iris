/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    iriscamera_sampletracergenerator.h

Abstract:

    This file contains the internal definitions for the SAMPLE_TRACER_GENERATOR type.

--*/

#ifndef _SAMPLE_TRACER_GENERATOR_IRIS_CAMERA_INTERNAL_
#define _SAMPLE_TRACER_GENERATOR_IRIS_CAMERA_INTERNAL_

#include <iriscamera.h>

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
SampleTracerGeneratorGenerateSampleTracer(
    _In_ PCSAMPLE_TRACER_GENERATOR SampleTracerGenerator,
    _In_ PSAMPLE_TRACER_ALLOCATOR SampleTracerAllocator,
    _Out_ PSAMPLE_TRACER *SampleTracer 
    );

#endif // _SAMPLE_TRACER_GENERATOR_IRIS_CAMERA_INTERNAL_