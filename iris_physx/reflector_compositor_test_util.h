/*++

Copyright (c) 2021 Brad Weinberger

Module Name:

    reflector_compositor_test_util.h

Abstract:

    Adapter for allocating reflector allocator from tests, since C++ doesn't 
    support loading stdatomic.h

--*/

#ifndef _IRIS_PHYSX_REFLECTOR_COMPOSITOR_TEST_UTIL_
#define _IRIS_PHYSX_REFLECTOR_COMPOSITOR_TEST_UTIL_

#include "iris_physx/reflector_compositor.h"

_Ret_maybenull_
PREFLECTOR_COMPOSITOR
ReflectorCompositorCreate(
    void
    );

void
ReflectorCompositorFree(
    _In_opt_ _Post_invalid_ PREFLECTOR_COMPOSITOR allocator
    );

#endif // _IRIS_PHYSX_REFLECTOR_COMPOSITOR_TEST_UTIL_