/*++

Copyright (c) 2021 Brad Weinberger

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
    _In_ PCINTERSECTION intersection,
    _In_ VECTOR3 model_geometry_normal,
    _In_ VECTOR3 world_geometry_normal,
    _In_ const void *additional_data,
    _In_ const void *texture_coordinates,
    _Out_ PVECTOR3 shading_normal,
    _Out_ PNORMAL_COORDINATE_SPACE coordinate_space
    )
{
    assert(normal_map != NULL);
    assert(intersection != NULL);
    assert(VectorValidate(model_geometry_normal));
    assert(VectorValidate(world_geometry_normal));
    assert(shading_normal != NULL);
    assert(coordinate_space != NULL);

    ISTATUS status = normal_map->vtable->compute_routine(normal_map->data,
                                                         intersection,
                                                         model_geometry_normal,
                                                         world_geometry_normal,
                                                         additional_data,
                                                         texture_coordinates,
                                                         shading_normal,
                                                         coordinate_space);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    if (!VectorValidate(*shading_normal))
    {
        return ISTATUS_INVALID_RESULT;
    }

    return ISTATUS_SUCCESS;
}

#endif // _IRIS_PHYSX_NORMAL_MAP_INTERNAL_