/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    spectrumcompositor.c

Abstract:

    This file contains the function definitions 
    for the SPECTRUM_COMPOSITOR type.

--*/

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

typedef struct _ATTENUATED_REFLECTION_SPECTRUM {
    SPECTRUM SpectrumHeader;
    PCSPECTRUM Spectrum;
    PCREFLECTOR Reflector;
    FLOAT Attenuation;
} ATTENUATED_REFLECTION_SPECTRUM, *PATTENUATED_REFLECTION_SPECTRUM;

typedef CONST ATTENUATED_REFLECTION_SPECTRUM *PCATTENUATED_REFLECTION_SPECTRUM;

struct _SPECTRUM_COMPOSITOR_REFERENCE {
    STATIC_MEMORY_ALLOCATOR AttenuatedReflectionSpectrumAllocator;
    STATIC_MEMORY_ALLOCATOR ReflectionSpectrumAllocator;
    STATIC_MEMORY_ALLOCATOR AttenuatedSpectrumAllocator;
    STATIC_MEMORY_ALLOCATOR FmaSpectrumAllocator;
    STATIC_MEMORY_ALLOCATOR SumSpectrumAllocator;
};

struct _SPECTRUM_COMPOSITOR {
    SPECTRUM_COMPOSITOR_REFERENCE CompositorReference;
};

//
// Static Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
STATIC
ISTATUS
FmaSpectrumSample(
    _In_ PCVOID Context,
    _In_ FLOAT Wavelength,
    _Out_ PFLOAT Intensity
    )
{
    PCFMA_SPECTRUM FmaSpectrum;
    FLOAT SpectrumIntensity0;
    FLOAT SpectrumIntensity1;
    ISTATUS Status;

    ASSERT(Context != NULL);
    ASSERT(IsFiniteFloat(Wavelength) != FALSE);
    ASSERT(IsGreaterThanZeroFloat(Wavelength) != FALSE);
    ASSERT(Intensity != NULL);

    FmaSpectrum = (PCFMA_SPECTRUM) Context;

    Status = SpectrumSample(FmaSpectrum->Spectrum0,
                            Wavelength,
                            &SpectrumIntensity0);

    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }

    Status = SpectrumSample(FmaSpectrum->Spectrum1,
                            Wavelength,
                            &SpectrumIntensity1);

    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }

    *Intensity = FmaFloat(FmaSpectrum->Attenuation,
                          SpectrumIntensity1, 
                          SpectrumIntensity0);

    return ISTATUS_SUCCESS; 
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
STATIC
ISTATUS
AttenuatedSpectrumSample(
    _In_ PCVOID Context,
    _In_ FLOAT Wavelength,
    _Out_ PFLOAT Intensity
    )
{
    PCATTENUATED_SPECTRUM AttenuatedSpectrum;
    FLOAT OutputIntensity;
    ISTATUS Status;

    ASSERT(Context != NULL);
    ASSERT(IsFiniteFloat(Wavelength) != FALSE);
    ASSERT(IsGreaterThanZeroFloat(Wavelength) != FALSE);
    ASSERT(Intensity != NULL);

    AttenuatedSpectrum = (PCATTENUATED_SPECTRUM) Context;

    Status = SpectrumSample(AttenuatedSpectrum->Spectrum,
                            Wavelength,
                            &OutputIntensity);

    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }

    *Intensity = OutputIntensity * AttenuatedSpectrum->Attenuation;
    return ISTATUS_SUCCESS; 
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
STATIC
ISTATUS
SumSpectrumSample(
    _In_ PCVOID Context,
    _In_ FLOAT Wavelength,
    _Out_ PFLOAT Intensity
    )
{
    PCSUM_SPECTRUM SumSpectrum;
    FLOAT SpectrumIntensity0;
    FLOAT SpectrumIntensity1;
    ISTATUS Status;

    ASSERT(Context != NULL);
    ASSERT(IsFiniteFloat(Wavelength) != FALSE);
    ASSERT(IsGreaterThanZeroFloat(Wavelength) != FALSE);
    ASSERT(Intensity != NULL);

    SumSpectrum = (PCSUM_SPECTRUM) Context;

    Status = SpectrumSample(SumSpectrum->Spectrum0,
                            Wavelength,
                            &SpectrumIntensity0);

    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }

    Status = SpectrumSample(SumSpectrum->Spectrum1,
                            Wavelength,
                            &SpectrumIntensity1);

    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }

    *Intensity = SpectrumIntensity0 + SpectrumIntensity1;
    return ISTATUS_SUCCESS; 
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
STATIC
ISTATUS
ReflectionSpectrumSample(
    _In_ PCVOID Context,
    _In_ FLOAT Wavelength,
    _Out_ PFLOAT Intensity
    )
{
    PCREFLECTION_SPECTRUM ReflectionSpectrum;
    FLOAT SpectrumIntensity;
    ISTATUS Status;

    ASSERT(Context != NULL);
    ASSERT(IsFiniteFloat(Wavelength) != FALSE);
    ASSERT(IsGreaterThanZeroFloat(Wavelength) != FALSE);
    ASSERT(Intensity != NULL);

    ReflectionSpectrum = (PCREFLECTION_SPECTRUM) Context;

    Status = SpectrumSample(ReflectionSpectrum->Spectrum,
                            Wavelength,
                            &SpectrumIntensity);

    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }

    Status = ReflectorReflect(ReflectionSpectrum->Reflector,
                              Wavelength,
                              SpectrumIntensity,
                              Intensity);

    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }

    return ISTATUS_SUCCESS; 
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
STATIC
ISTATUS
AttenuatedReflectionSpectrumSample(
    _In_ PCVOID Context,
    _In_ FLOAT Wavelength,
    _Out_ PFLOAT Intensity
    )
{
    PCATTENUATED_REFLECTION_SPECTRUM AttenuatedReflectionSpectrum;
    FLOAT ReflectedIntensity;
    FLOAT SpectrumIntensity;
    ISTATUS Status;

    ASSERT(Context != NULL);
    ASSERT(IsFiniteFloat(Wavelength) != FALSE);
    ASSERT(IsGreaterThanZeroFloat(Wavelength) != FALSE);
    ASSERT(Intensity != NULL);

    AttenuatedReflectionSpectrum = (PCATTENUATED_REFLECTION_SPECTRUM) Context;

    Status = SpectrumSample(AttenuatedReflectionSpectrum->Spectrum,
                            Wavelength,
                            &SpectrumIntensity);

    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }

    Status = ReflectorReflect(AttenuatedReflectionSpectrum->Reflector,
                              Wavelength,
                              SpectrumIntensity,
                              &ReflectedIntensity);

    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }

    *Intensity = ReflectedIntensity * AttenuatedReflectionSpectrum->Attenuation;

    return ISTATUS_SUCCESS; 
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
STATIC
ISTATUS
ZeroSpectrumSample(
    _In_ PCVOID Context,
    _In_ FLOAT Wavelength,
    _Out_ PFLOAT Intensity
    )
{
    ASSERT(Context == NULL);
    ASSERT(IsFiniteFloat(Wavelength) != FALSE);
    ASSERT(IsGreaterThanZeroFloat(Wavelength) != FALSE);
    ASSERT(Intensity != NULL);

    *Intensity = (FLOAT) 0.0;

    return ISTATUS_SUCCESS; 
}

//
// Static Variables
//

CONST STATIC SPECTRUM_VTABLE FmaSpectrumVTable = {
    FmaSpectrumSample,
    NULL
};

CONST STATIC SPECTRUM_VTABLE AttenuatedSpectrumVTable = {
    AttenuatedSpectrumSample,
    NULL
};

CONST STATIC SPECTRUM_VTABLE SumSpectrumVTable = {
    SumSpectrumSample,
    NULL
};

CONST STATIC SPECTRUM_VTABLE ReflectionSpectrumVTable = {
    ReflectionSpectrumSample,
    NULL
};

CONST STATIC SPECTRUM_VTABLE AttenuatedReflectionSpectrumVTable = {
    AttenuatedReflectionSpectrumSample,
    NULL
};

//
// Initialization Functions
//

STATIC
VOID
FmaSpectrumInitialize(
    _Out_ PFMA_SPECTRUM FmaSpectrum,
    _In_ PCSPECTRUM Spectrum0,
    _In_ PCSPECTRUM Spectrum1,
    _In_ FLOAT Attenuation
    )
{
    ASSERT(FmaSpectrum != NULL);
    ASSERT(Spectrum0 != NULL);
    ASSERT(Spectrum1 != NULL);
    ASSERT(IsFiniteFloat(Attenuation) != FALSE);
    ASSERT(IsNotZeroFloat(Attenuation) != FALSE);

    SpectrumInitialize(&FmaSpectrumVTable,
                       FmaSpectrum,
                       &FmaSpectrum->SpectrumHeader);
                                
    FmaSpectrum->Spectrum0 = Spectrum0;
    FmaSpectrum->Spectrum1 = Spectrum1;
    FmaSpectrum->Attenuation = Attenuation;
}

STATIC
VOID
AttenuatedSpectrumInitialize(
    _Out_ PATTENUATED_SPECTRUM AttenuatedSpectrum,
    _In_ PCSPECTRUM Spectrum,
    _In_ FLOAT Attenuation
    )
{
    ASSERT(AttenuatedSpectrum != NULL);
    ASSERT(Spectrum != NULL);
    ASSERT(IsFiniteFloat(Attenuation) != FALSE);
    ASSERT(IsNotZeroFloat(Attenuation) != FALSE);

    SpectrumInitialize(&AttenuatedSpectrumVTable,
                       AttenuatedSpectrum,
                       &AttenuatedSpectrum->SpectrumHeader);
                                
    AttenuatedSpectrum->Spectrum = Spectrum;
    AttenuatedSpectrum->Attenuation = Attenuation;
}

STATIC
VOID
SumSpectrumInitialize(
    _Out_ PSUM_SPECTRUM SumSpectrum,
    _In_ PCSPECTRUM Spectrum0,
    _In_ PCSPECTRUM Spectrum1
    )
{
    ASSERT(SumSpectrum != NULL);
    ASSERT(Spectrum0 != NULL);
    ASSERT(Spectrum1 != NULL);

    SpectrumInitialize(&SumSpectrumVTable,
                       SumSpectrum,
                       &SumSpectrum->SpectrumHeader);
                                
    SumSpectrum->Spectrum0 = Spectrum0;
    SumSpectrum->Spectrum1 = Spectrum1;
}

STATIC
VOID
ReflectionSpectrumInitialize(
    _Out_ PREFLECTION_SPECTRUM ReflectionSpectrum,
    _In_ PCSPECTRUM Spectrum,
    _In_ PCREFLECTOR Reflector
    )
{
    ASSERT(ReflectionSpectrum != NULL);
    ASSERT(Spectrum != NULL);
    ASSERT(Reflector != NULL);

    SpectrumInitialize(&ReflectionSpectrumVTable,
                       ReflectionSpectrum,
                       &ReflectionSpectrum->SpectrumHeader);
                                
    ReflectionSpectrum->Spectrum = Spectrum;
    ReflectionSpectrum->Reflector = Reflector;
}

STATIC
VOID
AttenuatedReflectionSpectrumInitialize(
    _Out_ PATTENUATED_REFLECTION_SPECTRUM AttenuatedReflectionSpectrum,
    _In_ PCSPECTRUM Spectrum,
    _In_ PCREFLECTOR Reflector,
    _In_ FLOAT Attenuation
    )
{
    ASSERT(AttenuatedReflectionSpectrum != NULL);
    ASSERT(Spectrum != NULL);
    ASSERT(Reflector != NULL);
    ASSERT(IsFiniteFloat(Attenuation) != FALSE);
    ASSERT(IsNotZeroFloat(Attenuation) != FALSE);

    SpectrumInitialize(&ReflectionSpectrumVTable,
                       AttenuatedReflectionSpectrum,
                       &AttenuatedReflectionSpectrum->SpectrumHeader);
                                
    AttenuatedReflectionSpectrum->Spectrum = Spectrum;
    AttenuatedReflectionSpectrum->Reflector = Reflector;
    AttenuatedReflectionSpectrum->Attenuation = Attenuation;
}

//
// Static Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS
SpectrumCompositorReferenceInitialize(
    _Out_ PSPECTRUM_COMPOSITOR_REFERENCE Compositor
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

    Status = StaticMemoryAllocatorInitialize(&Compositor->AttenuatedReflectionSpectrumAllocator,
                                             sizeof(ATTENUATED_REFLECTION_SPECTRUM));

    if (Status != ISTATUS_SUCCESS)
    {
        StaticMemoryAllocatorDestroy(&Compositor->SumSpectrumAllocator);
        StaticMemoryAllocatorDestroy(&Compositor->ReflectionSpectrumAllocator);
        StaticMemoryAllocatorDestroy(&Compositor->AttenuatedSpectrumAllocator);
        StaticMemoryAllocatorDestroy(&Compositor->FmaSpectrumAllocator);
        return Status;
    }

    return ISTATUS_SUCCESS;
}

SFORCEINLINE
VOID
SpectrumCompositorReferenceClear(
    _Inout_ PSPECTRUM_COMPOSITOR_REFERENCE Compositor
    )
{
    ASSERT(Compositor != NULL);

    StaticMemoryAllocatorFreeAll(&Compositor->AttenuatedReflectionSpectrumAllocator);
    StaticMemoryAllocatorFreeAll(&Compositor->ReflectionSpectrumAllocator);
    StaticMemoryAllocatorFreeAll(&Compositor->AttenuatedSpectrumAllocator);
    StaticMemoryAllocatorFreeAll(&Compositor->FmaSpectrumAllocator);
    StaticMemoryAllocatorFreeAll(&Compositor->SumSpectrumAllocator);
}

SFORCEINLINE
VOID
SpectrumCompositorReferenceDestroy(
    _In_ _Post_invalid_ PSPECTRUM_COMPOSITOR_REFERENCE Compositor
    )
{
    ASSERT(Compositor != NULL);

    StaticMemoryAllocatorDestroy(&Compositor->AttenuatedReflectionSpectrumAllocator);
    StaticMemoryAllocatorDestroy(&Compositor->ReflectionSpectrumAllocator);
    StaticMemoryAllocatorDestroy(&Compositor->AttenuatedSpectrumAllocator);
    StaticMemoryAllocatorDestroy(&Compositor->FmaSpectrumAllocator);
    StaticMemoryAllocatorDestroy(&Compositor->SumSpectrumAllocator);
}

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
SpectrumCompositorReferenceAddSpectra(
    _Inout_ PSPECTRUM_COMPOSITOR_REFERENCE Compositor,
    _In_opt_ PCSPECTRUM Spectrum0,
    _In_opt_ PCSPECTRUM Spectrum1,
    _Out_ PCSPECTRUM *Sum
    )
{
    PATTENUATED_SPECTRUM AttenuatedSpectrum;
    PSUM_SPECTRUM SumSpectrum;
    PVOID Allocation;
    ISTATUS Status;

    if (Compositor == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (Sum == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    if (Spectrum0 == NULL)
    {
        *Sum = Spectrum1;
        return ISTATUS_SUCCESS;
    }

    if (Spectrum1 == NULL)
    {
        *Sum = Spectrum0;
        return ISTATUS_SUCCESS;
    }

    if (Spectrum0->VTable == &AttenuatedSpectrumVTable)
    {
        AttenuatedSpectrum = (PATTENUATED_SPECTRUM) Spectrum0;

        Status = SpectrumCompositorReferenceAttenuatedAddSpectra(Compositor,
                                                                 Spectrum1,
                                                                 AttenuatedSpectrum->Spectrum,
                                                                 AttenuatedSpectrum->Attenuation,
                                                                 Sum);
        return Status;
    }

    if (Spectrum1->VTable == &AttenuatedSpectrumVTable)
    {
        AttenuatedSpectrum = (PATTENUATED_SPECTRUM) Spectrum1;

        Status = SpectrumCompositorReferenceAttenuatedAddSpectra(Compositor,
                                                                 Spectrum0,
                                                                 AttenuatedSpectrum->Spectrum,
                                                                 AttenuatedSpectrum->Attenuation,
                                                                 Sum);
                                                                   
        return Status;
    }

    Allocation = StaticMemoryAllocatorAllocate(&Compositor->SumSpectrumAllocator);

    if (Allocation == NULL)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    SumSpectrum = (PSUM_SPECTRUM) Allocation;

    SumSpectrumInitialize(SumSpectrum,
                          Spectrum0,
                          Spectrum1);

    *Sum = (PCSPECTRUM) SumSpectrum;
    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
SpectrumCompositorReferenceAttenuateSpectrum(
    _Inout_ PSPECTRUM_COMPOSITOR_REFERENCE Compositor,
    _In_opt_ PCSPECTRUM Spectrum,
    _In_ FLOAT Attenuation,
    _Out_ PCSPECTRUM *AttenuatedSpectrumOutput
    )
{
    PATTENUATED_SPECTRUM AttenuatedSpectrum;
    PATTENUATED_SPECTRUM SpectrumAsAttenuatedSpectrum;
    PVOID Allocation;

    if (Compositor == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if(IsFiniteFloat(Attenuation) == FALSE);
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (AttenuatedSpectrumOutput == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    if (Spectrum == NULL ||
        IsNotZeroFloat(Attenuation) == FALSE)
    {
        *AttenuatedSpectrumOutput = NULL;
        return ISTATUS_SUCCESS;
    }

    if (Attenuation == (FLOAT) 1.0)
    {
        *AttenuatedSpectrumOutput = Spectrum;
        return ISTATUS_SUCCESS;
    }

    if (Spectrum->VTable == &AttenuatedSpectrumVTable)
    {
        SpectrumAsAttenuatedSpectrum = (PATTENUATED_SPECTRUM) Spectrum;
        Spectrum = SpectrumAsAttenuatedSpectrum->Spectrum;
        Attenuation *= SpectrumAsAttenuatedSpectrum->Attenuation;
    }

    Allocation = StaticMemoryAllocatorAllocate(&Compositor->AttenuatedSpectrumAllocator);

    if (Allocation == NULL)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    AttenuatedSpectrum = (PATTENUATED_SPECTRUM) Allocation;

    AttenuatedSpectrumInitialize(AttenuatedSpectrum,
                                 Spectrum,
                                 Attenuation);

    *AttenuatedSpectrumOutput = (PCSPECTRUM) AttenuatedSpectrum;
    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
SpectrumCompositorReferenceAttenuatedAddSpectra(
    _Inout_ PSPECTRUM_COMPOSITOR_REFERENCE Compositor,
    _In_opt_ PCSPECTRUM Spectrum0,
    _In_opt_ PCSPECTRUM Spectrum1,
    _In_ FLOAT Attenuation,
    _Out_ PCSPECTRUM *AttenuatedSum
    )
{
    PATTENUATED_SPECTRUM AttenuatedSpectrum;
    PFMA_SPECTRUM FmaSpectrum;
    PVOID Allocation;
    ISTATUS Status;

    if (Compositor == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if(IsFiniteFloat(Attenuation) == FALSE);
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    if (AttenuatedSum == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    if (Spectrum0 == NULL)
    {
        Status = SpectrumCompositorReferenceAttenuateSpectrum(Compositor,
                                                              Spectrum1,
                                                              Attenuation,
                                                              AttenuatedSum);
                                                              
        return Status;
    }

    if (Spectrum1 == NULL ||
        IsNotZeroFloat(Attenuation) == FALSE)
    {
        *AttenuatedSum = Spectrum0;
        return ISTATUS_SUCCESS;
    }

    if (Attenuation == (FLOAT) 1.0)
    {
        Status = SpectrumCompositorReferenceAddSpectra(Compositor,
                                                       Spectrum0,
                                                       Spectrum1,
                                                       AttenuatedSum);   
        return Status;
    }

    if (Spectrum1->VTable == &AttenuatedSpectrumVTable)
    {
        AttenuatedSpectrum = (PATTENUATED_SPECTRUM) Spectrum1;
        Spectrum1 = AttenuatedSpectrum->Spectrum;
        Attenuation *= AttenuatedSpectrum->Attenuation;
    }

    Allocation = StaticMemoryAllocatorAllocate(&Compositor->FmaSpectrumAllocator);

    if (Allocation == NULL)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    FmaSpectrum = (PFMA_SPECTRUM) Allocation;

    FmaSpectrumInitialize(FmaSpectrum,
                          Spectrum0,
                          Spectrum1,
                          Attenuation);

    *AttenuatedSum = (PCSPECTRUM) FmaSpectrum;
    return ISTATUS_SUCCESS;   
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
SpectrumCompositorReferenceAddReflection(
    _Inout_ PSPECTRUM_COMPOSITOR_REFERENCE Compositor,
    _In_opt_ PCSPECTRUM Spectrum,
    _In_opt_ PCREFLECTOR Reflector,
    _Out_ PCSPECTRUM *ReflectedSpectrum
    )
{
    PREFLECTION_SPECTRUM ReflectionSpectrum;
    PVOID Allocation;

    if (Compositor == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (Spectrum == NULL ||
        Reflector == NULL)
    {
        *ReflectedSpectrum = NULL;
        return ISTATUS_SUCCESS;
    }

    Allocation = StaticMemoryAllocatorAllocate(&Compositor->ReflectionSpectrumAllocator);

    if (Allocation == NULL)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    ReflectionSpectrum = (PREFLECTION_SPECTRUM) Allocation;

    ReflectionSpectrumInitialize(ReflectionSpectrum,
                                 Spectrum,
                                 Reflector);

    *ReflectedSpectrum = (PCSPECTRUM) ReflectionSpectrum;
    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
SpectrumCompositorReferenceAttenuatedAddReflection(
    _Inout_ PSPECTRUM_COMPOSITOR_REFERENCE Compositor,
    _In_opt_ PCSPECTRUM Spectrum,
    _In_opt_ PCREFLECTOR Reflector,
    _In_ FLOAT Attenuation,
    _Out_ PCSPECTRUM *ReflectedSpectrum
    )
{
    PATTENUATED_REFLECTION_SPECTRUM AttenuatedReflectionSpectrum;
    PVOID Allocation;
    ISTATUS Status;

    if (Compositor == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if(IsFiniteFloat(Attenuation) == FALSE);
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    if (Spectrum == NULL ||
        Reflector == NULL ||
        IsNotZeroFloat(Attenuation) == FALSE)
    {
        *ReflectedSpectrum = NULL;
        return ISTATUS_SUCCESS;
    }

    if (Attenuation == (FLOAT) 1.0f)
    {
        Status = SpectrumCompositorReferenceAddReflection(Compositor,
                                                          Spectrum,
                                                          Reflector,
                                                          ReflectedSpectrum);

        return Status;
    }

    Allocation = StaticMemoryAllocatorAllocate(&Compositor->AttenuatedReflectionSpectrumAllocator);

    if (Allocation == NULL)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    AttenuatedReflectionSpectrum = (PATTENUATED_REFLECTION_SPECTRUM) Allocation;

    AttenuatedReflectionSpectrumInitialize(AttenuatedReflectionSpectrum,
                                           Spectrum,
                                           Reflector,
                                           Attenuation);

    *ReflectedSpectrum = (PCSPECTRUM) AttenuatedReflectionSpectrum;
    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
SpectrumCompositorAllocate(
    _Out_ PSPECTRUM_COMPOSITOR *Result
    )
{
    PSPECTRUM_COMPOSITOR Compositor;
    ISTATUS Status;

    if (Result == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    Compositor = (PSPECTRUM_COMPOSITOR) malloc(sizeof(SPECTRUM_COMPOSITOR));

    if (Compositor == NULL)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    Status = SpectrumCompositorReferenceInitialize(&Compositor->CompositorReference);

    if (Status != ISTATUS_SUCCESS)
    {
        free(Compositor);
        return Status;
    }

    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
SpectrumCompositorAddSpectra(
    _Inout_ PSPECTRUM_COMPOSITOR Compositor,
    _In_opt_ PCSPECTRUM Spectrum0,
    _In_opt_ PCSPECTRUM Spectrum1,
    _Out_ PCSPECTRUM *Sum
    )
{
    ISTATUS Status;
    
    //
    // &Compositor->CompositorReference should be safe to do even if
    // Compositor == NULL.
    //
    
    Status = SpectrumCompositorReferenceAddSpectra(&Compositor->CompositorReference,
                                                   Spectrum0,
                                                   Spectrum1,
                                                   Sum);
    
    return Status;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
SpectrumCompositorAttenuateSpectrum(
    _Inout_ PSPECTRUM_COMPOSITOR Compositor,
    _In_opt_ PCSPECTRUM Spectrum,
    _In_ FLOAT Attenuation,
    _Out_ PCSPECTRUM *AttenuatedSpectrum
    )
{
    ISTATUS Status;
    
    //
    // &Compositor->CompositorReference should be safe to do even if
    // Compositor == NULL.
    //
    
    Status = SpectrumCompositorReferenceAttenuateSpectrum(&Compositor->CompositorReference,
                                                          Spectrum,
                                                          Attenuation,
                                                          AttenuatedSpectrum);
    
    return Status;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
SpectrumCompositorAttenuatedAddSpectra(
    _Inout_ PSPECTRUM_COMPOSITOR Compositor,
    _In_opt_ PCSPECTRUM Spectrum0,
    _In_opt_ PCSPECTRUM Spectrum1,
    _In_ FLOAT Attenuation,
    _Out_ PCSPECTRUM *AttenuatedSum
    )
{
    ISTATUS Status;
    
    //
    // &Compositor->CompositorReference should be safe to do even if
    // Compositor == NULL.
    //
    
    Status = SpectrumCompositorReferenceAttenuatedAddSpectra(&Compositor->CompositorReference,
                                                             Spectrum0,
                                                             Spectrum1,
                                                             Attenuation,
                                                             AttenuatedSum);
    
    return Status;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
SpectrumCompositorAddReflection(
    _Inout_ PSPECTRUM_COMPOSITOR Compositor,
    _In_opt_ PCSPECTRUM Spectrum,
    _In_opt_ PCREFLECTOR Reflector,
    _Out_ PCSPECTRUM *ReflectedSpectrum
    )
{
    ISTATUS Status;
    
    //
    // &Compositor->CompositorReference should be safe to do even if
    // Compositor == NULL.
    //
    
    Status = SpectrumCompositorReferenceAddReflection(&Compositor->CompositorReference,
                                                      Spectrum,
                                                      Reflector,
                                                      ReflectedSpectrum);
    
    return Status;
}
    
_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
SpectrumCompositorAttenuatedAddReflection(
    _Inout_ PSPECTRUM_COMPOSITOR Compositor,
    _In_opt_ PCSPECTRUM Spectrum,
    _In_opt_ PCREFLECTOR Reflector,
    _In_ FLOAT Attenuation,
    _Out_ PCSPECTRUM *ReflectedSpectrum
    )
{
    ISTATUS Status;
    
    //
    // &Compositor->CompositorReference should be safe to do even if
    // Compositor == NULL.
    //
    
    Status = SpectrumCompositorReferenceAttenuatedAddReflection(&Compositor->CompositorReference,
                                                                Spectrum,
                                                                Reflector,
                                                                Attenuation,
                                                                ReflectedSpectrum);
    
    return Status;
}

_Ret_
PSPECTRUM_COMPOSITOR_REFERENCE
SpectrumCompositorGetSpectrumCompositorReference(
    _In_ PSPECTRUM_COMPOSITOR Compositor
    )
{
    if (Compositor == NULL)
    {
        return NULL;
    }

    return &Compositor->CompositorReference;
}

VOID
SpectrumCompositorClear(
    _Inout_ PSPECTRUM_COMPOSITOR Compositor
    )
{
    if (Compositor == NULL)
    {
        return;
    }

    SpectrumCompositorReferenceClear(&Compositor->CompositorReference);
}

VOID
SpectrumCompositorFree(
    _In_opt_ _Post_invalid_ PSPECTRUM_COMPOSITOR Compositor
    )
{
    if (Compositor == NULL)
    {
        return;
    }

    SpectrumCompositorReferenceDestroy(&Compositor->CompositorReference);
}