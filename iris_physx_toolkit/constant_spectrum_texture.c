/*++

Copyright (c) 2021 Brad Weinberger

Module Name:

    constant_spectrum_texture.c

Abstract:

    Allocates a constant spectrum texture.

--*/

#include <stdalign.h>
#include <stdlib.h>

#include "iris_physx_toolkit/constant_spectrum_texture.h"

//
// Types
//

typedef struct _CONSTANT_SPECTRUM_TEXTURE {
    PSPECTRUM spectrum;
} CONSTANT_SPECTRUM_TEXTURE, *PCONSTANT_SPECTRUM_TEXTURE;

typedef const CONSTANT_SPECTRUM_TEXTURE *PCCONSTANT_SPECTRUM_TEXTURE;

//
// Static Functions
//

static
ISTATUS
ConstantSpectrumTextureSample(
    _In_ const void *context,
    _In_ VECTOR3 direction,
    _In_opt_ VECTOR3 dxdy[2],
    _Inout_ PSPECTRUM_COMPOSITOR spectrum_compositor,
    _Out_ PCSPECTRUM *spectrum
    )
{
    PCCONSTANT_SPECTRUM_TEXTURE texture =
        (PCCONSTANT_SPECTRUM_TEXTURE)context;

    *spectrum = texture->spectrum;

    return ISTATUS_SUCCESS;
}

static
void
ConstantSpectrumTextureFree(
    _In_opt_ _Post_invalid_ void *context
    )
{
    PCONSTANT_SPECTRUM_TEXTURE texture = (PCONSTANT_SPECTRUM_TEXTURE)context;
    SpectrumRelease(texture->spectrum);
}

//
// Static Variables
//

static const SPECTRUM_TEXTURE_VTABLE constant_spectrum_texture_vtable = {
    ConstantSpectrumTextureSample,
    ConstantSpectrumTextureFree
};

//
// Functions
//

ISTATUS
ConstantSpectrumTextureAllocate(
    _In_opt_ PSPECTRUM spectrum,
    _Out_ PSPECTRUM_TEXTURE *texture
    )
{
    if (texture == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (spectrum == NULL)
    {
        *texture = NULL;
        return ISTATUS_SUCCESS;
    }

    CONSTANT_SPECTRUM_TEXTURE constant_texture;
    constant_texture.spectrum = spectrum;

    ISTATUS status = SpectrumTextureAllocate(&constant_spectrum_texture_vtable,
                                              &constant_texture,
                                              sizeof(PCONSTANT_SPECTRUM_TEXTURE),
                                              alignof(PCONSTANT_SPECTRUM_TEXTURE),
                                              texture);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    SpectrumRetain(spectrum);

    return ISTATUS_SUCCESS;
}