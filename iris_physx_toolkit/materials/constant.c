/*++

Copyright (c) 2021 Brad Weinberger

Module Name:

    constant.c

Abstract:

    Implements a constant material.

--*/

#include <stdalign.h>

#include "iris_physx_toolkit/materials/constant.h"

//
// Types
//

typedef struct _CONSTANT_MATERIAL {
    PBSDF bsdf;
} CONSTANT_MATERIAL, *PCONSTANT_MATERIAL;

typedef const CONSTANT_MATERIAL *PCCONSTANT_MATERIAL;

//
// Static Functions
//

static
ISTATUS
ConstantMaterialSample(
    _In_ const void *context,
    _In_ PCINTERSECTION intersection,
    _In_ const void *additional_data,
    _In_ const void *texture_coordinates,
    _Inout_ PBSDF_ALLOCATOR bsdf_allocator,
    _Inout_ PREFLECTOR_COMPOSITOR reflector_compositor,
    _Out_ PCBSDF *bsdf
    )
{
    PCCONSTANT_MATERIAL constant_material = (PCCONSTANT_MATERIAL)context;

    *bsdf = constant_material->bsdf;

    return ISTATUS_SUCCESS;
}

static
void
ConstantMaterialFree(
    _In_opt_ _Post_invalid_ void *context
    )
{
    PCONSTANT_MATERIAL constant_material = (PCONSTANT_MATERIAL)context;

    BsdfRelease(constant_material->bsdf);
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
    _In_opt_ PBSDF bsdf,
    _Out_ PMATERIAL *material
    )
{
    if (material == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (bsdf == NULL)
    {
        *material = NULL;
        return ISTATUS_SUCCESS;
    }

    CONSTANT_MATERIAL constant_material;
    constant_material.bsdf = bsdf;

    ISTATUS status = MaterialAllocate(&constant_material_vtable,
                                      &constant_material,
                                      sizeof(CONSTANT_MATERIAL),
                                      alignof(CONSTANT_MATERIAL),
                                      material);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    BsdfRetain(bsdf);

    return ISTATUS_SUCCESS;
}