/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    spectrum_compositor_test_util.h

Abstract:

    Adapter for allocating spectrum compositor from tests, since C++ doesn't
    support loading stdatomic.h

--*/

#ifndef _IRIS_PHYSX_SPECTRUM_COMPOSITOR_TEST_UTIL_
#define _IRIS_PHYSX_SPECTRUM_COMPOSITOR_TEST_UTIL_

#include "iris_physx/spectrum_compositor.h"

_Ret_maybenull_
PSPECTRUM_COMPOSITOR
SpectrumCompositorAllocate(
    void
    );

void
SpectrumCompositorFree(
    _In_opt_ _Post_invalid_ PSPECTRUM_COMPOSITOR compositor
    );

#endif // _IRIS_PHYSX_SPECTRUM_COMPOSITOR_TEST_UTIL_