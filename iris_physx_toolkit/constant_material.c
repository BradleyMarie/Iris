/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    constant_material.c

Abstract:

    Implements a constant material.

--*/

#include <stdalign.h>

#include "iris_physx_toolkit/constant_material.h"

//
// Types
//

typedef struct _CONSTANT_MATERIAL {
    PBRDF brdf;
} CONSTANT_MATERIAL, *PCONSTANT_MATERIAL;

typedef const CONSTANT_MATERIAL *PCCONSTANT_MATERIAL;

//
// Static Functions
//

static
ISTATUS
ConstantMaterialSample(
    _In_ const void *context,
    _In_ POINT3 model_hit_point,
    _In_ VECTOR3 world_surface_normal,
    _In_ const void *additional_data,
    _Inout_ PBRDF_ALLOCATOR brdf_allocator,
    _Inout_ PREFLECTOR_ALLOCATOR reflector_allocator,
    _Out_ PVECTOR3 world_shading_normal,
    _Out_ PCBRDF *brdf
    )
{
    PCONSTANT_MATERIAL constant_material = (PCONSTANT_MATERIAL)context;

    *world_shading_normal = world_surface_normal;
    *brdf = constant_material->brdf;

    return ISTATUS_SUCCESS;
}

static
void
ConstantMaterialFree(
    _In_opt_ _Post_invalid_ void *context
    )
{
    PCONSTANT_MATERIAL constant_material = (PCONSTANT_MATERIAL)context;

    BrdfRelease(constant_material->brdf);
}

//
// Static Variables
//

static const MATERIAL_VTABLE constant_material_vtable = {
    ConstantMaterialSample,
    ConstantMaterialFree
};

//
// Functions
//

ISTATUS
ConstantMaterialAllocate(
    _In_ PBRDF brdf,
    _Out_ PMATERIAL *material
    )
{
    if (brdf == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (material == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    CONSTANT_MATERIAL constant_material;
    constant_material.brdf = brdf;

    ISTATUS status = MaterialAllocate(&constant_material_vtable,
                                      &constant_material,
                                      sizeof(CONSTANT_MATERIAL),
                                      alignof(CONSTANT_MATERIAL),
                                      material);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    BrdfRetain(brdf);

    return ISTATUS_SUCCESS;
}