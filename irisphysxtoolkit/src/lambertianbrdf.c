/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    lambertianbrdf.h

Abstract:

    This file contains the definitions for SPECRUM_LAMBERTIAN_BRDF type.

--*/

#include <irisphysxtoolkitp.h>

//
// Types
//

typedef struct _SPECRUM_LAMBERTIAN_BRDF {
    PCREFLECTOR Reflectance;
    VECTOR3 SurfaceNormal;
} SPECRUM_LAMBERTIAN_BRDF, *PSPECRUM_LAMBERTIAN_BRDF;

typedef CONST SPECRUM_LAMBERTIAN_BRDF *PCSPECRUM_LAMBERTIAN_BRDF;

//
// Static Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
STATIC
ISTATUS
SpectrumLambertianBrdfSample(
    _In_ PCVOID Context,
    _In_ VECTOR3 Incoming,
    _Inout_ PRANDOM_REFERENCE Rng,
    _Inout_ PREFLECTOR_COMPOSITOR_REFERENCE Compositor,
    _Out_ PCREFLECTOR *Reflector,
    _Out_ PVECTOR3 Outgoing,
    _Out_ PFLOAT Pdf
    )
{
    PCSPECRUM_LAMBERTIAN_BRDF Brdf;
    ISTATUS Status;

    ASSERT(Context != NULL);
    ASSERT(VectorValidate(Incoming) != FALSE);
    ASSERT(Rng != NULL);
    ASSERT(Compositor != NULL);
    ASSERT(Reflector != NULL);
    ASSERT(Outgoing != NULL);
    ASSERT(Pdf != NULL);

    Brdf = (PCSPECRUM_LAMBERTIAN_BRDF) Context;

    Status = IrisAdvancedCosineSampleHemisphere(Brdf->SurfaceNormal,
                                                Rng,
                                                Outgoing);

    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }

    Status = ReflectorCompositorReferenceAttenuateReflection(Compositor,
                                                             Brdf->Reflectance,
                                                             IRIS_INV_PI,
                                                             Reflector);

    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }

    *Pdf = IRIS_INV_PI * VectorDotProduct(Brdf->SurfaceNormal, *Outgoing);

    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
STATIC
ISTATUS
SpectrumLambertianBrdfSampleWithLambertianFalloff(
    _In_ PCVOID Context,
    _In_ VECTOR3 Incoming,
    _Inout_ PRANDOM_REFERENCE Rng,
    _Inout_ PREFLECTOR_COMPOSITOR_REFERENCE Compositor,
    _Out_ PCREFLECTOR *Reflector,
    _Out_ PVECTOR3 Outgoing,
    _Out_ PFLOAT Pdf
    )
{
    FLOAT Attenuation;
    PCSPECRUM_LAMBERTIAN_BRDF Brdf;
    FLOAT DotProduct;
    ISTATUS Status;

    ASSERT(Context != NULL);
    ASSERT(VectorValidate(Incoming) != FALSE);
    ASSERT(Rng != NULL);
    ASSERT(Compositor != NULL);
    ASSERT(Reflector != NULL);
    ASSERT(Outgoing != NULL);
    ASSERT(Pdf != NULL);

    Brdf = (PCSPECRUM_LAMBERTIAN_BRDF)Context;

    Status = IrisAdvancedCosineSampleHemisphere(Brdf->SurfaceNormal,
                                                Rng,
                                                Outgoing);

    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }

    DotProduct = VectorDotProduct(Brdf->SurfaceNormal, *Outgoing);
    Attenuation = DotProduct * IRIS_INV_PI;

    Status = ReflectorCompositorReferenceAttenuateReflection(Compositor,
                                                             Brdf->Reflectance,
                                                             Attenuation,
                                                             Reflector);

    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }

    *Pdf = Attenuation;

    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
STATIC
ISTATUS
SpectrumLambertianBrdfComputeReflectance(
    _In_ PCVOID Context,
    _In_ VECTOR3 Incoming,
    _In_ VECTOR3 Outgoing,
    _Inout_ PREFLECTOR_COMPOSITOR_REFERENCE Compositor,
    _Out_ PCREFLECTOR *Reflector
    )
{
    PCSPECRUM_LAMBERTIAN_BRDF Brdf;
    ISTATUS Status;

    ASSERT(Context != NULL);
    ASSERT(VectorValidate(Incoming) != FALSE);
    ASSERT(VectorValidate(Outgoing) != FALSE);
    ASSERT(Compositor != NULL);
    ASSERT(Reflector != NULL);

    Brdf = (PCSPECRUM_LAMBERTIAN_BRDF) Context;

    Status = ReflectorCompositorReferenceAttenuateReflection(Compositor,
                                                             Brdf->Reflectance,
                                                             IRIS_INV_PI,
                                                             Reflector);

    return Status;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
STATIC
ISTATUS
SpectrumLambertianBrdfComputeReflectanceWithLambertianFalloff(
    _In_ PCVOID Context,
    _In_ VECTOR3 Incoming,
    _In_ VECTOR3 Outgoing,
    _Inout_ PREFLECTOR_COMPOSITOR_REFERENCE Compositor,
    _Out_ PCREFLECTOR *Reflector
    )
{
    FLOAT Attenuation;
    PCSPECRUM_LAMBERTIAN_BRDF Brdf;
    FLOAT DotProduct;
    ISTATUS Status;

    ASSERT(Context != NULL);
    ASSERT(VectorValidate(Incoming) != FALSE);
    ASSERT(VectorValidate(Outgoing) != FALSE);
    ASSERT(Compositor != NULL);
    ASSERT(Reflector != NULL);

    Brdf = (PCSPECRUM_LAMBERTIAN_BRDF) Context;
    
    DotProduct = VectorDotProduct(Brdf->SurfaceNormal, Outgoing);
    Attenuation = DotProduct * IRIS_INV_PI;

    Status = ReflectorCompositorReferenceAttenuateReflection(Compositor,
                                                             Brdf->Reflectance,
                                                             Attenuation,
                                                             Reflector);

    return Status;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
STATIC
ISTATUS
SpectrumLambertianBrdfComputeReflectanceWithPdf(
    _In_ PCVOID Context,
    _In_ VECTOR3 Incoming,
    _In_ VECTOR3 Outgoing,
    _Inout_ PREFLECTOR_COMPOSITOR_REFERENCE Compositor,
    _Out_ PCREFLECTOR *Reflector,
    _Out_ PFLOAT Pdf
    )
{
    PCSPECRUM_LAMBERTIAN_BRDF Brdf;
    ISTATUS Status;

    ASSERT(Context != NULL);
    ASSERT(VectorValidate(Incoming) != FALSE);
    ASSERT(VectorValidate(Outgoing) != FALSE);
    ASSERT(Compositor != NULL);
    ASSERT(Reflector != NULL);
    ASSERT(Pdf != NULL);

    Brdf = (PCSPECRUM_LAMBERTIAN_BRDF) Context;

    Status = ReflectorCompositorReferenceAttenuateReflection(Compositor,
                                                             Brdf->Reflectance,
                                                             IRIS_INV_PI,
                                                             Reflector);

    return Status;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
STATIC
ISTATUS
SpectrumLambertianBrdfComputeReflectanceWithPdfWithLambertianFalloff(
    _In_ PCVOID Context,
    _In_ VECTOR3 Incoming,
    _In_ VECTOR3 Outgoing,
    _Inout_ PREFLECTOR_COMPOSITOR_REFERENCE Compositor,
    _Out_ PCREFLECTOR *Reflector,
    _Out_ PFLOAT Pdf
    )
{
    FLOAT Attenuation;
    PCSPECRUM_LAMBERTIAN_BRDF Brdf;
    FLOAT DotProduct;
    ISTATUS Status;

    ASSERT(Context != NULL);
    ASSERT(VectorValidate(Incoming) != FALSE);
    ASSERT(VectorValidate(Outgoing) != FALSE);
    ASSERT(Compositor != NULL);
    ASSERT(Reflector != NULL);
    ASSERT(Pdf != NULL);

    Brdf = (PCSPECRUM_LAMBERTIAN_BRDF) Context;

    DotProduct = VectorDotProduct(Brdf->SurfaceNormal, Outgoing);
    Attenuation = DotProduct * IRIS_INV_PI;

    Status = ReflectorCompositorReferenceAttenuateReflection(Compositor,
                                                             Brdf->Reflectance,
                                                             Attenuation,
                                                             Reflector);

    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }

    *Pdf = Attenuation;

    return ISTATUS_SUCCESS;
}

//
// Static Variables
//

CONST STATIC PBR_BRDF_VTABLE SpectrumLambertianBrdfVTable = {
    SpectrumLambertianBrdfSample,
    SpectrumLambertianBrdfSampleWithLambertianFalloff,
    SpectrumLambertianBrdfComputeReflectance,
    SpectrumLambertianBrdfComputeReflectanceWithLambertianFalloff,
    SpectrumLambertianBrdfComputeReflectanceWithPdf,
    SpectrumLambertianBrdfComputeReflectanceWithPdfWithLambertianFalloff,
    NULL
};

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
SpectrumLambertianBrdfAllocate(
    _In_ PPBR_BRDF_ALLOCATOR Allocator,
    _In_ PCREFLECTOR Reflectance,
    _In_ VECTOR3 SurfaceNormal,
    _Out_ PCPBR_BRDF *PbrBrdf
    )
{
    SPECRUM_LAMBERTIAN_BRDF BrdfData;
    ISTATUS Status;

    if (Allocator == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (Reflectance == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (VectorValidate(SurfaceNormal) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (PbrBrdf == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    BrdfData.Reflectance = Reflectance;
    BrdfData.SurfaceNormal = SurfaceNormal;

    Status = PbrBrdfAllocatorAllocate(Allocator,
                                      &SpectrumLambertianBrdfVTable,
                                      &BrdfData,
                                      sizeof(SPECRUM_LAMBERTIAN_BRDF),
                                      _Alignof(SPECRUM_LAMBERTIAN_BRDF),
                                      PbrBrdf);

    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }

    return ISTATUS_SUCCESS;
}