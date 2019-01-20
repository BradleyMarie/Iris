/*++

Copyright (c) 2019 Brad Weinberger

Module Name:

    spectrum_compositor_test_util.c

Abstract:

    Adapter for allocating spectrum compositor from tests, since C++ doesn't
    support loading stdatomic.h

--*/

#include "iris_physx/spectrum_compositor_internal.h"
#include "iris_physx/spectrum_compositor_test_util.h"

_Ret_maybenull_
PSPECTRUM_COMPOSITOR
SpectrumCompositorAllocate(
    void
    )
{
    PSPECTRUM_COMPOSITOR compositor = 
        (PSPECTRUM_COMPOSITOR) malloc(sizeof(SPECTRUM_COMPOSITOR));
    if (compositor == NULL)
    {
        return NULL;
    }

    bool success = SpectrumCompositorInitialize(compositor);
    if (!success)
    {
        free(compositor);
        return NULL;
    }

    return compositor;
}

void
SpectrumCompositorFree(
    _In_opt_ _Post_invalid_ PSPECTRUM_COMPOSITOR compositor
    )
{
    if (compositor == NULL)
    {
        return;
    }

    SpectrumCompositorDestroy(compositor);
    free(compositor);
}