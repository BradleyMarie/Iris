/*++

Copyright (c) 2021 Brad Weinberger

Module Name:

    triangle_mesh_normal_map.c

Abstract:

    Implements a triangle mesh normal map.

--*/

#include <stdalign.h>
#include <stdatomic.h>
#include <stdlib.h>
#include <string.h>

#include "iris_physx_toolkit/triangle_mesh_normal_map.h"
#include "iris_physx_toolkit/triangle_mesh.h"

//
// Types
//

typedef struct _TRIANGLE_MESH_NORMAL_MAP {
    _Field_size_(num_vertices) PVECTOR3 normals;
    size_t num_vertices;
    atomic_uintmax_t *reference_count;
} TRIANGLE_MESH_NORMAL_MAP, *PTRIANGLE_MESH_NORMAL_MAP;

typedef const TRIANGLE_MESH_NORMAL_MAP *PCTRIANGLE_MESH_NORMAL_MAP;

//
// Static Functions
//

static
ISTATUS
TriangleMeshNormalMapCompute(
    _In_ const void *context,
    _In_ PCINTERSECTION intersection,
    _In_ VECTOR3 model_geometry_normal,
    _In_ VECTOR3 world_geometry_normal,
    _In_ const void *additional_data,
    _In_ const void *texture_coordinates,
    _Out_ PVECTOR3 shading_normal,
    _Out_ PNORMAL_COORDINATE_SPACE coordinate_space
    )
{
    PCTRIANGLE_MESH_NORMAL_MAP normal_map = (PTRIANGLE_MESH_NORMAL_MAP)context;
    PCTRIANGLE_MESH_ADDITIONAL_DATA hit_data =
        (PCTRIANGLE_MESH_ADDITIONAL_DATA)additional_data;

    if (normal_map->num_vertices < hit_data->vertex_indices[0] ||
        normal_map->num_vertices < hit_data->vertex_indices[1] ||
        normal_map->num_vertices < hit_data->vertex_indices[2])
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    *shading_normal =
        VectorScale(normal_map->normals[hit_data->vertex_indices[0]],
                    hit_data->barycentric_coordinates[0]);

    *shading_normal =
        VectorAddScaled(*shading_normal,
                        normal_map->normals[hit_data->vertex_indices[1]],
                        hit_data->barycentric_coordinates[1]);

    *shading_normal =
        VectorAddScaled(*shading_normal,
                        normal_map->normals[hit_data->vertex_indices[2]],
                        hit_data->barycentric_coordinates[2]);

    *shading_normal = VectorNormalize(*shading_normal, NULL, NULL);
    *coordinate_space = NORMAL_MODEL_COORDINATE_SPACE;

    return ISTATUS_SUCCESS;
}

static
ISTATUS
TriangleMeshNormalMapComputeBack(
    _In_ const void *context,
    _In_ PCINTERSECTION intersection,
    _In_ VECTOR3 model_geometry_normal,
    _In_ VECTOR3 world_geometry_normal,
    _In_ const void *additional_data,
    _In_ const void *texture_coordinates,
    _Out_ PVECTOR3 shading_normal,
    _Out_ PNORMAL_COORDINATE_SPACE coordinate_space
    )
{
    ISTATUS status = TriangleMeshNormalMapCompute(context,
                                                  intersection,
                                                  model_geometry_normal,
                                                  world_geometry_normal,
                                                  additional_data,
                                                  texture_coordinates,
                                                  shading_normal,
                                                  coordinate_space);

    *shading_normal = VectorNegate(*shading_normal);

    return status;
}

static
void
TriangleMeshNormalMapFree(
    _In_opt_ _Post_invalid_ void *context
    )
{
    PTRIANGLE_MESH_NORMAL_MAP normal_map = (PTRIANGLE_MESH_NORMAL_MAP)context;

    if (atomic_fetch_sub(normal_map->reference_count, 1) == 1)
    {
        free(normal_map->reference_count);
        free(normal_map->normals);
    }
}

//
// Static Variables
//

static const NORMAL_MAP_VTABLE front_triangle_mesh_normal_map_vtable = {
    TriangleMeshNormalMapCompute,
    TriangleMeshNormalMapFree
};

static const NORMAL_MAP_VTABLE back_triangle_mesh_normal_map_vtable = {
    TriangleMeshNormalMapComputeBack,
    TriangleMeshNormalMapFree
};

//
// Functions
//

ISTATUS
TriangleMeshNormalMapAllocate(
    _In_reads_(num_vertices) const VECTOR3 normals[],
    _In_ size_t num_vertices,
    _Out_ PNORMAL_MAP *front_normal_map,
    _Out_ PNORMAL_MAP *back_normal_map
    )
{
    if (normals == NULL && num_vertices != 0)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (front_normal_map == NULL && back_normal_map == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_COMBINATION_00;
    }

    TRIANGLE_MESH_NORMAL_MAP normal_map;
    normal_map.reference_count = malloc(sizeof(atomic_uintmax_t));

    if (normal_map.reference_count == NULL)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    if (front_normal_map == NULL || back_normal_map == NULL)
    {
        *normal_map.reference_count = 1;
    }
    else
    {
        *normal_map.reference_count = 2;
    }

    normal_map.normals = (PVECTOR3)calloc(num_vertices, sizeof(VECTOR3));

    if (normal_map.normals == NULL)
    {
        free(normal_map.reference_count);
        return ISTATUS_ALLOCATION_FAILED;
    }

    memcpy(normal_map.normals, normals, num_vertices * sizeof(VECTOR3));

    normal_map.num_vertices = num_vertices;

    if (front_normal_map == NULL)
    {
        ISTATUS status =
            NormalMapAllocate(&back_triangle_mesh_normal_map_vtable,
                              &normal_map,
                              sizeof(TRIANGLE_MESH_NORMAL_MAP),
                              alignof(TRIANGLE_MESH_NORMAL_MAP),
                              back_normal_map);

        if (status != ISTATUS_SUCCESS)
        {
            free(normal_map.reference_count);
            free(normal_map.normals);
        }

        return status;
    }

    if (back_normal_map == NULL)
    {
        ISTATUS status =
            NormalMapAllocate(&front_triangle_mesh_normal_map_vtable,
                              &normal_map,
                              sizeof(TRIANGLE_MESH_NORMAL_MAP),
                              alignof(TRIANGLE_MESH_NORMAL_MAP),
                              front_normal_map);

        if (status != ISTATUS_SUCCESS)
        {
            free(normal_map.reference_count);
            free(normal_map.normals);
        }

        return status;
    }

    PNORMAL_MAP tmp_front_normal_map;
    ISTATUS status =
        NormalMapAllocate(&front_triangle_mesh_normal_map_vtable,
                          &normal_map,
                          sizeof(TRIANGLE_MESH_NORMAL_MAP),
                          alignof(TRIANGLE_MESH_NORMAL_MAP),
                          &tmp_front_normal_map);

    if (status != ISTATUS_SUCCESS)
    {
        free(normal_map.reference_count);
        free(normal_map.normals);
        return status;
    }

    status =
        NormalMapAllocate(&back_triangle_mesh_normal_map_vtable,
                          &normal_map,
                          sizeof(TRIANGLE_MESH_NORMAL_MAP),
                          alignof(TRIANGLE_MESH_NORMAL_MAP),
                          back_normal_map);

    if (status != ISTATUS_SUCCESS)
    {
        NormalMapRelease(tmp_front_normal_map);
        free(normal_map.reference_count);
        free(normal_map.normals);
        return status;
    }

    *front_normal_map = tmp_front_normal_map;

    return ISTATUS_SUCCESS;
}