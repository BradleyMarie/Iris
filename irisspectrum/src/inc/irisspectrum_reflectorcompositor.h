/*++

Copyright (c) 2015 Brad Weinberger

Module Name:

    irisspectrum_reflectorcompositor.h

Abstract:

    This file contains the internal definitions for 
    the REFLECTOR_COMPOSITOR type.

--*/

#ifndef _REFLECTOR_COMPOSITOR_IRIS_REFLECTOR_INTERNAL_
#define _REFLECTOR_COMPOSITOR_IRIS_REFLECTOR_INTERNAL_

#include <irisspectrump.h>

//
// Types
//

typedef struct _FMA_REFLECTOR {
    REFLECTOR ReflectorHeader;
    PCREFLECTOR Reflector0;
    PCREFLECTOR Reflector1;
    FLOAT Attenuation;
} FMA_REFLECTOR, *PFMA_REFLECTOR;

typedef CONST FMA_REFLECTOR *PCFMA_REFLECTOR;

typedef struct _ATTENUATED_REFLECTOR {
    REFLECTOR ReflectorHeader;
    PCREFLECTOR Reflector;
    FLOAT Attenuation;
} ATTENUATED_REFLECTOR, *PATTENUATED_REFLECTOR;

typedef CONST ATTENUATED_REFLECTOR *PCATTENUATED_REFLECTOR;

typedef struct _SUM_REFLECTOR {
    REFLECTOR ReflectorHeader;
    PCREFLECTOR Reflector0;
    PCREFLECTOR Reflector1;
} SUM_REFLECTOR, *PSUM_REFLECTOR;

typedef CONST SUM_REFLECTOR *PCSUM_REFLECTOR;

struct _REFLECTOR_COMPOSITOR {
    STATIC_MEMORY_ALLOCATOR AttenuatedReflectorAllocator;
    STATIC_MEMORY_ALLOCATOR FmaReflectorAllocator;
    STATIC_MEMORY_ALLOCATOR SumReflectorAllocator;
};

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS
ReflectorCompositorInitialize(
    _Out_ PREFLECTOR_COMPOSITOR Compositor
    )
{
    ISTATUS Status;

    ASSERT(Compositor != NULL);

    Status = StaticMemoryAllocatorInitialize(&Compositor->AttenuatedReflectorAllocator,
                                             sizeof(ATTENUATED_REFLECTOR));

    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }

    Status = StaticMemoryAllocatorInitialize(&Compositor->FmaReflectorAllocator,
                                             sizeof(FMA_REFLECTOR));

    if (Status != ISTATUS_SUCCESS)
    {
        StaticMemoryAllocatorDestroy(&Compositor->AttenuatedReflectorAllocator);
        return Status;
    }

    Status = StaticMemoryAllocatorInitialize(&Compositor->SumReflectorAllocator,
                                             sizeof(SUM_REFLECTOR));

    if (Status != ISTATUS_SUCCESS)
    {
        StaticMemoryAllocatorDestroy(&Compositor->AttenuatedReflectorAllocator);
        StaticMemoryAllocatorDestroy(&Compositor->FmaReflectorAllocator);
        return Status;
    }

    return ISTATUS_SUCCESS;    
}

SFORCEINLINE
VOID
ReflectorCompositorClear(
    _Inout_ PREFLECTOR_COMPOSITOR Compositor
    )
{
    ASSERT(Compositor != NULL);

    StaticMemoryAllocatorFreeAll(&Compositor->AttenuatedReflectorAllocator);
    StaticMemoryAllocatorFreeAll(&Compositor->FmaReflectorAllocator);
    StaticMemoryAllocatorFreeAll(&Compositor->SumReflectorAllocator);
}

SFORCEINLINE
VOID
ReflectorCompositorDestroy(
    _In_ _Post_invalid_ PREFLECTOR_COMPOSITOR Compositor
    )
{
    ASSERT(Compositor != NULL);

    StaticMemoryAllocatorDestroy(&Compositor->AttenuatedReflectorAllocator);
    StaticMemoryAllocatorDestroy(&Compositor->FmaReflectorAllocator);
    StaticMemoryAllocatorDestroy(&Compositor->SumReflectorAllocator);
}

#endif // _REFLECTOR_COMPOSITOR_IRIS_REFLECTOR_INTERNAL_