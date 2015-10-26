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

struct _SPECTRUM_COMPOSITOR {
    STATIC_MEMORY_ALLOCATOR ReflectionSpectrumAllocator;
    STATIC_MEMORY_ALLOCATOR AttenuatedSpectrumAllocator;
    STATIC_MEMORY_ALLOCATOR FmaSpectrumAllocator;
    STATIC_MEMORY_ALLOCATOR SumSpectrumAllocator;
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
    ASSERT(IsNormalFloat(Wavelength) != FALSE);
    ASSERT(IsFiniteFloat(Wavelength) != FALSE);
    ASSERT(IsPositiveFloat(Wavelength) != FALSE);
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
    ASSERT(IsNormalFloat(Wavelength) != FALSE);
    ASSERT(IsFiniteFloat(Wavelength) != FALSE);
    ASSERT(IsPositiveFloat(Wavelength) != FALSE);
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
    ASSERT(IsNormalFloat(Wavelength) != FALSE);
    ASSERT(IsFiniteFloat(Wavelength) != FALSE);
    ASSERT(IsPositiveFloat(Wavelength) != FALSE);
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
    ASSERT(IsNormalFloat(Wavelength) != FALSE);
    ASSERT(IsFiniteFloat(Wavelength) != FALSE);
    ASSERT(IsPositiveFloat(Wavelength) != FALSE);
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
ZeroSpectrumSample(
    _In_ PCVOID Context,
    _In_ FLOAT Wavelength,
    _Out_ PFLOAT Intensity
    )
{
    ASSERT(Context == NULL);
    ASSERT(IsNormalFloat(Wavelength) != FALSE);
    ASSERT(IsFiniteFloat(Wavelength) != FALSE);
    ASSERT(IsPositiveFloat(Wavelength) != FALSE);
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
    ASSERT(IsNormalFloat(Attenuation) != FALSE);
    ASSERT(IsFiniteFloat(Attenuation) != FALSE);
    ASSERT(IsZeroFloat(Attenuation) == FALSE);

    FmaSpectrum->SpectrumHeader.VTable = &FmaSpectrumVTable;
    FmaSpectrum->SpectrumHeader.ReferenceCount = 0;
    FmaSpectrum->SpectrumHeader.Data = FmaSpectrum;
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
    ASSERT(IsNormalFloat(Attenuation) != FALSE);
    ASSERT(IsFiniteFloat(Attenuation) != FALSE);
    ASSERT(IsZeroFloat(Attenuation) == FALSE);

    AttenuatedSpectrum->SpectrumHeader.VTable = &AttenuatedSpectrumVTable;
    AttenuatedSpectrum->SpectrumHeader.ReferenceCount = 0;
    AttenuatedSpectrum->SpectrumHeader.Data = AttenuatedSpectrum;
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

    SumSpectrum->SpectrumHeader.VTable = &SumSpectrumVTable;
    SumSpectrum->SpectrumHeader.ReferenceCount = 0;
    SumSpectrum->SpectrumHeader.Data = SumSpectrum;
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

    ReflectionSpectrum->SpectrumHeader.VTable = &ReflectionSpectrumVTable;
    ReflectionSpectrum->SpectrumHeader.ReferenceCount = 0;
    ReflectionSpectrum->SpectrumHeader.Data = ReflectionSpectrum;
    ReflectionSpectrum->Spectrum = Spectrum;
    ReflectionSpectrum->Reflector = Reflector;
}

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
SpectrumCompositorAddSpectrums(
    _Inout_ PSPECTRUM_COMPOSITOR Compositor,
    _In_opt_ PCSPECTRUM Spectrum0,
    _In_opt_ PCSPECTRUM Spectrum1,
    _Out_ PCSPECTRUM *Sum
    )
{
    PATTENUATED_SPECTRUM AttenuatedSpectrum;
    PSUM_SPECTRUM SumSpectrum;
    PVOID Allocation;

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

        return SpectrumCompositorAttenuatedAddSpectrums(Compositor,
                                                        Spectrum1,
                                                        AttenuatedSpectrum->Spectrum,
                                                        AttenuatedSpectrum->Attenuation,
                                                        Sum);
    }

    if (Spectrum1->VTable == &AttenuatedSpectrumVTable)
    {
        AttenuatedSpectrum = (PATTENUATED_SPECTRUM) Spectrum1;

        return SpectrumCompositorAttenuatedAddSpectrums(Compositor,
                                                        Spectrum0,
                                                        AttenuatedSpectrum->Spectrum,
                                                        AttenuatedSpectrum->Attenuation,
                                                        Sum);
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
SpectrumCompositorAttenuateSpectrum(
    _Inout_ PSPECTRUM_COMPOSITOR Compositor,
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

    if(IsNormalFloat(Attenuation) != FALSE ||
       IsFiniteFloat(Attenuation) != FALSE);
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (AttenuatedSpectrumOutput == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    if (Spectrum == NULL ||
        IsZeroFloat(Attenuation) != FALSE)
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
SpectrumCompositorAttenuatedAddSpectrums(
    _Inout_ PSPECTRUM_COMPOSITOR Compositor,
    _In_opt_ PCSPECTRUM Spectrum0,
    _In_opt_ PCSPECTRUM Spectrum1,
    _In_ FLOAT Attenuation,
    _Out_ PCSPECTRUM *AttenuatedSum
    )
{
    PATTENUATED_SPECTRUM AttenuatedSpectrum;
    PFMA_SPECTRUM FmaSpectrum;
    PVOID Allocation;

    if (Compositor == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if(IsNormalFloat(Attenuation) != FALSE ||
       IsFiniteFloat(Attenuation) != FALSE);
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    if (AttenuatedSum == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    if (Spectrum0 == NULL)
    {
        return SpectrumCompositorAttenuateSpectrum(Compositor,
                                                   Spectrum1,
                                                   Attenuation,
                                                   AttenuatedSum);
    }

    if (Spectrum1 == NULL ||
        IsZeroFloat(Attenuation) != FALSE)
    {
        *AttenuatedSum = Spectrum0;
        return ISTATUS_SUCCESS;
    }

    if (Attenuation == (FLOAT) 1.0)
    {
        return SpectrumCompositorAddSpectrums(Compositor,
                                              Spectrum0,
                                              Spectrum1,
                                              AttenuatedSum);   
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
SpectrumCompositorAddReflection(
    _Inout_ PSPECTRUM_COMPOSITOR Compositor,
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
_Ret_maybenull_
PSPECTRUM_COMPOSITOR
SpectrumCompositorAllocate(
    VOID
    )
{
    PSPECTRUM_COMPOSITOR Compositor;
    ISTATUS Status;
    
    Compositor = (PSPECTRUM_COMPOSITOR) malloc(sizeof(SPECTRUM_COMPOSITOR));
    
    if (Compositor == NULL)
    {
        return NULL;
    }

    Status = StaticMemoryAllocatorInitialize(&Compositor->ReflectionSpectrumAllocator,
                                             sizeof(REFLECTION_SPECTRUM));

    if (Status != ISTATUS_SUCCESS)
    {
        return NULL;
    }

    Status = StaticMemoryAllocatorInitialize(&Compositor->AttenuatedSpectrumAllocator,
                                             sizeof(ATTENUATED_SPECTRUM));

    if (Status != ISTATUS_SUCCESS)
    {
        StaticMemoryAllocatorDestroy(&Compositor->ReflectionSpectrumAllocator);
        return NULL;
    }

    Status = StaticMemoryAllocatorInitialize(&Compositor->FmaSpectrumAllocator,
                                             sizeof(FMA_SPECTRUM));

    if (Status != ISTATUS_SUCCESS)
    {
        StaticMemoryAllocatorDestroy(&Compositor->ReflectionSpectrumAllocator);
        StaticMemoryAllocatorDestroy(&Compositor->AttenuatedSpectrumAllocator);
        return NULL;
    }

    Status = StaticMemoryAllocatorInitialize(&Compositor->SumSpectrumAllocator,
                                             sizeof(SUM_SPECTRUM));

    if (Status != ISTATUS_SUCCESS)
    {
        StaticMemoryAllocatorDestroy(&Compositor->ReflectionSpectrumAllocator);
        StaticMemoryAllocatorDestroy(&Compositor->AttenuatedSpectrumAllocator);
        StaticMemoryAllocatorDestroy(&Compositor->FmaSpectrumAllocator);
        return NULL;
    }

    return Compositor;    
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
SpectrumCompositorClear(
    _Inout_ PSPECTRUM_COMPOSITOR Compositor
    )
{
    if (Compositor == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }
    
    StaticMemoryAllocatorFreeAll(&Compositor->ReflectionSpectrumAllocator);
    StaticMemoryAllocatorFreeAll(&Compositor->AttenuatedSpectrumAllocator);
    StaticMemoryAllocatorFreeAll(&Compositor->FmaSpectrumAllocator);
    StaticMemoryAllocatorFreeAll(&Compositor->SumSpectrumAllocator);
    
    return ISTATUS_SUCCESS;
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
    
    StaticMemoryAllocatorDestroy(&Compositor->ReflectionSpectrumAllocator);
    StaticMemoryAllocatorDestroy(&Compositor->AttenuatedSpectrumAllocator);
    StaticMemoryAllocatorDestroy(&Compositor->FmaSpectrumAllocator);
    StaticMemoryAllocatorDestroy(&Compositor->SumSpectrumAllocator);
    
    free(Compositor);
}