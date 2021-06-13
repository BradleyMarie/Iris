/*++

Copyright (c) 2021 Brad Weinberger

Module Name:

    triangle_mesh_texture_coordinate_map.c

Abstract:

    Implements a triangle mesh texture_coordinate map.

--*/

#include <stdalign.h>
#include <stdlib.h>
#include <string.h>

#include "common/safe_math.h"
#include "iris_physx_toolkit/triangle_mesh_texture_coordinate_map.h"
#include "iris_physx_toolkit/shapes/triangle_mesh.h"
#include "iris_physx_toolkit/uv_texture_coordinate.h"

//
// Types
//

_Struct_size_bytes_(sizeof(size_t) + num_vertices * sizeof(float_t))
typedef struct _TRIANGLE_MESH_TEXTURE_COORDINATE_MAP {
    size_t num_vertices;
    _Field_size_(num_vertices) float_t texture_coordinates[][2];
} TRIANGLE_MESH_TEXTURE_COORDINATE_MAP, *PTRIANGLE_MESH_TEXTURE_COORDINATE_MAP;

typedef const TRIANGLE_MESH_TEXTURE_COORDINATE_MAP *PCTRIANGLE_MESH_TEXTURE_COORDINATE_MAP;

//
// Static Functions
//

static
ISTATUS
TriangleMeshTextureCoordinateMapCompute(
    _In_ const void *context,
    _In_ PCINTERSECTION intersection,
    _In_opt_ PCMATRIX model_to_world,
    _In_ const void *additional_data,
    _Inout_ PTEXTURE_COORDINATE_ALLOCATOR allocator,
    _Out_ void **texture_coordinates
    )
{
    PCTRIANGLE_MESH_TEXTURE_COORDINATE_MAP map =
        (PTRIANGLE_MESH_TEXTURE_COORDINATE_MAP)context;
    PCTRIANGLE_MESH_ADDITIONAL_DATA hit_data =
        (PCTRIANGLE_MESH_ADDITIONAL_DATA)additional_data;

    if (map->num_vertices < hit_data->vertex_indices[0] ||
        map->num_vertices < hit_data->vertex_indices[1] ||
        map->num_vertices < hit_data->vertex_indices[2])
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    ISTATUS status =
        TextureCoordinateAllocatorAllocate(allocator,
                                           sizeof(UV_TEXTURE_COORDINATE),
                                           alignof(UV_TEXTURE_COORDINATE),
                                           texture_coordinates);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    PUV_TEXTURE_COORDINATE uv_coordinates =
        (PUV_TEXTURE_COORDINATE)*texture_coordinates;

    float_t uv[2];
    uv[0] = map->texture_coordinates[hit_data->vertex_indices[0]][0] *
            hit_data->barycentric_coordinates[0];

    uv[1] = map->texture_coordinates[hit_data->vertex_indices[0]][1] *
            hit_data->barycentric_coordinates[0];

    uv[0] += map->texture_coordinates[hit_data->vertex_indices[1]][0] *
             hit_data->barycentric_coordinates[1];

    uv[1] += map->texture_coordinates[hit_data->vertex_indices[1]][1] *
             hit_data->barycentric_coordinates[1];

    uv[0] += map->texture_coordinates[hit_data->vertex_indices[2]][0] *
             hit_data->barycentric_coordinates[2];

    uv[1] += map->texture_coordinates[hit_data->vertex_indices[2]][1] *
             hit_data->barycentric_coordinates[2];

    if (!intersection->has_derivatives)
    {
        UVTextureCoordinateInitializeWithoutDerivatives(uv_coordinates, uv);
        return ISTATUS_SUCCESS;
    }

    float_t duv02[2];
    duv02[0] = map->texture_coordinates[hit_data->vertex_indices[0]][0] -
               map->texture_coordinates[hit_data->vertex_indices[2]][0];
    duv02[1] = map->texture_coordinates[hit_data->vertex_indices[0]][1] -
               map->texture_coordinates[hit_data->vertex_indices[2]][1];

    float_t duv12[2];
    duv12[0] = map->texture_coordinates[hit_data->vertex_indices[1]][0] -
               map->texture_coordinates[hit_data->vertex_indices[2]][0];
    duv12[1] = map->texture_coordinates[hit_data->vertex_indices[1]][1] -
               map->texture_coordinates[hit_data->vertex_indices[2]][1];

    float_t determinant = duv02[0] * duv12[1] - duv02[1] * duv12[0];
    if (fabs(determinant) < (float_t)0.0000001)
    {
        UVTextureCoordinateInitializeWithoutDerivatives(uv_coordinates, uv);
        return ISTATUS_SUCCESS;
    }

    VECTOR3 d02 =
        PointSubtract(hit_data->vertices[hit_data->vertex_indices[0]],
                      hit_data->vertices[hit_data->vertex_indices[2]]);

    VECTOR3 d12 =
        PointSubtract(hit_data->vertices[hit_data->vertex_indices[1]],
                      hit_data->vertices[hit_data->vertex_indices[2]]);

    float_t inverse_determinant = (float_t)1.0 / determinant;

    VECTOR3 dmodel_hit_point_du =
        VectorSubtract(VectorScale(d02, duv12[1]),
                       VectorScale(d12, duv02[1]));
    dmodel_hit_point_du = VectorScale(dmodel_hit_point_du, inverse_determinant);

    VECTOR3 dmodel_hit_point_dv =
        VectorSubtract(VectorScale(d12, duv02[0]),
                       VectorScale(d02, duv12[0]));
    dmodel_hit_point_dv = VectorScale(dmodel_hit_point_dv, inverse_determinant);

    UVTextureCoordinateInitialize(uv_coordinates,
                                  uv,
                                  model_to_world,
                                  dmodel_hit_point_du,
                                  dmodel_hit_point_dv,
                                  intersection->model_dp_dx,
                                  intersection->model_dp_dy);

    return ISTATUS_SUCCESS;
}

//
// Static Variables
//

static const TEXTURE_COORDINATE_MAP_VTABLE triangle_mesh_texture_coordinate_map_vtable = {
    TriangleMeshTextureCoordinateMapCompute,
    NULL
};

//
// Functions
//

ISTATUS
TriangleMeshTextureCoordinateMapAllocate(
    _In_reads_(num_vertices) const float_t texture_coordinates[][2],
    _In_ size_t num_vertices,
    _Out_ PTEXTURE_COORDINATE_MAP *texture_coordinate_map
    )
{
    if (texture_coordinates == NULL && num_vertices != 0)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (texture_coordinate_map == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_COMBINATION_02;
    }

    size_t size;
    bool success = CheckedMultiplySizeT(sizeof(float_t) * 2,
                                        num_vertices,
                                        &size);

    if (!success)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    success = CheckedAddSizeT(size, sizeof(size_t), &size);

    if (!success)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    PTRIANGLE_MESH_TEXTURE_COORDINATE_MAP map =
        (PTRIANGLE_MESH_TEXTURE_COORDINATE_MAP)malloc(size);

    if (map == NULL)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    memcpy(map->texture_coordinates,
           texture_coordinates,
           2 * num_vertices * sizeof(float_t));

    map->num_vertices = num_vertices;

    ISTATUS status =
        TextureCoordinateMapAllocate(&triangle_mesh_texture_coordinate_map_vtable,
                                     map,
                                     size,
                                     alignof(TRIANGLE_MESH_TEXTURE_COORDINATE_MAP),
                                     texture_coordinate_map);

    free(map);

    return status;
}