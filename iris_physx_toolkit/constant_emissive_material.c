/*++

Copyright (c) 2021 Brad Weinberger

Module Name:

    constant_emissive_material.c

Abstract:

    Implements a constant emissive material.

--*/

#include <stdalign.h>

#include "iris_physx_toolkit/constant_emissive_material.h"

//
// Types
//

typedef struct _CONSTANT_EMISSIVE_MATERIAL {
    PSPECTRUM spectrum;
} CONSTANT_EMISSIVE_MATERIAL, *PCONSTANT_EMISSIVE_MATERIAL;

typedef const CONSTANT_EMISSIVE_MATERIAL *PCCONSTANT_EMISSIVE_MATERIAL;

//
// Static Functions
//

static
ISTATUS
ConstantEmissiveMaterialSample(
    _In_ const void *context,
    _In_ POINT3 model_hit_point,
    _In_ const void *additional_data,
    _Out_ PCSPECTRUM *spectrum
    )
{
    PCCONSTANT_EMISSIVE_MATERIAL constant_material =
        (PCCONSTANT_EMISSIVE_MATERIAL)context;

    *spectrum = constant_material->spectrum;

    return ISTATUS_SUCCESS;
}

static
void
ConstantEmissiveMaterialFree(
    _In_opt_ _Post_invalid_ void *context
    )
{
    PCONSTANT_EMISSIVE_MATERIAL constant_material =
        (PCONSTANT_EMISSIVE_MATERIAL)context;

    SpectrumRelease(constant_material->spectrum);
}

//
// Static Variables
//

static const EMISSIVE_MATERIAL_VTABLE constant_material_vtable = {
    ConstantEmissiveMaterialSample,
    ConstantEmissiveMaterialFree
};

//
// Functions
//

ISTATUS
ConstantEmissiveMaterialAllocate(
    _In_ PSPECTRUM spectrum,
    _Out_ PEMISSIVE_MATERIAL *material
    )
{
    if (spectrum == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (material == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    CONSTANT_EMISSIVE_MATERIAL constant_material;
    constant_material.spectrum = spectrum;

    ISTATUS status = EmissiveMaterialAllocate(
        &constant_material_vtable,
        &constant_material,
        sizeof(CONSTANT_EMISSIVE_MATERIAL),
        alignof(CONSTANT_EMISSIVE_MATERIAL),
        material);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    SpectrumRetain(spectrum);

    return ISTATUS_SUCCESS;
}