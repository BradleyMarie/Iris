/*++

Copyright (c) 2020 Brad Weinberger

Module Name:

    reflector_compositor_internal.h

Abstract:

    Internal headers for reflector allocator.

--*/

#ifndef _IRIS_PHYSX_REFLECTOR_COMPOSITOR_INTERNAL_
#define _IRIS_PHYSX_REFLECTOR_COMPOSITOR_INTERNAL_

#include "common/static_allocator.h"
#include "iris_physx/reflector_internal.h"

//
// Types
//

typedef struct _ATTENUATED_REFLECTOR {
    struct _REFLECTOR header;
    const struct _REFLECTOR *reflector;
    float_t attenuation;
} ATTENUATED_REFLECTOR, *PATTENUATED_REFLECTOR;

typedef const ATTENUATED_REFLECTOR *PCATTENUATED_REFLECTOR;

typedef struct _ATTENUATED_SUM_REFLECTOR {
    struct _REFLECTOR header;
    const struct _REFLECTOR *added_reflector;
    const struct _REFLECTOR *attenuated_reflector;
    float_t attenuation;
} ATTENUATED_SUM_REFLECTOR, *PATTENUATED_SUM_REFLECTOR;

typedef const ATTENUATED_SUM_REFLECTOR *PCATTENUATED_SUM_REFLECTOR;

typedef struct _PRODUCT_REFLECTOR {
    struct _REFLECTOR header;
    const struct _REFLECTOR *multiplicand0;
    const struct _REFLECTOR *multiplicand1;
} PRODUCT_REFLECTOR, *PPRODUCT_REFLECTOR;

typedef const PRODUCT_REFLECTOR *PCPRODUCT_REFLECTOR;

struct _REFLECTOR_COMPOSITOR {
    STATIC_MEMORY_ALLOCATOR attenuated_sum_reflector_allocator;
    STATIC_MEMORY_ALLOCATOR attenuated_reflector_allocator;
    STATIC_MEMORY_ALLOCATOR product_reflector_allocator;
};

//
// Functions
//

_Check_return_
_Success_(return != 0)
static
inline
bool
ReflectorCompositorInitialize(
    _Out_ struct _REFLECTOR_COMPOSITOR *compositor
    )
{
    assert(compositor != NULL);

    bool success = StaticMemoryAllocatorInitialize(
        &compositor->attenuated_sum_reflector_allocator,
        sizeof(ATTENUATED_SUM_REFLECTOR));
    if (!success)
    {
        return false;
    }

    success = StaticMemoryAllocatorInitialize(
        &compositor->attenuated_reflector_allocator,
        sizeof(ATTENUATED_REFLECTOR));
    if (!success)
    {
        StaticMemoryAllocatorDestroy(
            &compositor->attenuated_sum_reflector_allocator);
    }

    success = StaticMemoryAllocatorInitialize(
        &compositor->product_reflector_allocator,
        sizeof(PRODUCT_REFLECTOR));
    if (!success)
    {
        StaticMemoryAllocatorDestroy(
            &compositor->attenuated_sum_reflector_allocator);
        StaticMemoryAllocatorDestroy(
            &compositor->attenuated_reflector_allocator);
    }

    return success;
}

static
inline
void
ReflectorCompositorClear(
    _Inout_ struct _REFLECTOR_COMPOSITOR *compositor
    )
{
    assert(compositor != NULL);

    StaticMemoryAllocatorFreeAll(&compositor->attenuated_sum_reflector_allocator);
    StaticMemoryAllocatorFreeAll(&compositor->attenuated_reflector_allocator);
    StaticMemoryAllocatorFreeAll(&compositor->product_reflector_allocator);
}

static
inline
void
ReflectorCompositorDestroy(
    _Inout_ struct _REFLECTOR_COMPOSITOR *compositor
    )
{
    assert(compositor != NULL);

    StaticMemoryAllocatorDestroy(&compositor->attenuated_sum_reflector_allocator);
    StaticMemoryAllocatorDestroy(&compositor->attenuated_reflector_allocator);
    StaticMemoryAllocatorDestroy(&compositor->product_reflector_allocator);
}

#endif // _IRIS_PHYSX_REFLECTOR_COMPOSITOR_INTERNAL_