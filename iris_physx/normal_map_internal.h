/*++

Copyright (c) 2019 Brad Weinberger

Module Name:

    normal_map_internal.h

Abstract:

    The internal routines for a normal map.

--*/

#ifndef _IRIS_PHYSX_NORMAL_MAP_INTERNAL_
#define _IRIS_PHYSX_NORMAL_MAP_INTERNAL_

#include <stdatomic.h>

#include "iris_physx/normal_map_vtable.h"

//
// Types
//

struct _NORMAL_MAP {
    PCNORMAL_MAP_VTABLE vtable;
    void *data;
    atomic_uintmax_t reference_count;
};

//
// Functions
//

static
inline
ISTATUS
NormalMapCompute(
    _In_ const struct _NORMAL_MAP *normal_map,
    _In_ POINT3 hit_point,
    _In_ const void *additional_data,
    _In_ const void *texture_coordinates,
    _Out_ PVECTOR3 normal
    )
{
    assert(normal_map != NULL);
    assert(PointValidate(hit_point));
    assert(normal != NULL);

    ISTATUS status = normal_map->vtable->compute_routine(normal_map->data,
                                                         hit_point,
                                                         additional_data,
                                                         texture_coordinates,
                                                         normal);

    return status;
}

#endif // _IRIS_PHYSX_NORMAL_MAP_INTERNAL_