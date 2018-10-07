/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    reflector_allocator_test_util.h

Abstract:

    Adapter for allocating reflector allocator from tests, since C++ doesn't 
    support loading stdatomic.h

--*/

#ifndef _IRIS_PHYSX_REFLECTOR_ALLOCATOR_TEST_UTIL_
#define _IRIS_PHYSX_REFLECTOR_ALLOCATOR_TEST_UTIL_

#include "iris_physx/reflector_allocator.h"

_Ret_maybenull_
PREFLECTOR_ALLOCATOR
ReflectorAllocatorCreate(
    void
    );

void
ReflectorAllocatorFree(
    _In_opt_ _Post_invalid_ PREFLECTOR_ALLOCATOR allocator
    );

#endif // _IRIS_PHYSX_REFLECTOR_ALLOCATOR_TEST_UTIL_