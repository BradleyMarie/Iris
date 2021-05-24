/*++

Copyright (c) 2021 Brad Weinberger

Module Name:

    texture_coordinate_map.h

Abstract:

    Interface for an object that computes the texture coordinates used for
    shading.

--*/

#ifndef _IRIS_PHYSX_TEXTURE_COORDINATE_MAP_
#define _IRIS_PHYSX_TEXTURE_COORDINATE_MAP_

#include <stddef.h>

#include "iris_physx/texture_coordinate_map_vtable.h"

//
// Types
//

typedef struct _TEXTURE_COORDINATE_MAP TEXTURE_COORDINATE_MAP;
typedef TEXTURE_COORDINATE_MAP *PTEXTURE_COORDINATE_MAP;
typedef const TEXTURE_COORDINATE_MAP *PCTEXTURE_COORDINATE_MAP;

//
// Functions
//

ISTATUS
TextureCoordinateMapAllocate(
    _In_ PCTEXTURE_COORDINATE_MAP_VTABLE vtable,
    _In_reads_bytes_opt_(data_size) const void *data,
    _In_ size_t data_size,
    _In_ size_t data_alignment,
    _Out_ PTEXTURE_COORDINATE_MAP *texture_coordinate_map
    );

void
TextureCoordinateMapRetain(
    _In_opt_ PTEXTURE_COORDINATE_MAP texture_coordinate_map
    );

void
TextureCoordinateMapRelease(
    _In_opt_ _Post_invalid_ PTEXTURE_COORDINATE_MAP texture_coordinate_map
    );

#endif // _IRIS_PHYSX_TEXTURE_COORDINATE_MAP_