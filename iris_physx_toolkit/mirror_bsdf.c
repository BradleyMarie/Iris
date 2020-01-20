/*++

Copyright (c) 2020 Brad Weinberger

Module Name:

    mirror_bsdf.c

Abstract:

    Implements a mirror BSDF.

--*/

#include <stdalign.h>

#include "iris_physx_toolkit/mirror_bsdf.h"

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
MirrorReflectorSample(
    _In_ const void *context,
    _In_ VECTOR3 incoming,
    _In_ VECTOR3 normal,
    _Inout_ PRANDOM rng,
    _Inout_ PREFLECTOR_COMPOSITOR compositor,
    _Out_ PCREFLECTOR *reflector,
    _Out_ bool *transmitted,
    _Out_ PVECTOR3 outgoing,
    _Out_ float_t *pdf
    )
{
    PCMIRROR_BSDF mirror_bsdf = (PCMIRROR_BSDF)context;

    *reflector = mirror_bsdf->reflector;
    *transmitted = false;
    *outgoing = VectorReflect(incoming, normal);
    *pdf = INFINITY;

    return ISTATUS_SUCCESS;
}

static
ISTATUS
MirrorReflectorComputeReflectance(
    _In_ const void *context,
    _In_ VECTOR3 incoming,
    _In_ VECTOR3 normal,
    _In_ VECTOR3 outgoing,
    _Inout_ PREFLECTOR_COMPOSITOR compositor,
    _Out_ PCREFLECTOR *reflector,
    _Out_ bool *transmitted
    )
{
    *reflector = NULL;
    *transmitted = false;

    return ISTATUS_SUCCESS;
}

static
ISTATUS
MirrorReflectorComputeReflectanceWithPdf(
    _In_ const void *context,
    _In_ VECTOR3 incoming,
    _In_ VECTOR3 normal,
    _In_ VECTOR3 outgoing,
    _Inout_ PREFLECTOR_COMPOSITOR compositor,
    _Out_ PCREFLECTOR *reflector,
    _Out_ bool *transmitted,
    _Out_ float_t *pdf
    )
{
    *pdf = (float_t)0.0;

    return ISTATUS_SUCCESS;
}

static
ISTATUS
MirrorReflectorCacheColors(
    _In_ const void *context,
    _Inout_ PCOLOR_CACHE color_cache
    )
{
    PCMIRROR_BSDF mirror_bsdf = (PCMIRROR_BSDF)context;

    ISTATUS status = ColorCacheAddReflector(color_cache,
                                            mirror_bsdf->reflector);

    return status;
}

static
void
MirrorReflectorFree(
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
    MirrorReflectorSample,
    MirrorReflectorComputeReflectance,
    MirrorReflectorComputeReflectanceWithPdf,
    MirrorReflectorCacheColors,
    MirrorReflectorFree
};

//
// Functions
//

ISTATUS
MirrorReflectorAllocate(
    _In_ PREFLECTOR reflector,
    _Out_ PBSDF *bsdf
    )
{
    if (reflector == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (bsdf == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
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
MirrorReflectorAllocateWithAllocator(
    _Inout_ PBSDF_ALLOCATOR bsdf_allocator,
    _In_ PCREFLECTOR reflector,
    _Out_ PCBSDF *bsdf
    )
{
    if (bsdf_allocator == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (reflector == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (bsdf == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
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