/*++

Copyright (c) 2019 Brad Weinberger

Module Name:

    spectrum_compositor_internal.h

Abstract:

    Internal headers for spectrum compositor.

--*/

#ifndef _IRIS_PHYSX_SPECTRUM_COMPOSITOR_INTERNAL_
#define _IRIS_PHYSX_SPECTRUM_COMPOSITOR_INTERNAL_

#include "common/static_allocator.h"
#include "iris_physx/reflector.h"
#include "iris_physx/spectrum_internal.h"

//
// Types
//

typedef struct _ATTENUATED_SPECTRUM {
    struct _SPECTRUM header;
    const struct _SPECTRUM *spectrum;
    float_t attenuation;
} ATTENUATED_SPECTRUM, *PATTENUATED_SPECTRUM;

typedef const ATTENUATED_SPECTRUM *PCATTENUATED_SPECTRUM;

typedef struct _SUM_SPECTRUM {
    struct _SPECTRUM header;
    const struct _SPECTRUM *spectrum0;
    const struct _SPECTRUM *spectrum1;
} SUM_SPECTRUM, *PSUM_SPECTRUM;

typedef const SUM_SPECTRUM *PCSUM_SPECTRUM;

typedef struct _ATTENUATED_REFLECTION_SPECTRUM {
    struct _SPECTRUM header;
    const struct _SPECTRUM *spectrum;
    PCREFLECTOR reflector;
    float_t attenuation;
} ATTENUATED_REFLECTION_SPECTRUM, *PATTENUATED_REFLECTION_SPECTRUM;

typedef const ATTENUATED_REFLECTION_SPECTRUM *PCATTENUATED_REFLECTION_SPECTRUM;

struct _SPECTRUM_COMPOSITOR {
    STATIC_MEMORY_ALLOCATOR attenuated_reflection_spectrum_allocator;
    STATIC_MEMORY_ALLOCATOR attenuated_spectrum_allocator;
    STATIC_MEMORY_ALLOCATOR sum_spectrum_allocator;
};

//
// Functions
//

_Check_return_
_Success_(return != 0)
static
inline
bool
SpectrumCompositorInitialize(
    _Out_ struct _SPECTRUM_COMPOSITOR *compositor
    )
{
    assert(compositor != NULL);

    bool success = StaticMemoryAllocatorInitialize(
        &compositor->attenuated_reflection_spectrum_allocator,
        sizeof(ATTENUATED_REFLECTION_SPECTRUM));
    if (!success)
    {
        return false;
    }

    success = StaticMemoryAllocatorInitialize(
        &compositor->attenuated_spectrum_allocator,
        sizeof(ATTENUATED_SPECTRUM));
    if (!success)
    {
        StaticMemoryAllocatorDestroy(
            &compositor->attenuated_reflection_spectrum_allocator);
        return false;
    }

    success = StaticMemoryAllocatorInitialize(
        &compositor->sum_spectrum_allocator,
        sizeof(SUM_SPECTRUM));
    if (!success)
    {
        StaticMemoryAllocatorDestroy(
            &compositor->attenuated_reflection_spectrum_allocator);
        StaticMemoryAllocatorDestroy(
            &compositor->attenuated_spectrum_allocator);
        return false;
    }

    return true;
}

static
inline
void
SpectrumCompositorClear(
    _Inout_ struct _SPECTRUM_COMPOSITOR *compositor
    )
{
    assert(compositor != NULL);

    StaticMemoryAllocatorFreeAll(
        &compositor->attenuated_reflection_spectrum_allocator);
    StaticMemoryAllocatorFreeAll(&compositor->attenuated_spectrum_allocator);
    StaticMemoryAllocatorFreeAll(&compositor->sum_spectrum_allocator);
}

static
inline
void
SpectrumCompositorDestroy(
    _Inout_ struct _SPECTRUM_COMPOSITOR *compositor
    )
{
    assert(compositor != NULL);

    StaticMemoryAllocatorDestroy(
        &compositor->attenuated_reflection_spectrum_allocator);
    StaticMemoryAllocatorDestroy(&compositor->attenuated_spectrum_allocator);
    StaticMemoryAllocatorDestroy(&compositor->sum_spectrum_allocator);
}

#endif // _IRIS_PHYSX_SPECTRUM_COMPOSITOR_INTERNAL_