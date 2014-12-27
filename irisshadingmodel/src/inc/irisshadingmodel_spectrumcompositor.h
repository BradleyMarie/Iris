/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    irisshadingmodel_spectrumcompositor.h

Abstract:

    This file contains the internal definitions for 
    the SPECTRUM_COMPOSITOR type.

--*/

#ifndef _SPECTRUM_COMPOSITOR_IRIS_SHADING_MODEL_INTERNAL_
#define _SPECTRUM_COMPOSITOR_IRIS_SHADING_MODEL_INTERNAL_

#include <irisshadingmodelp.h>
#include "../../iris/src/inc/iris_staticallocator.h"

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
    IRIS_STATIC_MEMORY_ALLOCATOR ReflectionSpectrumAllocator;
    IRIS_STATIC_MEMORY_ALLOCATOR AttenuatedSpectrumAllocator;
    IRIS_STATIC_MEMORY_ALLOCATOR FmaSpectrumAllocator;
    IRIS_STATIC_MEMORY_ALLOCATOR SumSpectrumAllocator;
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

    Status = IrisStaticMemoryAllocatorInitialize(&Compositor->ReflectionSpectrumAllocator,
                                                 sizeof(REFLECTION_SPECTRUM));

    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }

    Status = IrisStaticMemoryAllocatorInitialize(&Compositor->AttenuatedSpectrumAllocator,
                                                 sizeof(ATTENUATED_SPECTRUM));

    if (Status != ISTATUS_SUCCESS)
    {
        IrisStaticMemoryAllocatorDestroy(&Compositor->ReflectionSpectrumAllocator);
        return Status;
    }

    Status = IrisStaticMemoryAllocatorInitialize(&Compositor->FmaSpectrumAllocator,
                                                 sizeof(FMA_SPECTRUM));

    if (Status != ISTATUS_SUCCESS)
    {
        IrisStaticMemoryAllocatorDestroy(&Compositor->ReflectionSpectrumAllocator);
        IrisStaticMemoryAllocatorDestroy(&Compositor->AttenuatedSpectrumAllocator);
        return Status;
    }

    Status = IrisStaticMemoryAllocatorInitialize(&Compositor->SumSpectrumAllocator,
                                                 sizeof(SUM_SPECTRUM));

    if (Status != ISTATUS_SUCCESS)
    {
        IrisStaticMemoryAllocatorDestroy(&Compositor->ReflectionSpectrumAllocator);
        IrisStaticMemoryAllocatorDestroy(&Compositor->AttenuatedSpectrumAllocator);
        IrisStaticMemoryAllocatorDestroy(&Compositor->FmaSpectrumAllocator);
        return Status;
    }

    return ISTATUS_SUCCESS;    
}

SFORCEINLINE
VOID
SpectrumCompositorDestroy(
    _Inout_ PSPECTRUM_COMPOSITOR Compositor
    )
{
    IrisStaticMemoryAllocatorDestroy(&Compositor->ReflectionSpectrumAllocator);
    IrisStaticMemoryAllocatorDestroy(&Compositor->AttenuatedSpectrumAllocator);
    IrisStaticMemoryAllocatorDestroy(&Compositor->FmaSpectrumAllocator);
    IrisStaticMemoryAllocatorDestroy(&Compositor->SumSpectrumAllocator);
}

#endif // _SPECTRUM_COMPOSITOR_IRIS_SHADING_MODEL_INTERNAL_