/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    iriscameraplusplus_sampletracer.h

Abstract:

    This file contains the definitions for the 
    IrisCamera++ SampleTracer type.

--*/

#include <iriscameraplusplus.h>

#ifndef _SAMPLE_TRACER_IRIS_CAMERA_PLUS_PLUS_HEADER_
#define _SAMPLE_TRACER_IRIS_CAMERA_PLUS_PLUS_HEADER_

namespace IrisCamera {

//
// Types
//

class SampleTracer final {
public:
    SampleTracer(
        _In_ PSAMPLE_TRACER SampleTracerPtr
        )
    : Data(SampleTracerPtr)
    { 
        if (SampleTracerPtr == nullptr)
        {
            throw std::invalid_argument("SampleTracerPtr");
        }
    }

    _Ret_
    PCSAMPLE_TRACER
    AsPCSAMPLE_TRACER(
        void
        ) const
    {
        return Data;
    }

    _Ret_
    PSAMPLE_TRACER
    Release(
        void
        )
    {
        PSAMPLE_TRACER Result = Data;
        Data = nullptr;
        return Result;
    }

    ~SampleTracer(
        void
        )
    {
        SampleTracerFree(Data);
    }

private:
    PSAMPLE_TRACER Data;
};

} // namespace IrisCamera

#endif // _SAMPLE_TRACER_IRIS_CAMERA_PLUS_PLUS_HEADER_
