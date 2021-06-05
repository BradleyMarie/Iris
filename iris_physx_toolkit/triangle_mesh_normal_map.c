/*++

Copyright (c) 2021 Brad Weinberger

Module Name:

    triangle_mesh_normal_map.c

Abstract:

    Implements a triangle mesh normal map.

--*/

#include <stdalign.h>
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

    if (VectorDotProduct(model_geometry_normal, *shading_normal) < (float_t)0.0)
    {
        *shading_normal = VectorNegate(*shading_normal);
    }

    *coordinate_space = NORMAL_MODEL_COORDINATE_SPACE;

    return ISTATUS_SUCCESS;
}

static
void
TriangleMeshNormalMapFree(
    _In_opt_ _Post_invalid_ void *context
    )
{
    PTRIANGLE_MESH_NORMAL_MAP normal_map = (PTRIANGLE_MESH_NORMAL_MAP)context;

    free(normal_map->normals);
}

//
// Static Variables
//

static const NORMAL_MAP_VTABLE triangle_mesh_normal_map_vtable = {
    TriangleMeshNormalMapCompute,
    TriangleMeshNormalMapFree
};

//
// Functions
//

ISTATUS
TriangleMeshNormalMapAllocate(
    _In_reads_(num_vertices) const VECTOR3 normals[],
    _In_ size_t num_vertices,
    _Out_ PNORMAL_MAP *normal_map
    )
{
    if (normals == NULL && num_vertices != 0)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (normal_map == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    TRIANGLE_MESH_NORMAL_MAP triangle_normal_map;

    triangle_normal_map.normals =
        (PVECTOR3)calloc(num_vertices, sizeof(VECTOR3));

    if (triangle_normal_map.normals == NULL)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    memcpy(triangle_normal_map.normals,
           normals,
           num_vertices * sizeof(VECTOR3));

    triangle_normal_map.num_vertices = num_vertices;

    ISTATUS status =
        NormalMapAllocate(&triangle_mesh_normal_map_vtable,
                          &triangle_normal_map,
                          sizeof(TRIANGLE_MESH_NORMAL_MAP),
                          alignof(TRIANGLE_MESH_NORMAL_MAP),
                          normal_map);

    if (status != ISTATUS_SUCCESS)
    {
        free(triangle_normal_map.normals);
        return status;
    }

    return ISTATUS_SUCCESS;
}