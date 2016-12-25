/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    iriscameraplusplus_sampletracerallocator.h

Abstract:

    This file contains the definitions for the 
    IrisCamera++ SampleTracerAllocator type.

--*/

#include <iriscameraplusplus.h>

#ifndef _SAMPLE_TRACER_ALLOCATOR_IRIS_CAMERA_PLUS_PLUS_HEADER_
#define _SAMPLE_TRACER_ALLOCATOR_IRIS_CAMERA_PLUS_PLUS_HEADER_

namespace IrisCamera {

//
// Types
//

class SampleTracerAllocator final {
public:
    SampleTracerAllocator(
        _In_ PSAMPLE_TRACER_ALLOCATOR SampleTracerAllocatorPtr
        )
    : Data(SampleTracerAllocatorPtr)
    { 
        if (SampleTracerAllocatorPtr == nullptr)
        {
            throw std::invalid_argument("SampleTracerAllocatorPtr");
        }
    }

    _Ret_
    PSAMPLE_TRACER_ALLOCATOR
    AsPSAMPLE_TRACER_ALLOCATOR(
        void
        ) const
    {
        return Data;
    }

private:
    PSAMPLE_TRACER_ALLOCATOR Data;
};

} // namespace IrisCamera

#endif // _SAMPLE_TRACER_ALLOCATOR_IRIS_CAMERA_PLUS_PLUS_HEADER_
