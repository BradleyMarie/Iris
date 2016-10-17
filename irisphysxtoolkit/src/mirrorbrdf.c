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
    _In_ VECTOR3 SurfaceNormal,
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
    ASSERT(VectorValidate(SurfaceNormal) != FALSE);
    ASSERT(Rng != NULL);
    ASSERT(Compositor != NULL);
    ASSERT(Reflector != NULL);
    ASSERT(Outgoing != NULL);
    ASSERT(Pdf != NULL);

    Brdf = (PCSPECTRUM_MIRROR_BRDF) Context;

    *Reflector = Brdf->Reflectance;
    *Outgoing = VectorReflect(Incoming, SurfaceNormal);
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
    _In_ VECTOR3 SurfaceNormal,
    _In_ VECTOR3 Outgoing,
    _Inout_ PREFLECTOR_COMPOSITOR_REFERENCE Compositor,
    _Out_ PCREFLECTOR *Reflector
    )
{
    ASSERT(Context != NULL);
    ASSERT(VectorValidate(Incoming) != FALSE);
    ASSERT(VectorValidate(SurfaceNormal) != FALSE);
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
    _In_ VECTOR3 SurfaceNormal,
    _In_ VECTOR3 Outgoing,
    _Inout_ PREFLECTOR_COMPOSITOR_REFERENCE Compositor,
    _Out_ PCREFLECTOR *Reflector,
    _Out_ PFLOAT Pdf
    )
{
    ASSERT(Context != NULL);
    ASSERT(VectorValidate(Incoming) != FALSE);
    ASSERT(VectorValidate(SurfaceNormal) != FALSE);
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

CONST STATIC PHYSX_BRDF_VTABLE SpectrumMirrorBrdfVTable = {
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
    _In_ PPHYSX_BRDF_ALLOCATOR Allocator,
    _In_ PCREFLECTOR Reflectance,
    _Out_ PCPHYSX_BRDF *Brdf
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

    if (Brdf == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    BrdfData.Reflectance = Reflectance;

    Status = PhysxBrdfAllocatorAllocate(Allocator,
                                        &SpectrumMirrorBrdfVTable,
                                        &BrdfData,
                                        sizeof(SPECTRUM_MIRROR_BRDF),
                                        _Alignof(SPECTRUM_MIRROR_BRDF),
                                        Brdf);

    return Status;
}
