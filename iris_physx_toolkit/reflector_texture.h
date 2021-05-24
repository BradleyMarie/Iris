/*++

Copyright (c) 2021 Brad Weinberger

Module Name:

    reflector_texture.h

Abstract:

    Interface representing a texture of reflectors.

--*/

#ifndef _IRIS_PHYSX_TOOLKIT_REFLECTOR_TEXTURE_
#define _IRIS_PHYSX_TOOLKIT_REFLECTOR_TEXTURE_

#include "iris_physx_toolkit/reflector_texture_vtable.h"

#if __cplusplus
extern "C" {
#endif // __cplusplus

//
// Types
//

typedef struct _REFLECTOR_TEXTURE REFLECTOR_TEXTURE, *PREFLECTOR_TEXTURE;
typedef const REFLECTOR_TEXTURE *PCREFLECTOR_TEXTURE;

//
// Functions
//

ISTATUS
ReflectorTextureAllocate(
    _In_ PCREFLECTOR_TEXTURE_VTABLE vtable,
    _In_reads_bytes_opt_(data_size) const void *data,
    _In_ size_t data_size,
    _In_ size_t data_alignment,
    _Out_ PREFLECTOR_TEXTURE *texture
    );

ISTATUS
ReflectorTextureSample(
    _In_opt_ PCREFLECTOR_TEXTURE texture,
    _In_ PCINTERSECTION intersection,
    _In_ const void *additional_data,
    _In_ const void *texture_coordinates,
    _Inout_ PREFLECTOR_COMPOSITOR reflector_compositor,
    _Out_ PCREFLECTOR *value
    );

void
ReflectorTextureRetain(
    _In_opt_ PREFLECTOR_TEXTURE texture
    );

void
ReflectorTextureRelease(
    _In_opt_ _Post_invalid_ PREFLECTOR_TEXTURE texture
    );

#if __cplusplus
}
#endif // __cplusplus

#endif // _IRIS_PHYSX_TOOLKIT_REFLECTOR_TEXTURE_