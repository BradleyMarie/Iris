/*++

Copyright (c) 2020 Brad Weinberger

Module Name:

    normal_map_vtable.h

Abstract:

    The vtable for a normal map.

--*/

#ifndef _IRIS_PHYSX_NORMAL_MAP_VTABLE_
#define _IRIS_PHYSX_NORMAL_MAP_VTABLE_

#include "common/free_routine.h"
#include "iris/iris.h"

//
// Enums
//

typedef enum _NORMAL_COORDINATE_SPACE {
    NORMAL_MODEL_COORDINATE_SPACE = 0,
    NORMAL_TANGENT_COORDINATE_SPACE = 1
} NORMAL_COORDINATE_SPACE;

//
// Types
//

typedef
ISTATUS
(*PNORMAL_MAP_COMPUTE_ROUTINE)(
    _In_ const void *context,
    _In_ POINT3 hit_point,
    _In_ const void *additional_data,
    _In_ const void *texture_coordinates,
    _Out_ PVECTOR3 normal
    );

typedef struct _NORMAL_MAP_VTABLE {
    NORMAL_COORDINATE_SPACE coordinate_space;
    PNORMAL_MAP_COMPUTE_ROUTINE compute_routine;
    PFREE_ROUTINE free_routine;
} NORMAL_MAP_VTABLE, *PNORMAL_MAP_VTABLE;

typedef const NORMAL_MAP_VTABLE *PCNORMAL_MAP_VTABLE;

#endif // _IRIS_PHYSX_NORMAL_MAP_VTABLE_