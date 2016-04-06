/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    mirrorbrdf.h

Abstract:

    This file contains the definitions for SPECTRUM_MIRROR_BRDF type.

--*/

#include <irisphysxtoolkitp.h>

//
// Types
//

typedef struct _SPECTRUM_MIRROR_BRDF {
    PCREFLECTOR Reflectance;
    VECTOR3 SurfaceNormal;
} SPECTRUM_MIRROR_BRDF, *PSPECTRUM_MIRROR_BRDF;

typedef CONST SPECTRUM_MIRROR_BRDF *PCSPECTRUM_MIRROR_BRDF;

//
// Static Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
STATIC
ISTATUS
SpectrumMirrorBrdfSample(
    _In_ PCVOID Context,
    _In_ VECTOR3 Incoming,
    _Inout_ PRANDOM_REFERENCE Rng,
    _Inout_ PREFLECTOR_COMPOSITOR_REFERENCE Compositor,
    _Out_ PCREFLECTOR *Reflector,
    _Out_ PVECTOR3 Outgoing,
    _Out_ PFLOAT Pdf
    )
{
    PCSPECTRUM_MIRROR_BRDF Brdf;

    ASSERT(Context != NULL);
    ASSERT(VectorValidate(Incoming) != FALSE);
    ASSERT(Rng != NULL);
    ASSERT(Compositor != NULL);
    ASSERT(Reflector != NULL);
    ASSERT(Outgoing != NULL);
    ASSERT(Pdf != NULL);

    Brdf = (PCSPECTRUM_MIRROR_BRDF) Context;

    *Reflector = Brdf->Reflectance;
    *Outgoing = VectorReflect(Incoming, Brdf->SurfaceNormal);
    *Pdf = (FLOAT) INFINITY;

    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
STATIC
ISTATUS
SpectrumMirrorBrdfComputeReflectance(
    _In_ PCVOID Context,
    _In_ VECTOR3 Incoming,
    _In_ VECTOR3 Outgoing,
    _Inout_ PREFLECTOR_COMPOSITOR_REFERENCE Compositor,
    _Out_ PCREFLECTOR *Reflector
    )
{
    ASSERT(Context != NULL);
    ASSERT(VectorValidate(Incoming) != FALSE);
    ASSERT(VectorValidate(Outgoing) != FALSE);
    ASSERT(Compositor != NULL);
    ASSERT(Reflector != NULL);

    *Reflector = NULL;

    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
STATIC
ISTATUS
SpectrumMirrorBrdfComputeReflectanceWithPdf(
    _In_ PCVOID Context,
    _In_ VECTOR3 Incoming,
    _In_ VECTOR3 Outgoing,
    _Inout_ PREFLECTOR_COMPOSITOR_REFERENCE Compositor,
    _Out_ PCREFLECTOR *Reflector,
    _Out_ PFLOAT Pdf
    )
{
    ASSERT(Context != NULL);
    ASSERT(VectorValidate(Incoming) != FALSE);
    ASSERT(VectorValidate(Outgoing) != FALSE);
    ASSERT(Compositor != NULL);
    ASSERT(Reflector != NULL);
    ASSERT(Pdf != NULL);

    *Reflector = NULL;
    *Pdf = (FLOAT) 0.0f;

    return ISTATUS_SUCCESS;
}

//
// Static Variables
//

CONST STATIC PBR_BRDF_VTABLE SpectrumMirrorBrdfVTable = {
    SpectrumMirrorBrdfSample,
    SpectrumMirrorBrdfSample,
    SpectrumMirrorBrdfComputeReflectance,
    SpectrumMirrorBrdfComputeReflectance,
    SpectrumMirrorBrdfComputeReflectanceWithPdf,
    SpectrumMirrorBrdfComputeReflectanceWithPdf,
    NULL
};

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
SpectrumMirrorBrdfAllocate(
    _In_ PPBR_BRDF_ALLOCATOR Allocator,
    _In_ PCREFLECTOR Reflectance,
    _In_ VECTOR3 SurfaceNormal,
    _Out_ PCPBR_BRDF *PbrBrdf
    )
{
    SPECTRUM_MIRROR_BRDF BrdfData;
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
                                      &SpectrumMirrorBrdfVTable,
                                      &BrdfData,
                                      sizeof(SPECTRUM_MIRROR_BRDF),
                                      _Alignof(SPECTRUM_MIRROR_BRDF),
                                      PbrBrdf);

    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }

    return ISTATUS_SUCCESS;
}