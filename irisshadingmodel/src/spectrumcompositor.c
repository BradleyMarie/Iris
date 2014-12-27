/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    spectrumcompositor.c

Abstract:

    This file contains the function definitions 
    for the SPECTRUM_COMPOSITOR type.

--*/

#include <irisshadingmodelp.h>

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
    FLOAT Temp;
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

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISSHADINGMODELAPI
ISTATUS
SpectrumCompositorAddSpectrums(
    _Inout_ PSPECTRUM_COMPOSITOR Compositor,
    _In_ PCSPECTRUM Spectrum0,
    _In_ PCSPECTRUM Spectrum1,
    _Out_ PSPECTRUM *Sum
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISSHADINGMODELAPI
ISTATUS
SpectrumCompositorAttenuateSpectrum(
    _Inout_ PSPECTRUM_COMPOSITOR Compositor,
    _In_ PCSPECTRUM Spectrum,
    _Out_ PSPECTRUM *AttenuatedSpectrum
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISSHADINGMODELAPI
ISTATUS
SpectrumCompositorAttenuatedAddSpectrums(
    _Inout_ PSPECTRUM_COMPOSITOR Compositor,
    _In_ PCSPECTRUM Spectrum0,
    _In_ PCSPECTRUM Spectrum1,
    _In_ FLOAT Attenuation,
    _Out_ PSPECTRUM *AttenuatedSum
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISSHADINGMODELAPI
ISTATUS
SpectrumCompositorAddReflection(
    _Inout_ PSPECTRUM_COMPOSITOR Compositor,
    _In_ PCSPECTRUM Spectrum,
    _In_ PCREFLECTOR Reflector,
    _Out_ PSPECTRUM *Sum
    );

VOID
SpectrumDereference(
    _In_opt_ _Post_invalid_ PSPECTRUM Spectrum
    )
{
    PSPECTRUM_SAMPLE_ROUTINE SampleRoutine;
    PFREE_ROUTINE FreeRoutine;

    if (Spectrum == NULL)
    {
        return;
    }

    Spectrum->ReferenceCount -= 1;

    if (Spectrum->ReferenceCount == 0)
    {
        SampleRoutine = Spectrum->VTable->SampleRoutine;

        //
        // Spectrums allocated by a SpectrumCompositor were not allocated
        // on the general heap, so the rest of the dealloc code does not
        // apply to them.
        //

        if (SampleRoutine == FmaSpectrumSample ||
            SampleRoutine == SumSpectrumSample ||
            SampleRoutine == AttenuatedSpectrumSample ||
            SampleRoutine == ReflectionSpectrumSample)
        {
            return;
        }

        FreeRoutine = Spectrum->VTable->FreeRoutine;

        if (FreeRoutine != NULL)
        {
            FreeRoutine(Spectrum->Data);
        }

        IrisAlignedFree(Spectrum);
    }
}