/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    iriscamera_sampletracerallocator.h

Abstract:

    This file contains the internal definitions for the SAMPLE_TRACER_ALLOCATOR type.

--*/

#ifndef _SAMPLE_TRACER_ALLOCATOR_IRIS_CAMERA_INTERNAL_
#define _SAMPLE_TRACER_ALLOCATOR_IRIS_CAMERA_INTERNAL_

#include <iriscamera.h>

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
SampleTracerAllocatorCreate(
    _Out_ PSAMPLE_TRACER_ALLOCATOR *SampleTracerAllocator
    );

VOID
SampleTracerAllocatorClear(
    _Inout_ PSAMPLE_TRACER_ALLOCATOR SampleTracerAllocator
    );

VOID
SampleTracerAllocatorFree(
    _In_opt_ _Post_invalid_ PSAMPLE_TRACER_ALLOCATOR SampleTracerAllocator
    );

#endif // _SAMPLE_TRACER_ALLOCATOR_IRIS_CAMERA_INTERNAL_