/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    iriscamera_sampletracergenerator.h

Abstract:

    This file contains the definitions for the SAMPLE_TRACER_GENERATOR type.

--*/

#ifndef _SAMPLE_TRACER_GENERATOR_IRIS_CAMERA_
#define _SAMPLE_TRACER_GENERATOR_IRIS_CAMERA_

#include <iriscamera.h>

//
// Types
//

typedef
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
(*PSAMPLE_TRACER_GENERATOR_GENERATOR_SAMPLE_TRACER_ROUTINE)(
    _In_ PCVOID Context,
    _Out_ PSAMPLE_TRACER *SampleTracer
    );

typedef struct _SAMPLE_TRACER_GENERATOR_VTABLE {
    PSAMPLE_TRACER_GENERATOR_GENERATOR_SAMPLE_TRACER_ROUTINE GenerateSampleTracerRoutine;
    PFREE_ROUTINE FreeRoutine;
} SAMPLE_TRACER_GENERATOR_VTABLE, *PSAMPLE_TRACER_GENERATOR_VTABLE;

typedef CONST SAMPLE_TRACER_GENERATOR_VTABLE *PCSAMPLE_TRACER_GENERATOR_VTABLE;

typedef struct _SAMPLE_TRACER_GENERATOR SAMPLE_TRACER_GENERATOR, *PSAMPLE_TRACER_GENERATOR;
typedef CONST SAMPLE_TRACER_GENERATOR *PCSAMPLE_TRACER_GENERATOR;

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISCAMERAAPI
ISTATUS
SampleTracerGeneratorAllocate(
    _In_ PCSAMPLE_TRACER_GENERATOR_VTABLE SampleTracerGeneratorVTable,
    _When_(DataSizeInBytes != 0, _In_reads_bytes_opt_(DataSizeInBytes)) PCVOID Data,
    _In_ SIZE_T DataSizeInBytes,
    _When_(DataSizeInBytes != 0, _Pre_satisfies_(_Curr_ != 0 && (_Curr_ & (_Curr_ - 1)) == 0 && DataSizeInBytes % _Curr_ == 0)) SIZE_T DataAlignment,
    _Out_ PSAMPLE_TRACER_GENERATOR *SampleTracerGenerator
    );

IRISCAMERAAPI
VOID
SampleTracerGeneratorFree(
    _In_opt_ _Post_invalid_ PSAMPLE_TRACER_GENERATOR SampleTracerGenerator
    );

#endif // _SAMPLE_TRACER_GENERATOR_IRIS_CAMERA_