/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    iriscameraplusplus_sampletracerbase.h

Abstract:

    This file contains the definitions for the 
    IrisCamera++ SampleTracerBase type.

--*/

#include <iriscameraplusplus.h>

#ifndef _SAMPLE_TRACER_BASE_IRIS_CAMERA_PLUS_PLUS_HEADER_
#define _SAMPLE_TRACER_BASE_IRIS_CAMERA_PLUS_PLUS_HEADER_

namespace IrisCamera {

//
// Types
//

class SampleTracerBase {
protected:
    IRISCAMERAPLUSPLUSAPI
    static
    SampleTracer
    Create(
        _In_ std::unique_ptr<SampleTracerBase> SampleTracerBasePtr
        );

public:
    SampleTracerBase(
        void
        ) = default;

    virtual
    void
    GenerateThreadStateAndCallback(
        _In_ std::function<void(PVOID)> Callback
        ) = 0;

    virtual
    IrisAdvanced::Color3
    Trace(
        _In_opt_ PVOID ThreadState,
        _In_ const Iris::Ray & WorldRay,
        _In_ IrisAdvanced::Random Rng
        ) = 0;

    SampleTracerBase(
        _In_ const SampleTracerBase & ToCopy
        ) = delete;
        
    SampleTracerBase &
    operator=(
        _In_ const SampleTracerBase & ToCopy
        ) = delete;

    virtual
    ~SampleTracerBase(
        void
        ) = default;
};

} // namespace IrisCamera

#endif // _SAMPLE_TRACER_BASE_IRIS_CAMERA_PLUS_PLUS_HEADER_
