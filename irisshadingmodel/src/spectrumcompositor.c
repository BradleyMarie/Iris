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