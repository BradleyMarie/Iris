/*++

Copyright (c) 2020 Brad Weinberger

Module Name:

    triangle.c

Abstract:

    Implements a triangle.

--*/

#include <stdalign.h>

#include "iris_physx_toolkit/triangle.h"

//
// Constants
//

#define TRIANGLE_DEGENERATE_THRESHOLD (float_t)0.001

//
// Types
//

typedef struct _TRIANGLE {
    POINT3 v0;
    POINT3 v1;
    POINT3 v2;
    VECTOR3 surface_normal;
    PMATERIAL materials[2];
} TRIANGLE, *PTRIANGLE;

typedef const TRIANGLE *PCTRIANGLE;

typedef struct _EMISSIVE_TRIANGLE {
    TRIANGLE triangle;
    PEMISSIVE_MATERIAL emissive_materials[2];
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

    POINT3 v0 = PointSubtractPoint(triangle->v0, ray->origin);
    POINT3 v1 = PointSubtractPoint(triangle->v1, ray->origin);
    POINT3 v2 = PointSubtractPoint(triangle->v2, ray->origin);

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

    TRIANGLE_ADDITIONAL_DATA data;
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

    float_t dp = VectorDotProduct(ray->direction, triangle->surface_normal);

    uint32_t front_face, back_face;
    if (dp < (float_t)0.0)
    {
        front_face = TRIANGLE_FRONT_FACE;
        back_face = TRIANGLE_BACK_FACE;
    }
    else
    {
        front_face = TRIANGLE_BACK_FACE;
        back_face = TRIANGLE_FRONT_FACE;
    }

    ISTATUS status =
        ShapeHitAllocatorAllocate(allocator,
                                  NULL,
                                  distance,
                                  front_face,
                                  back_face,
                                  &data,
                                  sizeof(TRIANGLE_ADDITIONAL_DATA),
                                  alignof(TRIANGLE_ADDITIONAL_DATA),
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

    POINT3 world_v0 = PointMatrixMultiply(model_to_world, triangle->v0);
    POINT3 world_v1 = PointMatrixMultiply(model_to_world, triangle->v1);
    POINT3 world_v2 = PointMatrixMultiply(model_to_world, triangle->v2);

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
    if (face_hit > TRIANGLE_BACK_FACE)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    PCTRIANGLE triangle = (PCTRIANGLE)context;

    if (face_hit == TRIANGLE_FRONT_FACE)
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
    if (face_hit > TRIANGLE_BACK_FACE)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    PCTRIANGLE triangle = (PCTRIANGLE)context;
    *material = triangle->materials[face_hit];

    return ISTATUS_SUCCESS;
}

static
void
TriangleFree(
    _In_opt_ _Post_invalid_ void *context
    )
{
    PTRIANGLE triangle = (PTRIANGLE)context;

    MaterialRelease(triangle->materials[0]);
    MaterialRelease(triangle->materials[1]);
}

static
ISTATUS
TriangleInitialize(
    _In_ POINT3 v0,
    _In_ POINT3 v1,
    _In_ POINT3 v2,
    _In_opt_ PMATERIAL front_material,
    _In_opt_ PMATERIAL back_material,
    _Out_opt_ float_t *area,
    _Out_ PTRIANGLE triangle
    )
{
    assert(PointValidate(v0));
    assert(PointValidate(v1));
    assert(PointValidate(v2));
    assert(triangle != NULL);

    triangle->v0 = v0;
    triangle->v1 = v1;
    triangle->v2 = v2;
    triangle->materials[0] = front_material;
    triangle->materials[1] = back_material;

    VECTOR3 v0_to_v1 = PointSubtract(v1, v0);
    VECTOR3 v0_to_v2 = PointSubtract(v2, v0);

    triangle->surface_normal = VectorCrossProduct(v0_to_v1, v0_to_v2);
    float_t surface_normal_length = VectorLength(triangle->surface_normal);
    if (surface_normal_length <= TRIANGLE_DEGENERATE_THRESHOLD)
    {
        return ISTATUS_INVALID_ARGUMENT_COMBINATION_00;
    }

    float_t scalar = (float_t)1.0 / surface_normal_length;
    triangle->surface_normal = VectorScale(triangle->surface_normal, scalar);

    if (area != NULL)
    {
        *area = surface_normal_length * (float_t)0.5;
    }

    return ISTATUS_SUCCESS;
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
    if (face_hit > TRIANGLE_BACK_FACE)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    PCEMISSIVE_TRIANGLE triangle = (PCEMISSIVE_TRIANGLE)context;
    *emissive_material = triangle->emissive_materials[face_hit];

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

    VECTOR3 v0_to_v1 =
        PointSubtract(triangle->triangle.v1, triangle->triangle.v0);
    VECTOR3 v0_to_v2 =
        PointSubtract(triangle->triangle.v2, triangle->triangle.v0);

    POINT3 sum = PointVectorAddScaled(triangle->triangle.v0, v0_to_v1, u);
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

    if (face_hit == TRIANGLE_FRONT_FACE)
    {
        dp = -dp;
    }

    *pdf = (distance * distance) / (dp * triangle->area);

    return ISTATUS_SUCCESS;
}

static
void
EmissiveTriangleFree(
    _In_opt_ _Post_invalid_ void *context
    )
{
    PEMISSIVE_TRIANGLE triangle = (PEMISSIVE_TRIANGLE)context;

    MaterialRelease(triangle->triangle.materials[0]);
    MaterialRelease(triangle->triangle.materials[1]);
    EmissiveMaterialRelease(triangle->emissive_materials[0]);
    EmissiveMaterialRelease(triangle->emissive_materials[1]);
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
    EmissiveTriangleFree
};

//
// Static Functions
//

static
ISTATUS
TriangleAllocateInternal(
    _In_ POINT3 v0,
    _In_ POINT3 v1,
    _In_ POINT3 v2,
    _In_opt_ PMATERIAL front_material,
    _In_opt_ PMATERIAL back_material,
    _Out_ PSHAPE *shape
    )
{
    assert(PointValidate(v0));
    assert(PointValidate(v1));
    assert(PointValidate(v2));
    assert(shape != NULL);

    TRIANGLE triangle;
    ISTATUS status = TriangleInitialize(v0,
                                        v1,
                                        v2,
                                        front_material,
                                        back_material,
                                        NULL,
                                        &triangle);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    status = ShapeAllocate(&triangle_vtable,
                           &triangle,
                           sizeof(TRIANGLE),
                           alignof(TRIANGLE),
                           shape);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    MaterialRetain(front_material);
    MaterialRetain(back_material);

    return ISTATUS_SUCCESS;
}

//
// Functions
//

ISTATUS
TriangleAllocate(
    _In_ POINT3 v0,
    _In_ POINT3 v1,
    _In_ POINT3 v2,
    _In_opt_ PMATERIAL front_material,
    _In_opt_ PMATERIAL back_material,
    _Out_ PSHAPE *shape
    )
{
    if (!PointValidate(v0))
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (!PointValidate(v1))
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (!PointValidate(v2))
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (shape == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_05;
    }

    ISTATUS status = TriangleAllocateInternal(v0,
                                              v1,
                                              v2,
                                              front_material,
                                              back_material,
                                              shape);

    return status;
}

ISTATUS
EmissiveTriangleAllocate(
    _In_ POINT3 v0,
    _In_ POINT3 v1,
    _In_ POINT3 v2,
    _In_opt_ PMATERIAL front_material,
    _In_opt_ PMATERIAL back_material,
    _In_opt_ PEMISSIVE_MATERIAL front_emissive_material,
    _In_opt_ PEMISSIVE_MATERIAL back_emissive_material,
    _Out_ PSHAPE *shape
    )
{
    if (!PointValidate(v0))
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (!PointValidate(v1))
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (!PointValidate(v2))
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (shape == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_07;
    }

    if (front_emissive_material == NULL &&
        back_emissive_material == NULL)
    {
        ISTATUS status = TriangleAllocateInternal(v0,
                                                  v1,
                                                  v2,
                                                  front_material,
                                                  back_material,
                                                  shape);

        return status;
    }

    EMISSIVE_TRIANGLE triangle;
    ISTATUS status = TriangleInitialize(v0,
                                        v1,
                                        v2,
                                        front_material,
                                        back_material,
                                        &triangle.area,
                                        &triangle.triangle);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    triangle.emissive_materials[0] = front_emissive_material;
    triangle.emissive_materials[1] = back_emissive_material;

    status = ShapeAllocate(&emissive_triangle_vtable,
                           &triangle,
                           sizeof(EMISSIVE_TRIANGLE),
                           alignof(EMISSIVE_TRIANGLE),
                           shape);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    MaterialRetain(front_material);
    MaterialRetain(back_material);
    EmissiveMaterialRetain(front_emissive_material);
    EmissiveMaterialRetain(back_emissive_material);

    return ISTATUS_SUCCESS;
}