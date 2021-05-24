/*++

Copyright (c) 2021 Brad Weinberger

Module Name:

    normal_map_vtable.h

Abstract:

    The vtable for a normal map.

--*/

#ifndef _IRIS_PHYSX_NORMAL_MAP_VTABLE_
#define _IRIS_PHYSX_NORMAL_MAP_VTABLE_

#include "common/free_routine.h"
#include "iris_advanced/iris_advanced.h"

//
// Types
//

typedef enum _NORMAL_COORDINATE_SPACE {
    NORMAL_MODEL_COORDINATE_SPACE = 0,
    NORMAL_WORLD_COORDINATE_SPACE = 1
} NORMAL_COORDINATE_SPACE, *PNORMAL_COORDINATE_SPACE;

typedef
ISTATUS
(*PNORMAL_MAP_COMPUTE_ROUTINE)(
    _In_ const void *context,
    _In_ PCINTERSECTION intersection,
    _In_ VECTOR3 model_geometry_normal,
    _In_ VECTOR3 world_geometry_normal,
    _In_ const void *additional_data,
    _In_ const void *texture_coordinates,
    _Out_ PVECTOR3 shading_normal,
    _Out_ PNORMAL_COORDINATE_SPACE coordinate_space
    );

typedef struct _NORMAL_MAP_VTABLE {
    PNORMAL_MAP_COMPUTE_ROUTINE compute_routine;
    PFREE_ROUTINE free_routine;
} NORMAL_MAP_VTABLE, *PNORMAL_MAP_VTABLE;

typedef const NORMAL_MAP_VTABLE *PCNORMAL_MAP_VTABLE;

#endif // _IRIS_PHYSX_NORMAL_MAP_VTABLE_