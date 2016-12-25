/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    sampletracerallocator.c

Abstract:

    This file contains the definitions for the SAMPLE_TRACER_ALLOCATOR type.

--*/

#include <iriscamerap.h>
#include <iriscommon_pointerlist.h>

//
// Types
//

struct _SAMPLE_TRACER_ALLOCATOR {
    POINTER_LIST AllocatedRngs;
};

//
// Private Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
SampleTracerAllocatorCreate(
    _Out_ PSAMPLE_TRACER_ALLOCATOR *SampleTracerAllocator
    )
{
    PSAMPLE_TRACER_ALLOCATOR AllocatedSampleTracerAllocator;
    ISTATUS Status;

    if (SampleTracerAllocator == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    AllocatedSampleTracerAllocator = (PSAMPLE_TRACER_ALLOCATOR) malloc(sizeof(SAMPLE_TRACER_ALLOCATOR));

    if (AllocatedSampleTracerAllocator == NULL)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    Status = PointerListInitialize(&AllocatedSampleTracerAllocator->AllocatedRngs);

    if (Status != ISTATUS_SUCCESS)
    {
        ASSERT(Status == ISTATUS_ALLOCATION_FAILED);
        free(AllocatedSampleTracerAllocator);
        return Status;
    }

    *SampleTracerAllocator = AllocatedSampleTracerAllocator;

    return ISTATUS_SUCCESS;
}

VOID
SampleTracerAllocatorClear(
    _Inout_ PSAMPLE_TRACER_ALLOCATOR SampleTracerAllocator
    )
{
    SIZE_T Index;
    SIZE_T NumberOfRngs;
    PSAMPLE_TRACER Rng;

    if (SampleTracerAllocator == NULL)
    {
        return;
    }

    NumberOfRngs = PointerListGetSize(&SampleTracerAllocator->AllocatedRngs);

    for (Index = 0; Index < NumberOfRngs; Index += 1)
    {
        Rng = (PSAMPLE_TRACER) PointerListRetrieveAtIndex(&SampleTracerAllocator->AllocatedRngs, 
                                                          Index);

        SampleTracerFree(Rng);
    }

    PointerListClear(&SampleTracerAllocator->AllocatedRngs);
}

VOID
SampleTracerAllocatorFree(
    _In_opt_ _Post_invalid_ PSAMPLE_TRACER_ALLOCATOR SampleTracerAllocator
    )
{
    if (SampleTracerAllocator == NULL)
    {
        return;
    }

    SampleTracerAllocatorClear(SampleTracerAllocator);
    PointerListDestroy(&SampleTracerAllocator->AllocatedRngs);
    free(SampleTracerAllocator);
}

//
// Public Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
SampleTracerAllocatorAllocate(
    _Inout_ PSAMPLE_TRACER_ALLOCATOR SampleTracerAllocator,
    _In_ PCSAMPLE_TRACER_VTABLE SampleTracerVTable,
    _When_(DataSizeInBytes != 0, _In_reads_bytes_opt_(DataSizeInBytes)) PCVOID Data,
    _In_ SIZE_T DataSizeInBytes,
    _When_(DataSizeInBytes != 0, _Pre_satisfies_(_Curr_ != 0 && (_Curr_ & (_Curr_ - 1)) == 0 && DataSizeInBytes % _Curr_ == 0)) SIZE_T DataAlignment,
    _Out_ PSAMPLE_TRACER *Rng
    )
{
    PSAMPLE_TRACER AllocatedRng;
    ISTATUS Status;

    if (SampleTracerAllocator == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (SampleTracerVTable == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
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

    if (Rng == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_05;
    }

    Status = PointerListPrepareToAddPointers(&SampleTracerAllocator->AllocatedRngs, 1);

    if (Status != ISTATUS_SUCCESS)
    {
        ASSERT(Status == ISTATUS_ALLOCATION_FAILED);
        return Status;
    }

    Status = SampleTracerAllocate(SampleTracerVTable,
                                  Data,
                                  DataSizeInBytes,
                                  DataAlignment,
                                  &AllocatedRng);

    if (Status != ISTATUS_SUCCESS)
    {
        ASSERT(Status == ISTATUS_ALLOCATION_FAILED);
        return Status;
    }

    PointerListAddPointer(&SampleTracerAllocator->AllocatedRngs, AllocatedRng);

    *Rng = AllocatedRng;

    return ISTATUS_SUCCESS;
}