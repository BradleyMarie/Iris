/*++

Copyright (c) 2021 Brad Weinberger

Module Name:

    texture_coordinate_map_internal.h

Abstract:

    The internal routines for a texture coordinate map.

--*/

#ifndef _IRIS_PHYSX_TEXTURE_COORDINATE_MAP_INTERNAL_
#define _IRIS_PHYSX_TEXTURE_COORDINATE_MAP_INTERNAL_

#include <stdatomic.h>

#include "iris_physx/texture_coordinate_map_vtable.h"

//
// Types
//

struct _TEXTURE_COORDINATE_MAP {
    PCTEXTURE_COORDINATE_MAP_VTABLE vtable;
    void *data;
    atomic_uintmax_t reference_count;
};

//
// Functions
//

static
inline
ISTATUS
TextureCoordinateMapCompute(
    _In_ const struct _TEXTURE_COORDINATE_MAP *texture_coordinate_map,
    _In_ PCINTERSECTION intersection,
    _In_ const void *additional_data,
    _Inout_ PTEXTURE_COORDINATE_ALLOCATOR allocator,
    _Out_ void **texture_coordinates
    )
{
    assert(texture_coordinate_map != NULL);
    assert(intersection != NULL);
    assert(intersection != NULL);
    assert(texture_coordinates != NULL);

    ISTATUS status =
        texture_coordinate_map->vtable->compute_routine(texture_coordinate_map->data,
                                                        intersection,
                                                        additional_data,
                                                        allocator,
                                                        texture_coordinates);

    return status;
}

#endif // _IRIS_PHYSX_TEXTURE_COORDINATE_MAP_INTERNAL_