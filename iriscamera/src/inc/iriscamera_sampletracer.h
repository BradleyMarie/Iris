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
SampleTracerAllocate(
    _In_ PCSAMPLE_TRACER_VTABLE SampleTracerVTable,
    _When_(DataSizeInBytes != 0, _In_reads_bytes_opt_(DataSizeInBytes)) PCVOID Data,
    _In_ SIZE_T DataSizeInBytes,
    _When_(DataSizeInBytes != 0, _Pre_satisfies_(_Curr_ != 0 && (_Curr_ & (_Curr_ - 1)) == 0 && DataSizeInBytes % _Curr_ == 0)) SIZE_T DataAlignment,
    _Out_ PSAMPLE_TRACER *SampleTracer
    );

VOID
SampleTracerFree(
    _In_opt_ _Post_invalid_ PSAMPLE_TRACER SampleTracer
    );

#endif // _SAMPLE_TRACER_IRIS_CAMERA_INTERNAL_