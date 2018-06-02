/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    test_util.c

Abstract:

    Adapter for allocating spectrum compositor from tests, since C++ 
    doesn't support loading stdatomic.h

--*/

#include "iris_spectrum/test_util.h"
#include "iris_spectrum/spectrum_compositor_internal.h"

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