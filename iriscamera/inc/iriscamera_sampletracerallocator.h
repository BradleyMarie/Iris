/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    iriscamera_sampletracerallocator.h

Abstract:

    This file contains the definitions for the SAMPLE_TRACER_ALLOCATOR type.

--*/

#ifndef _SAMPLE_TRACER_ALLOCATOR_IRIS_CAMERA_
#define _SAMPLE_TRACER_ALLOCATOR_IRIS_CAMERA_

#include <iriscamera.h>

//
// Types
//

typedef struct _SAMPLE_TRACER_ALLOCATOR SAMPLE_TRACER_ALLOCATOR, *PSAMPLE_TRACER_ALLOCATOR;
typedef CONST SAMPLE_TRACER_ALLOCATOR *PCSAMPLE_TRACER_ALLOCATOR;

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISCAMERAAPI
ISTATUS
SampleTracerAllocatorAllocate(
    _Inout_ PSAMPLE_TRACER_ALLOCATOR SampleTracerAllocator,
    _In_ PCSAMPLE_TRACER_VTABLE SampleTracerVTable,
    _When_(DataSizeInBytes != 0, _In_reads_bytes_opt_(DataSizeInBytes)) PCVOID Data,
    _In_ SIZE_T DataSizeInBytes,
    _When_(DataSizeInBytes != 0, _Pre_satisfies_(_Curr_ != 0 && (_Curr_ & (_Curr_ - 1)) == 0 && DataSizeInBytes % _Curr_ == 0)) SIZE_T DataAlignment,
    _Out_ PSAMPLE_TRACER *SampleTracer
    );

#endif // _SAMPLE_TRACER_ALLOCATOR_IRIS_CAMERA_