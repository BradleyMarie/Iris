/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    iriscameraplusplus_sampletracergenerator.h

Abstract:

    This file contains the definitions for the 
    IrisCamera++ SampleTracerGenerator type.

--*/

#include <iriscameraplusplus.h>

#ifndef _SAMPLE_TRACER_GENERATOR_IRIS_CAMERA_PLUS_PLUS_HEADER_
#define _SAMPLE_TRACER_GENERATOR_IRIS_CAMERA_PLUS_PLUS_HEADER_

namespace IrisCamera {

//
// Types
//

class SampleTracerGenerator final {
public:
    SampleTracerGenerator(
        _In_ PSAMPLE_TRACER_GENERATOR SampleTracerGeneratorPtr
        )
    : Data(SampleTracerGeneratorPtr, [](PSAMPLE_TRACER_GENERATOR ToFree){ SampleTracerGeneratorFree(ToFree); })
    { 
        if (SampleTracerGeneratorPtr == nullptr)
        {
            throw std::invalid_argument("SampleTracerGeneratorPtr");
        }
    }

    _Ret_
    PCSAMPLE_TRACER_GENERATOR
    AsPCSAMPLE_TRACER_GENERATOR(
        void
        ) const
    {
        return Data.get();
    }

private:
    std::shared_ptr<SAMPLE_TRACER_GENERATOR> Data;
};

} // namespace IrisCamera

#endif // _SAMPLE_TRACER_GENERATOR_IRIS_CAMERA_PLUS_PLUS_HEADER_
