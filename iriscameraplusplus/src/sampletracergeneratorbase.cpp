/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    sampletracergeneratorbase.cpp

Abstract:

    This file contains the definitions for the 
    IrisCamera++ SampleTracerGeneratorBase type.

--*/

#include <iriscameraplusplus.h>
namespace IrisCamera {

//
// Adapter Functions
//

_Success_(return == ISTATUS_SUCCESS)
static
ISTATUS
SampleTracerGeneratorGenerateAdapter(
    _In_ PCVOID Context,
    _Out_ PSAMPLE_TRACER *SampleTracerPtr
    )
{
    assert(Context != NULL);
    assert(SampleTracer != NULL);

    const SampleTracerGeneratorBase **SampleTracerGeneratorBasePtr = (const SampleTracerGeneratorBase**) Context;
    *SampleTracerPtr = SampleTracerGeneratorBasePtr->Generate();

    return ISTATUS_SUCCESS;
}

static
VOID
SampleTracerGeneratorFreeAdapter(
    _In_ _Post_invalid_ PVOID Context
    )
{
    assert(Context != nullptr);

    SampleTracerGeneratorBase **SampleTracerGeneratorBasePtr = (SampleTracerGeneratorBase**) Context;
    delete *SampleTracerGeneratorBasePtr;
}

const static SAMPLE_TRACER_GENERATOR_VTABLE InteropVTable {
    SampleTracerGeneratorGenerateAdapter,
    SampleTracerGeneratorFreeAdapter
};

//
// Functions
//

SampleTracerGenerator
SampleTracerGeneratorBase::Create(
    _In_ std::unique_ptr<SampleTracerGeneratorBase> SampleTracerGeneratorBasePtr
    )
{
    if (!SampleTracerGeneratorBasePtr)
    {
        throw std::invalid_argument("SampleTracerGeneratorBasePtr");
    }

    SampleTracerGeneratorBase *UnmanagedSampleTracerGeneratorBasePtr = SampleTracerGeneratorBasePtr.release();
    PSAMPLE_TRACER SampleTracerGeneratorPtr;

    ISTATUS Success = SampleTracerGeneratorAllocate(&InteropVTable,
                                                    &UnmanagedSampleTracerGeneratorBasePtr,
                                                    sizeof(SampleTracerGeneratorBase*),
                                                    alignof(SampleTracerGeneratorBase*),
                                                    &SampleTracerGeneratorPtr);

    if (Success != ISTATUS_SUCCESS)
    {
        delete UnmanagedSampleTracerGeneratorBasePtr;
        throw std::bad_alloc();
    }
    
    return SampleTracerGenerator(SampleTracerGeneratorPtr);
}

} // namespace IrisCamera