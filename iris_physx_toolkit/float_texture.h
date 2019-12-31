/*++

Copyright (c) 2019 Brad Weinberger

Module Name:

    float_texture.h

Abstract:

    Interface representing a texture of floats.

--*/

#ifndef _IRIS_PHYSX_TOOLKIT_FLOAT_TEXTURE_
#define _IRIS_PHYSX_TOOLKIT_FLOAT_TEXTURE_

#include "iris_physx_toolkit/float_texture_vtable.h"

#if __cplusplus
extern "C" {
#endif // __cplusplus

//
// Types
//

typedef struct _FLOAT_TEXTURE FLOAT_TEXTURE, *PFLOAT_TEXTURE;
typedef const FLOAT_TEXTURE *PCFLOAT_TEXTURE;

//
// Functions
//

ISTATUS
FloatTextureAllocate(
    _In_ PCFLOAT_TEXTURE_VTABLE vtable,
    _In_reads_bytes_opt_(data_size) const void *data,
    _In_ size_t data_size,
    _In_ size_t data_alignment,
    _Out_ PFLOAT_TEXTURE *texture
    );

ISTATUS
FloatTextureSample(
    _In_opt_ PCFLOAT_TEXTURE texture,
    _In_ POINT3 model_hit_point,
    _In_opt_ const void *additional_data,
    _In_opt_ const void *texture_coordinates,
    _Out_ float_t *value
    );

void
FloatTextureRetain(
    _In_opt_ PFLOAT_TEXTURE texture
    );

void
FloatTextureRelease(
    _In_opt_ _Post_invalid_ PFLOAT_TEXTURE texture
    );

#if __cplusplus
}
#endif // __cplusplus

#endif // _IRIS_PHYSX_TOOLKIT_FLOAT_TEXTURE_