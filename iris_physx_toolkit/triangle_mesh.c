/*++

Copyright (c) 2019 Brad Weinberger

Module Name:

    triangle_mesh.c

Abstract:

    Implements a triangle mesh.

--*/

#include <stdalign.h>
#include <stdatomic.h>
#include <stdlib.h>
#include <string.h>

#include "iris_physx_toolkit/triangle_mesh.h"

//
// Constants
//

#define TRIANGLE_DEGENERATE_THRESHOLD (float_t)0.001

//
// Types
//

typedef struct _TRIANGLE_MESH {
    PPOINT3 vertices;
    PVECTOR3 normals;
    PMATERIAL materials[2];
    PEMISSIVE_MATERIAL emissive_materials[2];
    atomic_uintmax_t reference_count;
} TRIANGLE_MESH, *PTRIANGLE_MESH;

typedef const TRIANGLE_MESH *PCTRIANGLE_MESH;

typedef struct _TRIANGLE {
    PTRIANGLE_MESH mesh;
    uint32_t v0;
    uint32_t v1;
    uint32_t v2;
    VECTOR3 surface_normal;
    uint32_t mesh_face_index;
} TRIANGLE, *PTRIANGLE;

typedef const TRIANGLE *PCTRIANGLE;

typedef struct _EMISSIVE_TRIANGLE {
    TRIANGLE triangle;
    float_t area;
} EMISSIVE_TRIANGLE, *PEMISSIVE_TRIANGLE;

typedef const EMISSIVE_TRIANGLE *PCEMISSIVE_TRIANGLE;

//
// Static Triangle Functions
//

static
inline
POINT3
PointSubtractPoint(
    _In_ POINT3 p0,
    _In_ POINT3 p1
    )
{
    float_t x = p0.x - p1.x;
    float_t y = p0.y - p1.y;
    float_t z = p0.z - p1.z;

    return PointCreate(x, y, z);
}

static
inline
POINT3
PointPermuteXDominant(
    _In_ POINT3 point
    )
{
    return PointCreate(point.z, point.y, point.x);
}

static
inline
POINT3
PointPermuteYDominant(
    _In_ POINT3 point
    )
{
    return PointCreate(point.x, point.z, point.y);
}

static
inline
POINT3
PointPermuteZDominant(
    _In_ POINT3 point
    )
{
    return PointCreate(point.y, point.x, point.z);
}

static
inline
VECTOR3
VectorPermuteXDominant(
    _In_ VECTOR3 vector
    )
{
    return VectorCreate(vector.z, vector.y, vector.x);
}

static
inline
VECTOR3
VectorPermuteYDominant(
    _In_ VECTOR3 vector
    )
{
    return VectorCreate(vector.x, vector.z, vector.y);
}

static
inline
VECTOR3
VectorPermuteZDominant(
    _In_ VECTOR3 vector
    )
{
    return VectorCreate(vector.y, vector.x, vector.z);
}

static
ISTATUS
TriangleTrace(
    _In_ const void *context,
    _In_ PCRAY ray,
    _In_ PSHAPE_HIT_ALLOCATOR allocator,
    _Out_ PHIT *hit
    )
{
    PCTRIANGLE triangle = (PCTRIANGLE)context;

    POINT3 v0 = PointSubtractPoint(
        triangle->mesh->vertices[triangle->v0], ray->origin);
    POINT3 v1 = PointSubtractPoint(
        triangle->mesh->vertices[triangle->v1], ray->origin);
    POINT3 v2 = PointSubtractPoint(
        triangle->mesh->vertices[triangle->v2], ray->origin);

    VECTOR_AXIS dominant_axis = VectorDominantAxis(ray->direction);

    VECTOR3 direction;
    switch (dominant_axis)
    {
        case VECTOR_X_AXIS:
            direction = VectorPermuteXDominant(ray->direction);
            v0 = PointPermuteXDominant(v0);
            v1 = PointPermuteXDominant(v1);
            v2 = PointPermuteXDominant(v2);
            break;
        case VECTOR_Y_AXIS:
            direction = VectorPermuteYDominant(ray->direction);
            v0 = PointPermuteYDominant(v0);
            v1 = PointPermuteYDominant(v1);
            v2 = PointPermuteYDominant(v2);
            break;
        case VECTOR_Z_AXIS:
            direction = VectorPermuteZDominant(ray->direction);
            v0 = PointPermuteZDominant(v0);
            v1 = PointPermuteZDominant(v1);
            v2 = PointPermuteZDominant(v2);
            break;
    }

    float_t shear_x = -direction.x / direction.z;
    float_t shear_y = -direction.y / direction.z;

    v0.x = fma(shear_x, v0.z, v0.x);
    v0.y = fma(shear_y, v0.z, v0.y);
    v1.x = fma(shear_x, v1.z, v1.x);
    v1.y = fma(shear_y, v1.z, v1.y);
    v2.x = fma(shear_x, v2.z, v2.x);
    v2.y = fma(shear_y, v2.z, v2.y);

    TRIANGLE_MESH_ADDITIONAL_DATA data;
    data.barycentric_coordinates[0] = v1.x * v2.y - v1.y * v2.x;
    data.barycentric_coordinates[1] = v2.x * v0.y - v2.y * v0.x;
    data.barycentric_coordinates[2] = v0.x * v1.y - v0.y * v1.x;

#if FLT_EVAL_METHOD == 0
    if (data.barycentric_coordinates[0] == (float_t)0.0 ||
        data.barycentric_coordinates[1] == (float_t)0.0 ||
        data.barycentric_coordinates[2] == (float_t)0.0)
    {
        data.barycentric_coordinates[0] =
            ((double_t)v1.x * (double_t)v2.y - (double_t)v1.y * (double_t)v2.x);
        data.barycentric_coordinates[1] =
            ((double_t)v2.x * (double_t)v0.y - (double_t)v2.y * (double_t)v0.x);
        data.barycentric_coordinates[2] =
            ((double_t)v0.x * (double_t)v1.y - (double_t)v0.y * (double_t)v1.x);
    }
#endif

    if ((data.barycentric_coordinates[0] < (float_t)0.0 ||
         data.barycentric_coordinates[1] < (float_t)0.0 ||
         data.barycentric_coordinates[2] < (float_t)0.0) &&
        (data.barycentric_coordinates[0] > (float_t)0.0 ||
         data.barycentric_coordinates[1] > (float_t)0.0 ||
         data.barycentric_coordinates[2] > (float_t)0.0))
    {
        return ISTATUS_NO_INTERSECTION;
    }

    float_t determinant = data.barycentric_coordinates[0] +
                          data.barycentric_coordinates[1] +
                          data.barycentric_coordinates[2];

    if (determinant == (float_t)0.0)
    {
        return ISTATUS_NO_INTERSECTION;
    }

    float_t shear_z = (float_t)1.0 / direction.z;
    v0.z = v0.z * shear_z;
    v1.z = v1.z * shear_z;
    v2.z = v2.z * shear_z;

    float_t distance = data.barycentric_coordinates[0] * v0.z +
                       data.barycentric_coordinates[1] * v1.z +
                       data.barycentric_coordinates[2] * v2.z;

    float_t inverse_determinant = (float_t)1.0 / determinant;
    distance *= inverse_determinant;
    data.barycentric_coordinates[0] *= inverse_determinant;
    data.barycentric_coordinates[1] *= inverse_determinant;
    data.barycentric_coordinates[2] *= inverse_determinant;
    data.mesh_face_index = triangle->mesh_face_index;

    float_t dp = VectorDotProduct(ray->direction, triangle->surface_normal);

    uint32_t front_face, back_face;
    if (dp < (float_t)0.0)
    {
        front_face = TRIANGLE_MESH_FRONT_FACE;
        back_face = TRIANGLE_MESH_BACK_FACE;
    }
    else
    {
        front_face = TRIANGLE_MESH_BACK_FACE;
        back_face = TRIANGLE_MESH_FRONT_FACE;
    }

    ISTATUS status =
        ShapeHitAllocatorAllocate(allocator,
                                  NULL,
                                  distance,
                                  front_face,
                                  back_face,
                                  &data,
                                  sizeof(TRIANGLE_MESH_ADDITIONAL_DATA),
                                  alignof(TRIANGLE_MESH_ADDITIONAL_DATA),
                                  hit);

    return status;
}

static
ISTATUS
TriangleComputeBounds(
    _In_ const void *context,
    _In_opt_ PCMATRIX model_to_world,
    _Out_ PBOUNDING_BOX world_bounds
    )
{
    PCTRIANGLE triangle = (PCTRIANGLE)context;

    POINT3 world_v0 = PointMatrixMultiply(
        model_to_world, triangle->mesh->vertices[triangle->v0]);
    POINT3 world_v1 = PointMatrixMultiply(
        model_to_world, triangle->mesh->vertices[triangle->v1]);
    POINT3 world_v2 = PointMatrixMultiply(
        model_to_world, triangle->mesh->vertices[triangle->v2]);

    float_t min_x = fmin(world_v0.x, fmin(world_v1.x, world_v2.x));
    float_t min_y = fmin(world_v0.y, fmin(world_v1.y, world_v2.y));
    float_t min_z = fmin(world_v0.z, fmin(world_v1.z, world_v2.z));
    POINT3 bottom = PointCreate(min_x, min_y, min_z);

    float_t max_x = fmax(world_v0.x, fmax(world_v1.x, world_v2.x));
    float_t max_y = fmax(world_v0.y, fmax(world_v1.y, world_v2.y));
    float_t max_z = fmax(world_v0.z, fmax(world_v1.z, world_v2.z));
    POINT3 top = PointCreate(max_x, max_y, max_z);

    *world_bounds = BoundingBoxCreate(bottom, top);

    return ISTATUS_SUCCESS;
}

static
ISTATUS
TriangleComputeNormal(
    _In_ const void *context,
    _In_ POINT3 hit_point,
    _In_ uint32_t face_hit,
    _Out_ PVECTOR3 surface_normal
    )
{
    if (face_hit > TRIANGLE_MESH_BACK_FACE)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    PCTRIANGLE triangle = (PCTRIANGLE)context;

    if (face_hit == TRIANGLE_MESH_FRONT_FACE)
    {
        *surface_normal = triangle->surface_normal;
    }
    else
    {
        *surface_normal = VectorNegate(triangle->surface_normal);
    }

    return ISTATUS_SUCCESS;
}

static
ISTATUS
TriangleGetMaterial(
    _In_opt_ const void *context,
    _In_ uint32_t face_hit,
    _Outptr_result_maybenull_ PCMATERIAL *material
    )
{
    if (face_hit > TRIANGLE_MESH_BACK_FACE)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    PCTRIANGLE triangle = (PCTRIANGLE)context;
    *material = triangle->mesh->materials[face_hit];

    return ISTATUS_SUCCESS;
}

ISTATUS
TriangleComputeShadingNormal(
    _In_opt_ const void *context,
    _In_ POINT3 hit_point,
    _In_ uint32_t face_hit,
    _In_ const void *additional_data,
    _Out_ PVECTOR3 normal
    )
{
    PCTRIANGLE triangle = (PCTRIANGLE)context;
    PCTRIANGLE_MESH mesh = triangle->mesh;
    PCTRIANGLE_MESH_ADDITIONAL_DATA hit_data =
        (PCTRIANGLE_MESH_ADDITIONAL_DATA)additional_data;

    *normal = VectorScale(mesh->normals[triangle->v0],
                          hit_data->barycentric_coordinates[0]);

    *normal = VectorAddScaled(*normal,
                              mesh->normals[triangle->v1],
                              hit_data->barycentric_coordinates[1]);

    *normal = VectorAddScaled(*normal,
                              mesh->normals[triangle->v2],
                              hit_data->barycentric_coordinates[2]);

    *normal = VectorNormalize(*normal, NULL, NULL);

    if (face_hit == TRIANGLE_MESH_BACK_FACE)
    {
        *normal = VectorNegate(*normal);
    }

    return ISTATUS_SUCCESS;
}

static
void
TriangleMeshFree(
    _In_opt_ _Post_invalid_ PTRIANGLE_MESH mesh
    )
{
    free(mesh->vertices);
    free(mesh->normals);
    MaterialRelease(mesh->materials[0]);
    MaterialRelease(mesh->materials[1]);
    EmissiveMaterialRelease(mesh->emissive_materials[0]);
    EmissiveMaterialRelease(mesh->emissive_materials[1]);
    free(mesh);
}

static
void
TriangleFree(
    _In_opt_ _Post_invalid_ void *context
    )
{
    PTRIANGLE triangle = (PTRIANGLE)context;

    if (atomic_fetch_sub(&triangle->mesh->reference_count, 1) == 1)
    {
        TriangleMeshFree(triangle->mesh);
    }
}

static
bool
TriangleInitialize(
    _In_ PTRIANGLE_MESH mesh,
    _In_ uint32_t v0,
    _In_ uint32_t v1,
    _In_ uint32_t v2,
    _In_ uint32_t mesh_face_index,
    _Out_opt_ float_t *area,
    _Out_ PTRIANGLE triangle
    )
{
    assert(mesh != NULL);
    assert(triangle != NULL);

    triangle->mesh = mesh;
    triangle->v0 = v0;
    triangle->v1 = v1;
    triangle->v2 = v2;
    triangle->mesh_face_index = mesh_face_index;

    VECTOR3 v0_to_v1 = PointSubtract(mesh->vertices[v1], mesh->vertices[v0]);
    VECTOR3 v0_to_v2 = PointSubtract(mesh->vertices[v2], mesh->vertices[v0]);

    triangle->surface_normal = VectorCrossProduct(v0_to_v1, v0_to_v2);
    float_t surface_normal_length = VectorLength(triangle->surface_normal);
    if (surface_normal_length <= TRIANGLE_DEGENERATE_THRESHOLD)
    {
        return false;
    }

    float_t scalar = (float_t)1.0 / surface_normal_length;
    triangle->surface_normal = VectorScale(triangle->surface_normal, scalar);

    if (area != NULL)
    {
        *area = surface_normal_length * (float_t)0.5;
    }

    return true;
}

//
// Static Emissive Triangle Functions
//

static
ISTATUS
EmissiveTriangleGetEmissiveMaterial(
    _In_opt_ const void *context,
    _In_ uint32_t face_hit,
    _Outptr_result_maybenull_ PCEMISSIVE_MATERIAL *emissive_material
    )
{
    if (face_hit > TRIANGLE_MESH_BACK_FACE)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    PCEMISSIVE_TRIANGLE triangle = (PCEMISSIVE_TRIANGLE)context;
    *emissive_material = triangle->triangle.mesh->emissive_materials[face_hit];

    return ISTATUS_SUCCESS;
}

static
ISTATUS
EmissiveTriangleSampleFace(
    _In_opt_ const void *context,
    _In_ uint32_t face_hit,
    _Inout_ PRANDOM rng,
    _Out_ PPOINT3 sampled_point
    )
{
    PEMISSIVE_TRIANGLE triangle = (PEMISSIVE_TRIANGLE)context;

    float_t u;
    ISTATUS status = RandomGenerateFloat(rng, (float_t)0.0f, (float_t)1.0f, &u);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    float_t v;
    status = RandomGenerateFloat(rng, (float_t)0.0f, (float_t)1.0f, &v);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    if ((float_t)1.0 < u + v)
    {
        u = (float_t)1.0 - u;
        v = (float_t)1.0 - v;
    }

    VECTOR3 v0_to_v1 = PointSubtract(
        triangle->triangle.mesh->vertices[triangle->triangle.v1],
        triangle->triangle.mesh->vertices[triangle->triangle.v0]);
    VECTOR3 v0_to_v2 = PointSubtract(
        triangle->triangle.mesh->vertices[triangle->triangle.v2],
        triangle->triangle.mesh->vertices[triangle->triangle.v0]);

    POINT3 sum = PointVectorAddScaled(
        triangle->triangle.mesh->vertices[triangle->triangle.v0], v0_to_v1, u);
    *sampled_point = PointVectorAddScaled(sum, v0_to_v2, v);

    return ISTATUS_SUCCESS;
}

ISTATUS
EmissiveTriangleComputePdfBySolidArea(
    _In_opt_ const void *context,
    _In_ PCRAY to_shape,
    _In_ float_t distance,
    _In_ uint32_t face_hit,
    _Out_ float_t *pdf
    )
{
    PEMISSIVE_TRIANGLE triangle = (PEMISSIVE_TRIANGLE)context;

    float_t dp = VectorDotProduct(triangle->triangle.surface_normal,
                                  to_shape->direction);

    if (face_hit == TRIANGLE_MESH_FRONT_FACE)
    {
        dp = -dp;
    }

    *pdf = (distance * distance) / (dp * triangle->area);

    return ISTATUS_SUCCESS;
}

//
// Static Variables
//

static const SHAPE_VTABLE triangle_vtable = {
    TriangleTrace,
    TriangleComputeBounds,
    TriangleComputeNormal,
    TriangleGetMaterial,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    TriangleFree
};

static const SHAPE_VTABLE triangle_with_normals_vtable = {
    TriangleTrace,
    TriangleComputeBounds,
    TriangleComputeNormal,
    TriangleGetMaterial,
    NULL,
    NULL,
    NULL,
    TriangleComputeShadingNormal,
    NULL,
    NULL,
    TriangleFree
};

static const SHAPE_VTABLE emissive_triangle_vtable = {
    TriangleTrace,
    TriangleComputeBounds,
    TriangleComputeNormal,
    TriangleGetMaterial,
    EmissiveTriangleGetEmissiveMaterial,
    EmissiveTriangleSampleFace,
    EmissiveTriangleComputePdfBySolidArea,
    NULL,
    NULL,
    NULL,
    TriangleFree
};

static const SHAPE_VTABLE emissive_triangle_with_normals_vtable = {
    TriangleTrace,
    TriangleComputeBounds,
    TriangleComputeNormal,
    TriangleGetMaterial,
    EmissiveTriangleGetEmissiveMaterial,
    EmissiveTriangleSampleFace,
    EmissiveTriangleComputePdfBySolidArea,
    TriangleComputeShadingNormal,
    NULL,
    NULL,
    TriangleFree
};

//
// Static Functions
//

static
ISTATUS
TriangleMeshValidateVerticesAndNormals(
    _In_reads_(num_vertices) const POINT3 vertices[],
    _In_reads_opt_(num_vertices) const VECTOR3 normals[],
    _In_ size_t num_vertices,
    _In_reads_(num_triangles) const size_t vertex_indices[][3],
    _In_ size_t num_triangles
    )
{
    if (vertices == NULL && num_vertices != 0)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (vertex_indices == NULL && num_triangles != 0)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    for (size_t i = 0; i < num_triangles; i++)
    {
        if (num_vertices <= vertex_indices[i][0])
        {
            return ISTATUS_INVALID_ARGUMENT_03;
        }

        if (!PointValidate(vertices[vertex_indices[i][0]]))
        {
            return ISTATUS_INVALID_ARGUMENT_00;
        }

        if (normals != NULL && !VectorValidate(normals[vertex_indices[i][0]]))
        {
            return ISTATUS_INVALID_ARGUMENT_02;
        }

        if (num_vertices <= vertex_indices[i][1])
        {
            return ISTATUS_INVALID_ARGUMENT_03;
        }
        
        if(!PointValidate(vertices[vertex_indices[i][1]]))
        {
            return ISTATUS_INVALID_ARGUMENT_00;
        }

        if (normals != NULL && !VectorValidate(normals[vertex_indices[i][1]]))
        {
            return ISTATUS_INVALID_ARGUMENT_02;
        }

        if (num_vertices <= vertex_indices[i][2])
        {
            return ISTATUS_INVALID_ARGUMENT_03;
        }

        if (!PointValidate(vertices[vertex_indices[i][2]]))
        {
            return ISTATUS_INVALID_ARGUMENT_00;
        }

        if (normals != NULL && !VectorValidate(normals[vertex_indices[i][2]]))
        {
            return ISTATUS_INVALID_ARGUMENT_02;
        }

        if (vertex_indices[i][0] == vertex_indices[i][1] ||
            vertex_indices[i][0] == vertex_indices[i][2] ||
            vertex_indices[i][1] == vertex_indices[i][2])
        {
            // TODO: Consider returning ISTATUS_INVALID_ARGUMENT_COMBINATION_00
            continue;
        }
    }

    return ISTATUS_SUCCESS;
}

static
ISTATUS
TriangleMeshAllocateInternal(
    _In_reads_(num_vertices) const POINT3 vertices[],
    _In_reads_opt_(num_vertices) const VECTOR3 normals[],
    _In_ size_t num_vertices,
    _In_opt_ PMATERIAL front_material,
    _In_opt_ PMATERIAL back_material,
    _In_opt_ PEMISSIVE_MATERIAL front_emissive_material,
    _In_opt_ PEMISSIVE_MATERIAL back_emissive_material,
    _Out_ PTRIANGLE_MESH* mesh
    )
{
    assert(vertices != NULL);
    assert(mesh != NULL);

    *mesh = (PTRIANGLE_MESH)malloc(sizeof(TRIANGLE_MESH));

    if (mesh == NULL)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    (*mesh)->vertices = (PPOINT3)calloc(num_vertices, sizeof(POINT3));

    if ((*mesh)->vertices == NULL)
    {
        free(*mesh);
        return ISTATUS_ALLOCATION_FAILED;
    }

    if (normals != NULL)
    {
        (*mesh)->normals = (PVECTOR3)calloc(num_vertices, sizeof(VECTOR3));

        if ((*mesh)->normals == NULL)
        {
            free((*mesh)->vertices);
            free(*mesh);
            return ISTATUS_ALLOCATION_FAILED;
        }

        memcpy((*mesh)->normals, normals, sizeof(VECTOR3) * num_vertices);
    }
    else
    {
        (*mesh)->normals = NULL;
    }

    memcpy((*mesh)->vertices, vertices, sizeof(POINT3) * num_vertices);

    (*mesh)->materials[0] = front_material;
    (*mesh)->materials[1] = back_material;
    (*mesh)->emissive_materials[0] = front_emissive_material;
    (*mesh)->emissive_materials[1] = back_emissive_material;
    (*mesh)->reference_count = 0;

    return ISTATUS_SUCCESS;
}

static
ISTATUS
TriangleAllocateInternal(
    _In_ PTRIANGLE_MESH mesh,
    _In_ uint32_t v0,
    _In_ uint32_t v1,
    _In_ uint32_t v2,
    _In_ uint32_t mesh_face_index,
    _Outptr_result_maybenull_ PSHAPE *shape
    )
{
    assert(mesh != NULL);
    assert(shape != NULL);

    TRIANGLE triangle;
    bool success = TriangleInitialize(mesh,
                                      v0,
                                      v1,
                                      v2,
                                      mesh_face_index,
                                      NULL,
                                      &triangle);

    if (!success)
    {
        *shape = NULL;
        return ISTATUS_SUCCESS;
    }

    PCSHAPE_VTABLE vtable;
    if (mesh->normals != NULL)
    {
        vtable = &triangle_with_normals_vtable;
    }
    else
    {
        vtable = &triangle_vtable;
    }

    ISTATUS status = ShapeAllocate(vtable,
                                   &triangle,
                                   sizeof(TRIANGLE),
                                   alignof(TRIANGLE),
                                   shape);

    return status;
}

static
ISTATUS
EmissiveTriangleAllocateInternal(
    _In_ PTRIANGLE_MESH mesh,
    _In_ uint32_t v0,
    _In_ uint32_t v1,
    _In_ uint32_t v2,
    _In_ uint32_t mesh_face_index,
    _Outptr_result_maybenull_ PSHAPE *shape
    )
{
    EMISSIVE_TRIANGLE emissive_triangle;
    bool success = TriangleInitialize(mesh,
                                      v0,
                                      v1,
                                      v2,
                                      mesh_face_index,
                                      &emissive_triangle.area,
                                      &emissive_triangle.triangle);

    if (!success)
    {
        *shape = NULL;
        return ISTATUS_SUCCESS;
    }

    PCSHAPE_VTABLE vtable;
    if (mesh->normals != NULL)
    {
        vtable = &emissive_triangle_with_normals_vtable;
    }
    else
    {
        vtable = &emissive_triangle_vtable;
    }

    ISTATUS status = ShapeAllocate(vtable,
                                   &emissive_triangle,
                                   sizeof(EMISSIVE_TRIANGLE),
                                   alignof(EMISSIVE_TRIANGLE),
                                   shape);

    return status;
}

static
void
ShapeListFree(
    _Inout_updates_to_(num_to_free, 0) PSHAPE shapes[],
    _In_ size_t num_to_free
    )
{
    for (size_t i = 0; i < num_to_free; i++)
    {
        ShapeRelease(shapes[i]);
    }
}

//
// Functions
//

ISTATUS
TriangleMeshAllocate(
    _In_reads_(num_vertices) const POINT3 vertices[],
    _In_reads_opt_(num_vertices) const VECTOR3 normals[],
    _In_ size_t num_vertices,
    _In_reads_(num_triangles) const size_t vertex_indices[][3],
    _In_ size_t num_triangles,
    _In_opt_ PMATERIAL front_material,
    _In_opt_ PMATERIAL back_material,
    _In_opt_ PEMISSIVE_MATERIAL front_emissive_material,
    _In_opt_ PEMISSIVE_MATERIAL back_emissive_material,
    _Out_writes_(num_triangles) PSHAPE shapes[],
    _Out_ size_t *triangles_allocated
    )
{
    ISTATUS status = TriangleMeshValidateVerticesAndNormals(vertices,
                                                            normals,
                                                            num_vertices,
                                                            vertex_indices,
                                                            num_triangles);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    if (num_triangles != 0 && shapes == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_09;
    }

    if (triangles_allocated == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_10;
    }

    PTRIANGLE_MESH mesh;
    status = TriangleMeshAllocateInternal(vertices,
                                          normals,
                                          num_vertices,
                                          front_material,
                                          back_material,
                                          front_emissive_material,
                                          back_emissive_material,
                                          &mesh);

    if (status != ISTATUS_SUCCESS)
    {
        assert(status == ISTATUS_ALLOCATION_FAILED);
        return status;
    }

    PSHAPE *shapes_tmp = (PSHAPE*)calloc(num_triangles, sizeof(PSHAPE));

    if (shapes_tmp == NULL)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    for (size_t i = 0; i < num_triangles; i++)
    {
        uint32_t v0 = vertex_indices[i][0];
        uint32_t v1 = vertex_indices[i][1];
        uint32_t v2 = vertex_indices[i][2];
        uint32_t face_index = (uint32_t)i;
        PSHAPE *shape = shapes_tmp + mesh->reference_count;

        if (front_emissive_material != NULL || back_emissive_material != NULL)
        {
            status = EmissiveTriangleAllocateInternal(mesh,
                                                      v0,
                                                      v1,
                                                      v2,
                                                      face_index,
                                                      shape);
        }
        else
        {
            status = TriangleAllocateInternal(mesh,
                                              v0,
                                              v1,
                                              v2,
                                              face_index,
                                              shape);
        }

        if (status != ISTATUS_SUCCESS)
        {
            assert(status == ISTATUS_ALLOCATION_FAILED);
            ShapeListFree(shapes_tmp, mesh->reference_count);
            free(shapes_tmp);
            TriangleMeshFree(mesh);
            return status;
        }

        if (*shape != NULL)
        {
            mesh->reference_count += 1;
        }
    }

    if (mesh->reference_count == 0)
    {
        TriangleMeshFree(mesh);
        *triangles_allocated = 0;
        return ISTATUS_SUCCESS;
    }

    MaterialRetain(front_material);
    MaterialRetain(back_material);
    EmissiveMaterialRetain(front_emissive_material);
    EmissiveMaterialRetain(back_emissive_material);

    *triangles_allocated = mesh->reference_count;

    memcpy(shapes, shapes_tmp, sizeof(PSHAPE) * num_triangles);
    free(shapes_tmp);

    return ISTATUS_SUCCESS;
}