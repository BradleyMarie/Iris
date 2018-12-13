/*++

Copyright (c) 2018 Brad Weinberger

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
    VECTOR3 v0_to_v1;
    VECTOR3 v0_to_v2;
    VECTOR3 surface_normal;
    PMATERIAL materials[2];
} TRIANGLE, *PTRIANGLE;

typedef const TRIANGLE *PCTRIANGLE;

//
// Static Functions
//

static
ISTATUS
TriangleTraceXYDominant(
    _In_ const void *context,
    _In_ PCRAY ray,
    _In_ PSHAPE_HIT_ALLOCATOR allocator,
    _Out_ PHIT *hit
    )
{
    PCTRIANGLE triangle = (PCTRIANGLE)context;

    VECTOR3 v0_to_ray_origin = PointSubtract(ray->origin, triangle->v0);
    float_t dp = VectorDotProduct(ray->direction, triangle->surface_normal);

    if (dp == (float_t)0.0)
    {
        return ISTATUS_SUCCESS;
    }

    float_t distance_to_plane =
        VectorDotProduct(v0_to_ray_origin, triangle->surface_normal) / -dp;

    if (isinf(distance_to_plane))
    {
        return ISTATUS_SUCCESS;
    }

#if !defined(CONSTRUCTIVE_SOLID_GEOMETRY) 
    if (distance_to_plane < (float_t)0.0)
    {
        return ISTATUS_SUCCESS;
    }
#endif // defined(CONSTRUCTIVE_SOLID_GEOMETRY) 

    POINT3 plane_intersection = RayEndpoint(*ray, distance_to_plane);
    VECTOR3 vertex_to_plane_intersection =
        PointSubtract(plane_intersection, triangle->v0);

    TRIANGLE_ADDITIONAL_DATA additional_data;
    additional_data.barycentric_coordinates[1] =
        (vertex_to_plane_intersection.y * triangle->v0_to_v2.x - 
         vertex_to_plane_intersection.x * triangle->v0_to_v2.y) /
        (triangle->v0_to_v1.y * triangle->v0_to_v2.x - 
         triangle->v0_to_v1.x * triangle->v0_to_v2.y);

    if (additional_data.barycentric_coordinates[1] > (float_t)1.0 ||
        additional_data.barycentric_coordinates[1] < (float_t)0.0)
    {
        return ISTATUS_SUCCESS;
    }

    additional_data.barycentric_coordinates[2] =
        (vertex_to_plane_intersection.y * triangle->v0_to_v1.x - 
         vertex_to_plane_intersection.x * triangle->v0_to_v1.y) /
        (triangle->v0_to_v2.y * triangle->v0_to_v1.x - 
         triangle->v0_to_v2.x * triangle->v0_to_v1.y);

    if (additional_data.barycentric_coordinates[2] < (float_t)0.0)
    {
        return ISTATUS_SUCCESS;
    }

    float_t coordinate_sum = 
        additional_data.barycentric_coordinates[1] +
        additional_data.barycentric_coordinates[2];

    if (coordinate_sum > (float_t)1.0)
    {
        return ISTATUS_SUCCESS;
    }

    additional_data.barycentric_coordinates[0] = (float_t)1.0 - coordinate_sum;

    ISTATUS status;
    if (dp < (float_t)0.0)
    {
        status =
            ShapeHitAllocatorAllocateWithHitPoint(allocator,
                                                  NULL,
                                                  distance_to_plane,
                                                  TRIANGLE_FRONT_FACE,
                                                  TRIANGLE_BACK_FACE,
                                                  &additional_data,
                                                  sizeof(TRIANGLE_ADDITIONAL_DATA),
                                                  alignof(TRIANGLE_ADDITIONAL_DATA),
                                                  plane_intersection,
                                                  hit);
    }
    else
    {
        status =
            ShapeHitAllocatorAllocateWithHitPoint(allocator,
                                                  NULL,
                                                  distance_to_plane,
                                                  TRIANGLE_BACK_FACE,
                                                  TRIANGLE_FRONT_FACE,
                                                  &additional_data,
                                                  sizeof(TRIANGLE_ADDITIONAL_DATA),
                                                  alignof(TRIANGLE_ADDITIONAL_DATA),
                                                  plane_intersection,
                                                  hit);
    }

    return status;
}

static
ISTATUS
TriangleTraceXZDominant(
    _In_ const void *context,
    _In_ PCRAY ray,
    _In_ PSHAPE_HIT_ALLOCATOR allocator,
    _Out_ PHIT *hit
    )
{
    PCTRIANGLE triangle = (PCTRIANGLE)context;

    VECTOR3 v0_to_ray_origin = PointSubtract(ray->origin, triangle->v0);
    float_t dp = VectorDotProduct(ray->direction, triangle->surface_normal);

    if (dp == (float_t)0.0)
    {
        return ISTATUS_SUCCESS;
    }

    float_t distance_to_plane =
        VectorDotProduct(v0_to_ray_origin, triangle->surface_normal) / -dp;

    if (isinf(distance_to_plane))
    {
        return ISTATUS_SUCCESS;
    }

#if !defined(CONSTRUCTIVE_SOLID_GEOMETRY) 
    if (distance_to_plane < (float_t)0.0)
    {
        return ISTATUS_SUCCESS;
    }
#endif // defined(CONSTRUCTIVE_SOLID_GEOMETRY) 

    POINT3 plane_intersection = RayEndpoint(*ray, distance_to_plane);
    VECTOR3 vertex_to_plane_intersection =
        PointSubtract(plane_intersection, triangle->v0);

    TRIANGLE_ADDITIONAL_DATA additional_data;
    additional_data.barycentric_coordinates[1] =
        (vertex_to_plane_intersection.x * triangle->v0_to_v2.z - 
         vertex_to_plane_intersection.z * triangle->v0_to_v2.x) /
        (triangle->v0_to_v1.x * triangle->v0_to_v2.z - 
         triangle->v0_to_v1.z * triangle->v0_to_v2.x);

    if (additional_data.barycentric_coordinates[1] > (float_t)1.0 ||
        additional_data.barycentric_coordinates[1] < (float_t)0.0)
    {
        return ISTATUS_SUCCESS;
    }

    additional_data.barycentric_coordinates[2] =
        (vertex_to_plane_intersection.x * triangle->v0_to_v1.z - 
         vertex_to_plane_intersection.z * triangle->v0_to_v1.x) /
        (triangle->v0_to_v2.x * triangle->v0_to_v1.z - 
         triangle->v0_to_v2.z * triangle->v0_to_v1.x);

    if (additional_data.barycentric_coordinates[2] < (float_t)0.0)
    {
        return ISTATUS_SUCCESS;
    }

    float_t coordinate_sum = 
        additional_data.barycentric_coordinates[1] +
        additional_data.barycentric_coordinates[2];

    if (coordinate_sum > (float_t)1.0)
    {
        return ISTATUS_SUCCESS;
    }

    additional_data.barycentric_coordinates[0] = (float_t)1.0 - coordinate_sum;

    ISTATUS status;
    if (dp < (float_t)0.0)
    {
        status =
            ShapeHitAllocatorAllocateWithHitPoint(allocator,
                                                  NULL,
                                                  distance_to_plane,
                                                  TRIANGLE_FRONT_FACE,
                                                  TRIANGLE_BACK_FACE,
                                                  &additional_data,
                                                  sizeof(TRIANGLE_ADDITIONAL_DATA),
                                                  alignof(TRIANGLE_ADDITIONAL_DATA),
                                                  plane_intersection,
                                                  hit);
    }
    else
    {
        status =
            ShapeHitAllocatorAllocateWithHitPoint(allocator,
                                                  NULL,
                                                  distance_to_plane,
                                                  TRIANGLE_BACK_FACE,
                                                  TRIANGLE_FRONT_FACE,
                                                  &additional_data,
                                                  sizeof(TRIANGLE_ADDITIONAL_DATA),
                                                  alignof(TRIANGLE_ADDITIONAL_DATA),
                                                  plane_intersection,
                                                  hit);
    }

    return status;
}

static
ISTATUS
TriangleTraceYZDominant(
    _In_ const void *context,
    _In_ PCRAY ray,
    _In_ PSHAPE_HIT_ALLOCATOR allocator,
    _Out_ PHIT *hit
    )
{
    PCTRIANGLE triangle = (PCTRIANGLE)context;

    VECTOR3 v0_to_ray_origin = PointSubtract(ray->origin, triangle->v0);
    float_t dp = VectorDotProduct(ray->direction, triangle->surface_normal);

    if (dp == (float_t)0.0)
    {
        return ISTATUS_SUCCESS;
    }

    float_t distance_to_plane =
        VectorDotProduct(v0_to_ray_origin, triangle->surface_normal) / -dp;

    if (isinf(distance_to_plane))
    {
        return ISTATUS_SUCCESS;
    }

#if !defined(CONSTRUCTIVE_SOLID_GEOMETRY) 
    if (distance_to_plane < (float_t)0.0)
    {
        return ISTATUS_SUCCESS;
    }
#endif // defined(CONSTRUCTIVE_SOLID_GEOMETRY) 

    POINT3 plane_intersection = RayEndpoint(*ray, distance_to_plane);
    VECTOR3 vertex_to_plane_intersection =
        PointSubtract(plane_intersection, triangle->v0);

    TRIANGLE_ADDITIONAL_DATA additional_data;
    additional_data.barycentric_coordinates[1] =
        (vertex_to_plane_intersection.z * triangle->v0_to_v2.y - 
         vertex_to_plane_intersection.y * triangle->v0_to_v2.z) /
        (triangle->v0_to_v1.z * triangle->v0_to_v2.y - 
         triangle->v0_to_v1.y * triangle->v0_to_v2.z);

    if (additional_data.barycentric_coordinates[1] > (float_t)1.0 ||
        additional_data.barycentric_coordinates[1] < (float_t)0.0)
    {
        return ISTATUS_SUCCESS;
    }

    additional_data.barycentric_coordinates[2] =
        (vertex_to_plane_intersection.z * triangle->v0_to_v1.y - 
         vertex_to_plane_intersection.y * triangle->v0_to_v1.z) /
        (triangle->v0_to_v2.z * triangle->v0_to_v1.y - 
         triangle->v0_to_v2.y * triangle->v0_to_v1.z);

    if (additional_data.barycentric_coordinates[2] < (float_t)0.0)
    {
        return ISTATUS_SUCCESS;
    }

    float_t coordinate_sum = 
        additional_data.barycentric_coordinates[1] +
        additional_data.barycentric_coordinates[2];

    if (coordinate_sum > (float_t)1.0)
    {
        return ISTATUS_SUCCESS;
    }

    additional_data.barycentric_coordinates[0] = (float_t)1.0 - coordinate_sum;

    ISTATUS status;
    if (dp < (float_t)0.0)
    {
        status =
            ShapeHitAllocatorAllocateWithHitPoint(allocator,
                                                  NULL,
                                                  distance_to_plane,
                                                  TRIANGLE_FRONT_FACE,
                                                  TRIANGLE_BACK_FACE,
                                                  &additional_data,
                                                  sizeof(TRIANGLE_ADDITIONAL_DATA),
                                                  alignof(TRIANGLE_ADDITIONAL_DATA),
                                                  plane_intersection,
                                                  hit);
    }
    else
    {
        status =
            ShapeHitAllocatorAllocateWithHitPoint(allocator,
                                                  NULL,
                                                  distance_to_plane,
                                                  TRIANGLE_BACK_FACE,
                                                  TRIANGLE_FRONT_FACE,
                                                  &additional_data,
                                                  sizeof(TRIANGLE_ADDITIONAL_DATA),
                                                  alignof(TRIANGLE_ADDITIONAL_DATA),
                                                  plane_intersection,
                                                  hit);
    }

    return status;
}

static
ISTATUS
TriangleCheckBounds(
    _In_ const void *context,
    _In_opt_ PCMATRIX model_to_world,
    _In_ BOUNDING_BOX world_bounds,
    _Out_ bool *contains
    )
{
    PCTRIANGLE triangle = (PCTRIANGLE)context;

    POINT3 v1 = PointVectorAdd(triangle->v0, triangle->v0_to_v1);
    POINT3 v2 = PointVectorAdd(triangle->v0, triangle->v0_to_v2);

    POINT3 world_v0 = PointMatrixMultiply(model_to_world, triangle->v0);
    POINT3 world_v1 = PointMatrixMultiply(model_to_world, v1);
    POINT3 world_v2 = PointMatrixMultiply(model_to_world, v2);

    BOUNDING_BOX object_bounds = BoundingBoxCreate(world_v0, world_v1);
    object_bounds = BoundingBoxEnvelop(object_bounds, world_v2);

    *contains = BoundingBoxOverlaps(world_bounds, object_bounds);

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

//
// Static Variables
//

static const SHAPE_VTABLE xy_dominant_triangle_vtable = {
        TriangleTraceXYDominant,
        TriangleCheckBounds,
        TriangleComputeNormal,
        TriangleGetMaterial,
        NULL,
        NULL,
        NULL,
        TriangleFree
};

static const SHAPE_VTABLE xz_dominant_triangle_vtable = {
        TriangleTraceXZDominant,
        TriangleCheckBounds,
        TriangleComputeNormal,
        TriangleGetMaterial,
        NULL,
        NULL,
        NULL,
        TriangleFree
};

static const SHAPE_VTABLE yz_dominant_triangle_vtable = {
        TriangleTraceYZDominant,
        TriangleCheckBounds,
        TriangleComputeNormal,
        TriangleGetMaterial,
        NULL,
        NULL,
        NULL,
        TriangleFree
};

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
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (!PointValidate(v2))
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (shape == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_05;
    }

    TRIANGLE triangle;
    triangle.v0 = v0;
    triangle.v0_to_v1 = PointSubtract(v1, v0);
    triangle.v0_to_v2 = PointSubtract(v2, v0);
    triangle.materials[0] = front_material;
    triangle.materials[1] = back_material;
    
    float_t surface_normal_length;
    triangle.surface_normal = 
        VectorCrossProduct(triangle.v0_to_v1, triangle.v0_to_v2);
    triangle.surface_normal = 
        VectorNormalize(triangle.surface_normal, NULL, &surface_normal_length);

    if (surface_normal_length <= TRIANGLE_DEGENERATE_THRESHOLD)
    {
        return ISTATUS_INVALID_ARGUMENT_COMBINATION_00;
    }

    PCSHAPE_VTABLE triangle_vtable;
    VECTOR_AXIS dominant_axis = VectorDominantAxis(triangle.surface_normal);
    switch (dominant_axis)
    {
        case VECTOR_X_AXIS:
            triangle_vtable = &yz_dominant_triangle_vtable;
            break;
        case VECTOR_Y_AXIS:
            triangle_vtable = &xz_dominant_triangle_vtable;
            break;
        default: // VECTOR_Z_AXIS
            triangle_vtable = &xy_dominant_triangle_vtable;
            break;
    }

    ISTATUS status = ShapeAllocate(triangle_vtable,
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