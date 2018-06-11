/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    test_util.c

Abstract:

    Adapter for allocating spectrum compositor and reflector allocator from 
    tests, since C++ doesn't support loading stdatomic.h

--*/

#include "iris_spectrum/test_util.h"

#include "iris_spectrum/reflector_allocator_internal.h"
#include "iris_spectrum/spectrum_compositor_internal.h"

_Ret_maybenull_
PREFLECTOR_ALLOCATOR
ReflectorAllocatorCreate(
    void
    )
{
    PREFLECTOR_ALLOCATOR allocator = 
        (PREFLECTOR_ALLOCATOR) malloc(sizeof(REFLECTOR_ALLOCATOR));
    if (allocator == NULL)
    {
        return NULL;
    }

    ReflectorAllocatorInitialize(allocator);

    return allocator;
}

void
ReflectorAllocatorFree(
    _In_opt_ _Post_invalid_ PREFLECTOR_ALLOCATOR allocator
    )
{
    if (allocator == NULL)
    {
        return;
    }

    ReflectorAllocatorDestroy(allocator);
    free(allocator);
}

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