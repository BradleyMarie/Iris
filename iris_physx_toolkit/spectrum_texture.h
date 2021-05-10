/*++

Copyright (c) 2021 Brad Weinberger

Module Name:

    spectrum_texture.h

Abstract:

    Interface representing a texture of spectrums.

--*/

#ifndef _IRIS_PHYSX_TOOLKIT_SPECTRUM_TEXTURE_
#define _IRIS_PHYSX_TOOLKIT_SPECTRUM_TEXTURE_

#include "iris_physx_toolkit/spectrum_texture_vtable.h"

#if __cplusplus
extern "C" {
#endif // __cplusplus

//
// Types
//

typedef struct _SPECTRUM_TEXTURE SPECTRUM_TEXTURE, *PSPECTRUM_TEXTURE;
typedef const SPECTRUM_TEXTURE *PCSPECTRUM_TEXTURE;

//
// Functions
//

ISTATUS
SpectrumTextureAllocate(
    _In_ PCSPECTRUM_TEXTURE_VTABLE vtable,
    _In_reads_bytes_opt_(data_size) const void *data,
    _In_ size_t data_size,
    _In_ size_t data_alignment,
    _Out_ PSPECTRUM_TEXTURE *texture
    );

ISTATUS
SpectrumTextureSample(
    _In_opt_ PCSPECTRUM_TEXTURE texture,
    _In_ VECTOR3 direction,
    _In_opt_ VECTOR3 dxdy[2],
    _Inout_ PSPECTRUM_COMPOSITOR spectrum_compositor,
    _Out_ PCSPECTRUM *value
    );

void
SpectrumTextureRetain(
    _In_opt_ PSPECTRUM_TEXTURE texture
    );

void
SpectrumTextureRelease(
    _In_opt_ _Post_invalid_ PSPECTRUM_TEXTURE texture
    );

#if __cplusplus
}
#endif // __cplusplus

#endif // _IRIS_PHYSX_TOOLKIT_SPECTRUM_TEXTURE_