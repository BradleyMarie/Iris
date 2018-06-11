/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    test_util.h

Abstract:

    Adapter for allocating spectrum compositor and reflector allocator from 
    tests, since C++ doesn't support loading stdatomic.h

--*/

#ifndef _IRIS_SPECTRUM_TEST_UTIL_
#define _IRIS_SPECTRUM_TEST_UTIL_

#include "iris_spectrum/reflector_allocator.h"
#include "iris_spectrum/spectrum_compositor.h"

_Ret_maybenull_
PREFLECTOR_ALLOCATOR
ReflectorAllocatorCreate(
    void
    );

void
ReflectorAllocatorFree(
    _In_opt_ _Post_invalid_ PREFLECTOR_ALLOCATOR allocator
    );

_Ret_maybenull_
PSPECTRUM_COMPOSITOR
SpectrumCompositorAllocate(
    void
    );

void
SpectrumCompositorFree(
    _In_opt_ _Post_invalid_ PSPECTRUM_COMPOSITOR compositor
    );

#endif // _IRIS_SPECTRUM_TEST_UTIL_