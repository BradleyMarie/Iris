/*++

Copyright (c) 2020 Brad Weinberger

Module Name:

    matte_material.c

Abstract:

    Re-implementation of pbrt's matte material.

--*/

#include <stdalign.h>

#include "iris_physx_toolkit/matte_material.h"
#include "iris_physx_toolkit/lambertian_bsdf.h"

//
// Types
//

typedef struct _MATTE_MATERIAL {
    PREFLECTOR_TEXTURE diffuse;
} MATTE_MATERIAL, *PMATTE_MATERIAL;

typedef const MATTE_MATERIAL *PCMATTE_MATERIAL;

//
// Static Functions
//

static
ISTATUS
MatteMaterialSample(
    _In_ const void *context,
    _In_ POINT3 model_hit_point,
    _In_ const void *additional_data,
    _In_ const void *texture_coordinates,
    _Inout_ PBSDF_ALLOCATOR bsdf_allocator,
    _Inout_ PREFLECTOR_COMPOSITOR reflector_compositor,
    _Out_ PCBSDF *bsdf
    )
{
    PCMATTE_MATERIAL matte_material = (PCMATTE_MATERIAL)context;

    PCREFLECTOR reflector;
    ISTATUS status = ReflectorTextureSample(matte_material->diffuse,
                                            model_hit_point,
                                            additional_data,
                                            texture_coordinates,
                                            reflector_compositor,
                                            &reflector);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    status = LambertianReflectorAllocateWithAllocator(bsdf_allocator,
                                                      reflector,
                                                      bsdf);

    return status;
}

static
void
MatteMaterialFree(
    _In_opt_ _Post_invalid_ void *context
    )
{
    PMATTE_MATERIAL matte_material = (PMATTE_MATERIAL)context;

    ReflectorTextureRelease(matte_material->diffuse);
}

//
// Static Variables
//

static const MATERIAL_VTABLE matte_material_vtable = {
    MatteMaterialSample,
    MatteMaterialFree
};

//
// Functions
//

ISTATUS
MatteMaterialAllocate(
    _In_ PREFLECTOR_TEXTURE diffuse,
    _Out_ PMATERIAL *material
    )
{
    if (diffuse == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (material == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    MATTE_MATERIAL matte_material;
    matte_material.diffuse = diffuse;

    ISTATUS status = MaterialAllocate(&matte_material_vtable,
                                      &matte_material,
                                      sizeof(MATTE_MATERIAL),
                                      alignof(MATTE_MATERIAL),
                                      material);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    ReflectorTextureRetain(diffuse);

    return ISTATUS_SUCCESS;
}