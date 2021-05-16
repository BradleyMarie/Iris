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
#include "iris/iris.h"

//
// Types
//

typedef
ISTATUS
(*PNORMAL_MAP_COMPUTE_ROUTINE)(
    _In_ const void *context,
    _In_ POINT3 hit_point,
    _In_ VECTOR3 geometry_normal,
    _In_ const void *additional_data,
    _In_ const void *texture_coordinates,
    _Out_ PVECTOR3 shading_normal
    );

typedef struct _NORMAL_MAP_VTABLE {
    PNORMAL_MAP_COMPUTE_ROUTINE compute_routine;
    PFREE_ROUTINE free_routine;
} NORMAL_MAP_VTABLE, *PNORMAL_MAP_VTABLE;

typedef const NORMAL_MAP_VTABLE *PCNORMAL_MAP_VTABLE;

#endif // _IRIS_PHYSX_NORMAL_MAP_VTABLE_