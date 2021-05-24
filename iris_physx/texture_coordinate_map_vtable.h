/*++

Copyright (c) 2021 Brad Weinberger

Module Name:

    texture_coordinate_map_vtable.h

Abstract:

    The vtable for a texture coordinate map.

--*/

#ifndef _IRIS_PHYSX_TEXTURE_COORDINATE_MAP_VTABLE_
#define _IRIS_PHYSX_TEXTURE_COORDINATE_MAP_VTABLE_

#include "common/free_routine.h"
#include "iris_advanced/iris_advanced.h"
#include "iris_physx/texture_coordinate_allocator.h"

//
// Types
//

typedef
ISTATUS
(*PTEXTURE_COORDINATE_MAP_COMPUTE_ROUTINE)(
    _In_ const void *context,
    _In_ PCINTERSECTION intersection,
    _In_opt_ PCMATRIX model_to_world,
    _In_ const void *additional_data,
    _Inout_ PTEXTURE_COORDINATE_ALLOCATOR allocator,
    _Out_ void **texture_coordinates
    );

typedef struct _TEXTURE_COORDINATE_MAP_VTABLE {
    PTEXTURE_COORDINATE_MAP_COMPUTE_ROUTINE compute_routine;
    PFREE_ROUTINE free_routine;
} TEXTURE_COORDINATE_MAP_VTABLE, *PTEXTURE_COORDINATE_MAP_VTABLE;

typedef const TEXTURE_COORDINATE_MAP_VTABLE *PCTEXTURE_COORDINATE_MAP_VTABLE;

#endif // _IRIS_PHYSX_TEXTURE_COORDINATE_MAP_VTABLE_