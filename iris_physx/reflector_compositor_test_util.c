/*++

Copyright (c) 2019 Brad Weinberger

Module Name:

    reflector_compositor_test_util.c

Abstract:

    Adapter for allocating reflector allocator from tests, since C++ doesn't 
    support loading stdatomic.h

--*/

#include "iris_physx/reflector_compositor_internal.h"
#include "iris_physx/reflector_compositor_test_util.h"

_Ret_maybenull_
PREFLECTOR_COMPOSITOR
ReflectorCompositorCreate(
    void
    )
{
    PREFLECTOR_COMPOSITOR allocator = 
        (PREFLECTOR_COMPOSITOR) malloc(sizeof(REFLECTOR_COMPOSITOR));
    if (allocator == NULL)
    {
        return NULL;
    }

    bool success = ReflectorCompositorInitialize(allocator);
    if (!success)
    {
        free(allocator);
        return NULL;
    }

    return allocator;
}

void
ReflectorCompositorFree(
    _In_opt_ _Post_invalid_ PREFLECTOR_COMPOSITOR allocator
    )
{
    if (allocator == NULL)
    {
        return;
    }

    ReflectorCompositorDestroy(allocator);
    free(allocator);
}