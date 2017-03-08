/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    sampletracerbase.cpp

Abstract:

    This file contains the definitions for the 
    IrisCamera++ SampleTracerBase type.

--*/

#include <iriscameraplusplus.h>
namespace IrisCamera {

//
// Adapter Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
static
ISTATUS
SampleTracerGenerateThreadStateAndCallbackAdapter(
    _In_ PCVOID Context,
    _In_ PRANDOM RngPtr,
    _In_ PCSAMPLE_TRACER_CALLBACK Callback
    )
{
    ASSERT(Context != NULL);
    ASSERT(RngPtr != NULL);
    ASSERT(Callback != NULL);

    SampleTracerBase **SampleTracerBasePtr = (SampleTracerBase**) Context;

    std::function<void(PVOID)> Func = [=](PVOID ThreadState) -> void {
        ISTATUS Status = SampleTracerCallback(Callback, ThreadState);

        if (Status != ISTATUS_SUCCESS)
        {
            throw std::runtime_error(Iris::ISTATUSToCString(Status));
        }
    };

    IrisAdvanced::Random Rng(RngPtr);

    (*SampleTracerBasePtr)->GenerateThreadStateAndCallback(Func, Rng);

    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
static
ISTATUS
SampleTracerTraceAdapter(
    _In_ PCVOID Context,
    _In_ PVOID ThreadState,
    _In_ RAY WorldRay,
    _In_ PRANDOM RngPtr,
    _Out_ PCOLOR3 Color
    )
{
    assert(Context != NULL);
    assert(RayValidate(WorldRay) != FALSE);
    assert(RngPtr != NULL);
    assert(Color != NULL);

    SampleTracerBase **SampleTracerBasePtr = (SampleTracerBase**) Context;
    IrisAdvanced::Random Rng(RngPtr);
    IrisAdvanced::Color3 Result = (*SampleTracerBasePtr)->Trace(ThreadState,
                                                                Iris::Ray(WorldRay),
                                                                Rng);

    *Color = Result.AsCOLOR3();

    return ISTATUS_SUCCESS;
}

static
VOID
SampleTracerFreeAdapter(
    _In_ _Post_invalid_ PVOID Context
    )
{
    assert(Context != nullptr);

    SampleTracerBase **SampleTracerBasePtr = (SampleTracerBase**) Context;
    delete *SampleTracerBasePtr;
}

const static SAMPLE_TRACER_VTABLE InteropVTable {
    SampleTracerGenerateThreadStateAndCallbackAdapter,
    SampleTracerTraceAdapter,
    SampleTracerFreeAdapter
};

//
// Functions
//

SampleTracer
SampleTracerBase::Create(
    _In_ std::unique_ptr<SampleTracerBase> SampleTracerBasePtr
    )
{
    if (!SampleTracerBasePtr)
    {
        throw std::invalid_argument("SampleTracerBasePtr");
    }

    SampleTracerBase *UnmanagedSampleTracerBasePtr = SampleTracerBasePtr.release();
    PSAMPLE_TRACER SampleTracerPtr;

    ISTATUS Success = SampleTracerAllocate(&InteropVTable,
                                           &UnmanagedSampleTracerBasePtr,
                                           sizeof(SampleTracerBase*),
                                           alignof(SampleTracerBase*),
                                           &SampleTracerPtr);

    if (Success != ISTATUS_SUCCESS)
    {
        delete UnmanagedSampleTracerBasePtr;
        throw std::bad_alloc();
    }
    
    return SampleTracer(SampleTracerPtr);
}

} // namespace IrisCamera