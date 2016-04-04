/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    pointlight.h

Abstract:

    This file contains the definitions for SPECTRUM_POINT_LIGHT type.

--*/

#include <irisphysxtoolkitp.h>

//
// Types
//

typedef struct _SPECTRUM_POINT_LIGHT {
    PSPECTRUM Intensity;
    POINT3 Location;
} SPECTRUM_POINT_LIGHT, *PSPECTRUM_POINT_LIGHT;

typedef CONST SPECTRUM_POINT_LIGHT *PCSPECTRUM_POINT_LIGHT;

//
// Static Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
STATIC
ISTATUS
SpectrumPointLightSample(
    _In_ PCVOID Context,
    _In_ POINT3 HitPoint,
    _Inout_ PPBR_VISIBILITY_TESTER Tester,
    _Inout_ PRANDOM_REFERENCE Rng,
    _Inout_ PSPECTRUM_COMPOSITOR_REFERENCE Compositor,
    _Out_ PCSPECTRUM *Spectrum,
    _Out_ PVECTOR3 ToLight,
    _Out_ PFLOAT Pdf
    )
{
    PCSPECTRUM_POINT_LIGHT Light;
    FLOAT DistanceToLightSquared;
    VECTOR3 DirectionToLight;
    FLOAT DistanceToLight;
    FLOAT Attenuation;
    RAY RayToLight;
    ISTATUS Status;
    BOOL Visible;

    ASSERT(Context != NULL);
    ASSERT(PointValidate(HitPoint) != FALSE);
    ASSERT(Tester != NULL);
    ASSERT(Rng != NULL);
    ASSERT(Compositor != NULL);
    ASSERT(Spectrum != NULL);
    ASSERT(ToLight != NULL);
    ASSERT(Pdf != NULL);

    Light = (PCSPECTRUM_POINT_LIGHT) Context;

    DirectionToLight = PointSubtract(Light->Location, HitPoint);

    DirectionToLight = VectorNormalize(DirectionToLight,
                                       &DistanceToLightSquared,
                                       &DistanceToLight);

    RayToLight = RayCreate(HitPoint, DirectionToLight);

    Status = PBRVisibilityTesterTestVisibility(Tester,
                                               RayToLight,
                                               DistanceToLight,
                                               &Visible);

    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }

    if (Visible == FALSE)
    {
        *Spectrum = NULL;
        *ToLight = DirectionToLight;
        *Pdf = (FLOAT) 0.0f;
        return ISTATUS_SUCCESS;
    }

    Attenuation = (FLOAT) 1.0f / DistanceToLightSquared;

    Status = SpectrumCompositorReferenceAttenuateSpectrum(Compositor,
                                                          Light->Intensity,
                                                          Attenuation,
                                                          Spectrum);

    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }
    
    *ToLight = DirectionToLight;
    *Pdf = (FLOAT) INFINITY;

    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
STATIC
ISTATUS
SpectrumPointLightComputeEmissive(
    _In_ PCVOID Context,
    _In_ RAY ToLight,
    _Inout_ PPBR_VISIBILITY_TESTER Tester,
    _Out_ PCSPECTRUM *Spectrum
    )
{
    ASSERT(Context != NULL);
    ASSERT(RayValidate(ToLight) != FALSE);
    ASSERT(Tester != NULL);
    ASSERT(Spectrum != NULL);

    *Spectrum = NULL;

    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
STATIC
ISTATUS
SpectrumPointLightComputeEmissiveWithPdf(
    _In_ PCVOID Context,
    _In_ RAY ToLight,
    _Inout_ PPBR_VISIBILITY_TESTER Tester,
    _Out_ PCSPECTRUM *Spectrum,
    _Out_ PFLOAT Pdf
    )
{
    ASSERT(Context != NULL);
    ASSERT(RayValidate(ToLight) != FALSE);
    ASSERT(Tester != NULL);
    ASSERT(Spectrum != NULL);
    ASSERT(Pdf != NULL);

    *Spectrum = NULL;
    *Pdf = (FLOAT) 0.0f;

    return ISTATUS_SUCCESS;
}

STATIC
VOID
SpectrumPointLightFree(
    _In_ PCVOID Context
    )
{
    PCSPECTRUM_POINT_LIGHT Light;

    ASSERT(Context != NULL);

    Light = (PCSPECTRUM_POINT_LIGHT) Context;

    SpectrumRelease(Light->Intensity);
}

//
// Static Variables
//

CONST STATIC PBR_LIGHT_VTABLE PointLightVTable = {
    SpectrumPointLightSample,
    SpectrumPointLightComputeEmissive,
    SpectrumPointLightComputeEmissiveWithPdf,
    SpectrumPointLightFree
};

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
SpectrumPointLightAllocate(
    _In_ PSPECTRUM Intensity,
    _In_ POINT3 WorldLocation,
    _Out_ PPBR_LIGHT *Light
    )
{
    SPECTRUM_POINT_LIGHT LightData;
    ISTATUS Status;

    LightData.Location = WorldLocation;
    LightData.Intensity = Intensity;

    Status = PbrLightAllocate(&PointLightVTable,
                              &LightData,
                              sizeof(SPECTRUM_POINT_LIGHT),
                              _Alignof(SPECTRUM_POINT_LIGHT),
                              Light);

    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }

    SpectrumRetain(Intensity);

    return ISTATUS_SUCCESS;
}