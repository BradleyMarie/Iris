/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    iriscameraplusplus_sampletracergeneratorbase.h

Abstract:

    This file contains the definitions for the 
    IrisCamera++ SampleTracerGeneratorBase type.

--*/

#include <iriscameraplusplus.h>

#ifndef _SAMPLE_TRACER_GENERATOR_BASE_IRIS_CAMERA_PLUS_PLUS_HEADER_
#define _SAMPLE_TRACER_GENERATOR_BASE_IRIS_CAMERA_PLUS_PLUS_HEADER_

namespace IrisCamera {

//
// Types
//

class SampleTracerGeneratorBase final {
protected:
    IRISCAMERAPLUSPLUSAPI
    static
    SampleTracerGenerator
    Create(
        _In_ std::unique_ptr<SampleTracerGeneratorBase> SampleTracerGeneratorBasePtr
        );

public:
    SampleTracerGeneratorBase(
        void
        )
    { }

    virtual
    SampleTracer
    Generate(
        void
        ) const = 0;

    SampleTracerGeneratorBase(
        _In_ const SampleTracerGeneratorBase & ToCopy
        ) = delete;
        
    SampleTracerGeneratorBase &
    operator=(
        _In_ const SampleTracerGeneratorBase & ToCopy
        ) = delete;

    virtual
    ~SampleTracerGeneratorBase(
        void
        )
    { }
};

} // namespace IrisCamera

#endif // _SAMPLE_TRACER_GENERATOR_BASE_IRIS_CAMERA_PLUS_PLUS_HEADER_
