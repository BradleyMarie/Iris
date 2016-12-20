/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    sampletracer.c

Abstract:

    This file contains the definitions for the SAMPLE_TRACER type.

--*/

#include <iriscamerap.h>

//
// Types
//

struct _SAMPLE_TRACER {
    PCSAMPLE_TRACER_VTABLE VTable;
    PVOID Data;
};

//
// Public Functions
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
    )
{
    PSAMPLE_TRACER AllocatedSampleTracer;
    BOOL AllocationSuccessful;
    PVOID HeaderAllocation;
    PVOID DataAllocation;

    if (SampleTracerVTable == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (DataSizeInBytes != 0)
    {
        if (Data == NULL)
        {
            return ISTATUS_INVALID_ARGUMENT_COMBINATION_00;
        }

        if (DataAlignment == 0 ||
            DataAlignment & DataAlignment - 1)
        {
            return ISTATUS_INVALID_ARGUMENT_COMBINATION_01;
        }

        if (DataSizeInBytes % DataAlignment != 0)
        {
            return ISTATUS_INVALID_ARGUMENT_COMBINATION_02;
        }
    }

    if (SampleTracer == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    AllocationSuccessful = IrisAlignedAllocWithHeader(sizeof(SAMPLE_TRACER),
                                                      _Alignof(SAMPLE_TRACER),
                                                      &HeaderAllocation,
                                                      DataSizeInBytes,
                                                      DataAlignment,
                                                      &DataAllocation,
                                                      NULL);

    if (AllocationSuccessful == FALSE)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    AllocatedSampleTracer = (PSAMPLE_TRACER) HeaderAllocation;

    if (DataSizeInBytes != 0)
    {
        memcpy(DataAllocation, Data, DataSizeInBytes);
    }

    AllocatedSampleTracer->VTable = SampleTracerVTable;
    AllocatedSampleTracer->Data = DataAllocation;

    *SampleTracer = AllocatedSampleTracer;

    return ISTATUS_SUCCESS;
}

_Success_(return == ISTATUS_SUCCESS)
ISTATUS
SampleTracerTrace(
    _In_ PCSAMPLE_TRACER SampleTracer,
    _In_ RAY WorldRay,
    _In_ PRANDOM_REFERENCE Rng,
    _Out_ PCOLOR3 Color
    )
{
    ISTATUS Status;

    if (SampleTracer == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (RayValidate(WorldRay) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (Rng == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (Color == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    Status = SampleTracer->VTable->TraceRoutine(SampleTracer->Data,
                                                WorldRay,
                                                Rng,
                                                Color);
    
    return Status;
}

VOID
SampleTracerFree(
    _In_opt_ _Post_invalid_ PSAMPLE_TRACER SampleTracer
    )
{
    PFREE_ROUTINE FreeRoutine;

    if (SampleTracer == NULL)
    {
        return;
    }
    
    FreeRoutine = SampleTracer->VTable->FreeRoutine;

    if (FreeRoutine != NULL)
    {
        FreeRoutine(SampleTracer->Data);
    }

    IrisAlignedFree(SampleTracer);
}
