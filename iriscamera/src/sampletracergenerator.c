/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    sampletracergenerator.c

Abstract:

    This file contains the definitions for the SAMPLE_TRACER type.

--*/

#include <iriscamerap.h>

//
// Types
//

struct _SAMPLE_TRACER_GENERATOR {
    PCSAMPLE_TRACER_GENERATOR_VTABLE VTable;
    PVOID Data;
};

//
// Private Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
SampleTracerGeneratorGenerateSampleTracer(
    _In_ PCSAMPLE_TRACER_GENERATOR SampleTracerGenerator,
    _In_ PSAMPLE_TRACER_ALLOCATOR SampleTracerAllocator,
    _Out_ PSAMPLE_TRACER *SampleTracer 
    )
{
    ISTATUS Status;

    ASSERT(SampleTracerGenerator != NULL);
    ASSERT(SampleTracerAllocator != NULL);
    ASSERT(SampleTracer != NULL);
    
    Status = SampleTracerGenerator->VTable->GenerateSampleTracerRoutine(SampleTracerGenerator->Data,    
                                                                        SampleTracerAllocator,
                                                                        SampleTracer);

    return Status;
}

//
// Public Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
SampleTracerGeneratorAllocate(
    _In_ PCSAMPLE_TRACER_GENERATOR_VTABLE SampleTracerGeneratorVTable,
    _When_(DataSizeInBytes != 0, _In_reads_bytes_opt_(DataSizeInBytes)) PCVOID Data,
    _In_ SIZE_T DataSizeInBytes,
    _When_(DataSizeInBytes != 0, _Pre_satisfies_(_Curr_ != 0 && (_Curr_ & (_Curr_ - 1)) == 0 && DataSizeInBytes % _Curr_ == 0)) SIZE_T DataAlignment,
    _Out_ PSAMPLE_TRACER_GENERATOR *SampleTracerGenerator
    )
{
    PSAMPLE_TRACER_GENERATOR AllocatedSampleTracerGenerator;
    BOOL AllocationSuccessful;
    PVOID HeaderAllocation;
    PVOID DataAllocation;

    if (SampleTracerGeneratorVTable == NULL)
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

    if (SampleTracerGenerator == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    AllocationSuccessful = IrisAlignedAllocWithHeader(sizeof(SAMPLE_TRACER_GENERATOR),
                                                      _Alignof(SAMPLE_TRACER_GENERATOR),
                                                      &HeaderAllocation,
                                                      DataSizeInBytes,
                                                      DataAlignment,
                                                      &DataAllocation,
                                                      NULL);

    if (AllocationSuccessful == FALSE)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    AllocatedSampleTracerGenerator = (PSAMPLE_TRACER_GENERATOR) HeaderAllocation;

    if (DataSizeInBytes != 0)
    {
        memcpy(DataAllocation, Data, DataSizeInBytes);
    }

    AllocatedSampleTracerGenerator->VTable = SampleTracerGeneratorVTable;
    AllocatedSampleTracerGenerator->Data = DataAllocation;

    *SampleTracerGenerator = AllocatedSampleTracerGenerator;

    return ISTATUS_SUCCESS;
}

VOID
SampleTracerGeneratorFree(
    _In_opt_ _Post_invalid_ PSAMPLE_TRACER_GENERATOR SampleTracerGenerator
    )
{
    PFREE_ROUTINE FreeRoutine;

    if (SampleTracerGenerator == NULL)
    {
        return;
    }
    
    FreeRoutine = SampleTracerGenerator->VTable->FreeRoutine;

    if (FreeRoutine != NULL)
    {
        FreeRoutine(SampleTracerGenerator->Data);
    }

    IrisAlignedFree(SampleTracerGenerator);
}
