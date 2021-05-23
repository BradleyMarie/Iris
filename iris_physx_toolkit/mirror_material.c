/*++

Copyright (c) 2021 Brad Weinberger

Module Name:

    mirror_material.c

Abstract:

    Re-implementation of pbrt's mirror material.

--*/

#include <stdalign.h>

#include "iris_physx_toolkit/mirror_material.h"
#include "iris_physx_toolkit/mirror_bsdf.h"

//
// Types
//

typedef struct _MIRROR_MATERIAL {
    PREFLECTOR_TEXTURE reflectance;
} MIRROR_MATERIAL, *PMIRROR_MATERIAL;

typedef const MIRROR_MATERIAL *PCMIRROR_MATERIAL;

//
// Static Functions
//

static
ISTATUS
MirrorMaterialSample(
    _In_ const void *context,
    _In_ POINT3 model_hit_point,
    _In_ const void *additional_data,
    _In_ const void *texture_coordinates,
    _Inout_ PBSDF_ALLOCATOR bsdf_allocator,
    _Inout_ PREFLECTOR_COMPOSITOR reflector_compositor,
    _Out_ PCBSDF *bsdf
    )
{
    PCMIRROR_MATERIAL mirror_material = (PCMIRROR_MATERIAL)context;

    PCREFLECTOR reflector;
    ISTATUS status = ReflectorTextureSample(mirror_material->reflectance,
                                            model_hit_point,
                                            additional_data,
                                            texture_coordinates,
                                            reflector_compositor,
                                            &reflector);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    status = MirrorBsdfAllocateWithAllocator(bsdf_allocator,
                                             reflector,
                                             bsdf);

    return status;
}

static
void
MirrorMaterialFree(
    _In_opt_ _Post_invalid_ void *context
    )
{
    PMIRROR_MATERIAL mirror_material = (PMIRROR_MATERIAL)context;

    ReflectorTextureRelease(mirror_material->reflectance);
}

//
// Static Variables
//

static const MATERIAL_VTABLE mirror_material_vtable = {
    MirrorMaterialSample,
    MirrorMaterialFree
};

//
// Functions
//

ISTATUS
MirrorMaterialAllocate(
    _In_opt_ PREFLECTOR_TEXTURE reflectance,
    _Out_ PMATERIAL *material
    )
{
    if (material == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (reflectance == NULL)
    {
        *material = NULL;
        return ISTATUS_SUCCESS;
    }

    MIRROR_MATERIAL mirror_material;
    mirror_material.reflectance = reflectance;

    ISTATUS status = MaterialAllocate(&mirror_material_vtable,
                                      &mirror_material,
                                      sizeof(MIRROR_MATERIAL),
                                      alignof(MIRROR_MATERIAL),
                                      material);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    ReflectorTextureRetain(reflectance);

    return ISTATUS_SUCCESS;
}