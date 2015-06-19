/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    irisspectrum_spectrumcompositor.h

Abstract:

    This file contains the internal definitions for 
    the SPECTRUM_COMPOSITOR type.

--*/

#ifndef _SPECTRUM_COMPOSITOR_IRIS_SPECTRUM_INTERNAL_
#define _SPECTRUM_COMPOSITOR_IRIS_SPECTRUM_INTERNAL_

#include <irisspectrump.h>

//
// Types
//

typedef struct _FMA_SPECTRUM {
    SPECTRUM SpectrumHeader;
    PCSPECTRUM Spectrum0;
    PCSPECTRUM Spectrum1;
    FLOAT Attenuation;
} FMA_SPECTRUM, *PFMA_SPECTRUM;

typedef CONST FMA_SPECTRUM *PCFMA_SPECTRUM;

typedef struct _ATTENUATED_SPECTRUM {
    SPECTRUM SpectrumHeader;
    PCSPECTRUM Spectrum;
    FLOAT Attenuation;
} ATTENUATED_SPECTRUM, *PATTENUATED_SPECTRUM;

typedef CONST ATTENUATED_SPECTRUM *PCATTENUATED_SPECTRUM;

typedef struct _SUM_SPECTRUM {
    SPECTRUM SpectrumHeader;
    PCSPECTRUM Spectrum0;
    PCSPECTRUM Spectrum1;
} SUM_SPECTRUM, *PSUM_SPECTRUM;

typedef CONST SUM_SPECTRUM *PCSUM_SPECTRUM;

typedef struct _REFLECTION_SPECTRUM {
    SPECTRUM SpectrumHeader;
    PCSPECTRUM Spectrum;
    PCREFLECTOR Reflector;
} REFLECTION_SPECTRUM, *PREFLECTION_SPECTRUM;

typedef CONST REFLECTION_SPECTRUM *PCREFLECTION_SPECTRUM;

struct _SPECTRUM_COMPOSITOR {
    STATIC_MEMORY_ALLOCATOR ReflectionSpectrumAllocator;
    STATIC_MEMORY_ALLOCATOR AttenuatedSpectrumAllocator;
    STATIC_MEMORY_ALLOCATOR FmaSpectrumAllocator;
    STATIC_MEMORY_ALLOCATOR SumSpectrumAllocator;
};

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS
SpectrumCompositorInitialize(
    _Out_ PSPECTRUM_COMPOSITOR Compositor
    )
{
    ISTATUS Status;

    ASSERT(Compositor != NULL);

    Status = StaticMemoryAllocatorInitialize(&Compositor->ReflectionSpectrumAllocator,
                                             sizeof(REFLECTION_SPECTRUM));

    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }

    Status = StaticMemoryAllocatorInitialize(&Compositor->AttenuatedSpectrumAllocator,
                                             sizeof(ATTENUATED_SPECTRUM));

    if (Status != ISTATUS_SUCCESS)
    {
        StaticMemoryAllocatorDestroy(&Compositor->ReflectionSpectrumAllocator);
        return Status;
    }

    Status = StaticMemoryAllocatorInitialize(&Compositor->FmaSpectrumAllocator,
                                             sizeof(FMA_SPECTRUM));

    if (Status != ISTATUS_SUCCESS)
    {
        StaticMemoryAllocatorDestroy(&Compositor->ReflectionSpectrumAllocator);
        StaticMemoryAllocatorDestroy(&Compositor->AttenuatedSpectrumAllocator);
        return Status;
    }

    Status = StaticMemoryAllocatorInitialize(&Compositor->SumSpectrumAllocator,
                                             sizeof(SUM_SPECTRUM));

    if (Status != ISTATUS_SUCCESS)
    {
        StaticMemoryAllocatorDestroy(&Compositor->ReflectionSpectrumAllocator);
        StaticMemoryAllocatorDestroy(&Compositor->AttenuatedSpectrumAllocator);
        StaticMemoryAllocatorDestroy(&Compositor->FmaSpectrumAllocator);
        return Status;
    }

    return ISTATUS_SUCCESS;    
}

SFORCEINLINE
VOID
SpectrumCompositorClear(
    _Inout_ PSPECTRUM_COMPOSITOR Compositor
    )
{
    StaticMemoryAllocatorFreeAll(&Compositor->ReflectionSpectrumAllocator);
    StaticMemoryAllocatorFreeAll(&Compositor->AttenuatedSpectrumAllocator);
    StaticMemoryAllocatorFreeAll(&Compositor->FmaSpectrumAllocator);
    StaticMemoryAllocatorFreeAll(&Compositor->SumSpectrumAllocator);
}

SFORCEINLINE
VOID
SpectrumCompositorDestroy(
    _Inout_ PSPECTRUM_COMPOSITOR Compositor
    )
{
    StaticMemoryAllocatorDestroy(&Compositor->ReflectionSpectrumAllocator);
    StaticMemoryAllocatorDestroy(&Compositor->AttenuatedSpectrumAllocator);
    StaticMemoryAllocatorDestroy(&Compositor->FmaSpectrumAllocator);
    StaticMemoryAllocatorDestroy(&Compositor->SumSpectrumAllocator);
}

#endif // _SPECTRUM_COMPOSITOR_IRIS_SPECTRUM_INTERNAL_