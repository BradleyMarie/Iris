/*++

Copyright (c) 2021 Brad Weinberger

Module Name:

    mirror.c

Abstract:

    Implements a mirror BSDF.

--*/

#include <stdalign.h>

#include "iris_physx_toolkit/bsdfs/mirror.h"

//
// Types
//

typedef struct _MIRROR_BSDF {
    PREFLECTOR reflector;
} MIRROR_BSDF, *PMIRROR_BSDF;

typedef const MIRROR_BSDF *PCMIRROR_BSDF;

//
// Static Functions
//

static
ISTATUS
MirrorBsdfSample(
    _In_ const void *context,
    _In_ VECTOR3 incoming,
    _In_ VECTOR3 surface_normal,
    _In_ VECTOR3 shading_normal,
    _Inout_ PRANDOM rng,
    _Inout_ PREFLECTOR_COMPOSITOR compositor,
    _Out_ PCREFLECTOR *reflector,
    _Out_ PBSDF_SAMPLE_TYPE type,
    _Out_ PVECTOR3 outgoing,
    _Out_ float_t *pdf
    )
{
    PCMIRROR_BSDF mirror_bsdf = (PCMIRROR_BSDF)context;

    *reflector = mirror_bsdf->reflector;
    *type = BSDF_SAMPLE_TYPE_REFLECTION_CONTIANS_SPECULAR;
    *outgoing = VectorReflect(incoming, shading_normal);
    *pdf = INFINITY;

    return ISTATUS_SUCCESS;
}

static
void
MirrorBsdfFree(
    _In_opt_ _Post_invalid_ void *context
    )
{
    PMIRROR_BSDF mirror_bsdf = (PMIRROR_BSDF)context;

    ReflectorRelease(mirror_bsdf->reflector);
}

//
// Static Variables
//

static const BSDF_VTABLE mirror_bsdf_vtable = {
    MirrorBsdfSample,
    NULL,
    NULL,
    NULL,
    MirrorBsdfFree
};

//
// Functions
//

ISTATUS
MirrorBsdfAllocate(
    _In_opt_ PREFLECTOR reflector,
    _Out_ PBSDF *bsdf
    )
{
    if (bsdf == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (reflector == NULL)
    {
        *bsdf = NULL;
        return ISTATUS_SUCCESS;
    }

    MIRROR_BSDF mirror_bsdf;
    mirror_bsdf.reflector = reflector;

    ISTATUS status = BsdfAllocate(&mirror_bsdf_vtable,
                                  &mirror_bsdf,
                                  sizeof(MIRROR_BSDF),
                                  alignof(MIRROR_BSDF),
                                  bsdf);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    ReflectorRetain(reflector);

    return ISTATUS_SUCCESS;
}

ISTATUS
MirrorBsdfAllocateWithAllocator(
    _Inout_ PBSDF_ALLOCATOR bsdf_allocator,
    _In_opt_ PCREFLECTOR reflector,
    _Out_ PCBSDF *bsdf
    )
{
    if (bsdf_allocator == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (bsdf == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (reflector == NULL)
    {
        *bsdf = NULL;
        return ISTATUS_SUCCESS;
    }

    MIRROR_BSDF mirror_bsdf;
    mirror_bsdf.reflector = (PREFLECTOR)reflector;

    ISTATUS status = BsdfAllocatorAllocate(bsdf_allocator,
                                           &mirror_bsdf_vtable,
                                           &mirror_bsdf,
                                           sizeof(MIRROR_BSDF),
                                           alignof(MIRROR_BSDF),
                                           bsdf);

    return status;   
}