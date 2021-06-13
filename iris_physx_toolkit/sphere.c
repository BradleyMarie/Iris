/*++

Copyright (c) 2021 Brad Weinberger

Module Name:

    sphere.c

Abstract:

    Implements a sphere.

--*/

#include <stdalign.h>

#include "iris_advanced_toolkit/sample_geometry.h"
#include "iris_physx_toolkit/sphere.h"

//
// Types
//

typedef struct _SPHERE {
    POINT3 center;
    float_t radius_squared;
    PMATERIAL materials[2];
} SPHERE, *PSPHERE;

typedef const SPHERE *PCSPHERE;

typedef struct _EMISSIVE_SPHERE {
    SPHERE sphere;
    PEMISSIVE_MATERIAL emissive_materials[2];
    float_t radius;
} EMISSIVE_SPHERE, *PEMISSIVE_SPHERE;

typedef const EMISSIVE_SPHERE *PCEMISSIVE_SPHERE;

//
// Static Sphere Functions
//

static
ISTATUS
SphereTrace(
    _In_ const void *context,
    _In_ PCRAY ray,
    _In_ float_t minimum_distance,
    _In_ float_t maximum_distance,
    _In_ PSHAPE_HIT_ALLOCATOR allocator,
    _Out_ PHIT *hit
    )
{
    PCSPHERE sphere = (PCSPHERE)context;

    VECTOR3 to_center = PointSubtract(sphere->center, ray->origin);
    float_t distance_to_chord_midpoint = VectorDotProduct(to_center,
                                                          ray->direction);
        
    float_t distance_to_center_squared = VectorDotProduct(to_center, to_center);

    float_t distance_to_chord_midpoint_squared =
        distance_to_chord_midpoint * distance_to_chord_midpoint;
    float_t distance_from_chord_to_center_squared = 
        distance_to_center_squared - distance_to_chord_midpoint_squared;
    
    // The ray completely misses the sphere. No intersections are possible.
    if (sphere->radius_squared < distance_from_chord_to_center_squared)
    {
        return ISTATUS_NO_INTERSECTION;
    }

    float_t half_chord_length = 
        sqrt(sphere->radius_squared - distance_from_chord_to_center_squared);

    // Ray intersects sphere and originates inside the sphere
    if (distance_to_center_squared < sphere->radius_squared)
    {
        float_t forward_hit = distance_to_chord_midpoint + half_chord_length;
        ISTATUS status = ShapeHitAllocatorAllocate(allocator,
                                                   NULL,
                                                   forward_hit,
                                                   SPHERE_BACK_FACE,
                                                   SPHERE_FRONT_FACE,
                                                   NULL,
                                                   0,
                                                   0,
                                                   hit);

        if (status != ISTATUS_SUCCESS)
        {
            return status;
        }

        float_t backwards_hit = distance_to_chord_midpoint - half_chord_length;
        status = ShapeHitAllocatorAllocate(allocator,
                                           *hit,
                                           backwards_hit,
                                           SPHERE_BACK_FACE,
                                           SPHERE_FRONT_FACE,
                                           NULL,
                                           0,
                                           0,
                                           hit);

        return status;
    }

    // Ray intersects sphere and originates outside the sphere.
    float_t farther_hit = distance_to_chord_midpoint + half_chord_length;
    ISTATUS status = ShapeHitAllocatorAllocate(allocator,
                                                NULL,
                                                farther_hit,
                                                SPHERE_BACK_FACE,
                                                SPHERE_FRONT_FACE,
                                                NULL,
                                                0,
                                                0,
                                                hit);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    float_t closer_hit = distance_to_chord_midpoint - half_chord_length;
    status = ShapeHitAllocatorAllocate(allocator,
                                       *hit,
                                        closer_hit,
                                        SPHERE_FRONT_FACE,
                                        SPHERE_BACK_FACE,
                                        NULL,
                                        0,
                                        0,
                                        hit);

    return status;
}

static
ISTATUS
SphereComputeBounds(
    _In_ const void *context,
    _In_opt_ PCMATRIX model_to_world,
    _Out_ PBOUNDING_BOX world_bounds
    )
{
    PCSPHERE sphere = (PCSPHERE)context;

    float_t radius = sqrt(sphere->radius_squared);
    VECTOR3 to_bound = VectorCreate(radius, radius, radius);

    POINT3 lower_bound = PointVectorSubtract(sphere->center, to_bound);
    POINT3 upper_bound = PointVectorAdd(sphere->center, to_bound);

    BOUNDING_BOX object_bounds = BoundingBoxCreate(lower_bound, upper_bound);
    *world_bounds = BoundingBoxTransform(model_to_world, object_bounds);

    return ISTATUS_SUCCESS;
}

static
ISTATUS
SphereComputeNormal(
    _In_ const void *context,
    _In_ POINT3 hit_point,
    _In_ uint32_t face_hit,
    _Out_ PVECTOR3 surface_normal
    )
{
    if (face_hit > SPHERE_BACK_FACE)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    PCSPHERE sphere = (PCSPHERE)context;

    VECTOR3 normal;
    if (face_hit == SPHERE_FRONT_FACE)
    {
        normal = PointSubtract(hit_point, sphere->center);
    }
    else
    {
        normal = PointSubtract(sphere->center, hit_point);
    }

    *surface_normal = VectorNormalize(normal, NULL, NULL);

    return ISTATUS_SUCCESS;
}

ISTATUS
SphereGetTextureCoordinateMap(
    _In_opt_ const void *context,
    _In_ uint32_t face_hit,
    _Outptr_ PCTEXTURE_COORDINATE_MAP *texture_coordinate_map
    )
{
    if (face_hit > SPHERE_BACK_FACE)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    *texture_coordinate_map = NULL;

    return ISTATUS_SUCCESS;
}

static
ISTATUS
SphereGetNormalMap(
    _In_opt_ const void *context,
    _In_ uint32_t face_hit,
    _Outptr_ PCNORMAL_MAP *normal_map
    )
{
    if (face_hit > SPHERE_BACK_FACE)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    *normal_map = NULL;

    return ISTATUS_SUCCESS;
}

static
ISTATUS
SphereGetMaterial(
    _In_opt_ const void *context, 
    _In_ uint32_t face_hit,
    _Outptr_result_maybenull_ PCMATERIAL *material
    )
{
    if (face_hit > SPHERE_BACK_FACE)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    PCSPHERE sphere = (PCSPHERE)context;
    *material = sphere->materials[face_hit];

    return ISTATUS_SUCCESS;
}

static
void
SphereFree(
    _In_opt_ _Post_invalid_ void *context
    )
{
    PSPHERE sphere = (PSPHERE)context;

    MaterialRelease(sphere->materials[0]);
    MaterialRelease(sphere->materials[1]);
}

//
// Static Emissive Sphere Functions
//

static
ISTATUS
EmissiveSphereGetEmissiveMaterial(
    _In_opt_ const void *context,
    _In_ uint32_t face_hit,
    _Outptr_result_maybenull_ PCEMISSIVE_MATERIAL *emissive_material
    )
{
    if (face_hit > SPHERE_BACK_FACE)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    PCEMISSIVE_SPHERE sphere = (PCEMISSIVE_SPHERE)context;
    *emissive_material = sphere->emissive_materials[face_hit];

    return ISTATUS_SUCCESS;
}

static
ISTATUS
EmissiveSphereSampleFace(
    _In_opt_ const void *context,
    _In_ uint32_t face_hit,
    _Inout_ PRANDOM rng,
    _Out_ PPOINT3 sampled_point
    )
{
    PEMISSIVE_SPHERE sphere = (PEMISSIVE_SPHERE)context;

    VECTOR3 offset;
    ISTATUS status = SampleSphereUniformly(sphere->radius, rng, &offset);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    *sampled_point = PointVectorAdd(sphere->sphere.center, offset);

    return ISTATUS_SUCCESS;
}

ISTATUS
EmissiveSphereComputePdfBySolidArea(
    _In_opt_ const void *context,
    _In_ PCRAY to_shape,
    _In_ float_t distance,
    _In_ uint32_t face_hit,
    _Out_ float_t *pdf
    )
{
    PEMISSIVE_SPHERE sphere = (PEMISSIVE_SPHERE)context;

    if (face_hit == SPHERE_BACK_FACE)
    {
        POINT3 hit_point = RayEndpoint(*to_shape, distance);
        VECTOR3 reversed_surface_normal =
            PointSubtract(hit_point, sphere->sphere.center);
        reversed_surface_normal =
            VectorNormalize(reversed_surface_normal, NULL, NULL);
        VECTOR3 incoming = VectorNormalize(to_shape->direction, NULL, NULL);
        float_t dot_product =
            VectorBoundedDotProduct(reversed_surface_normal, incoming);
        float_t area = (float_t)4.0 * iris_pi * sphere->sphere.radius_squared;
        *pdf = (distance * distance) / (dot_product * area);
        return ISTATUS_SUCCESS;
    }

    VECTOR3 to_center = PointSubtract(to_shape->origin, sphere->sphere.center);
    float_t distance_to_center_squared = VectorDotProduct(to_center, to_center);

    float_t sin_theta_squared =
        sphere->sphere.radius_squared / distance_to_center_squared;
    float_t cos_theta_squared =
        IMax((float_t)0.0, (float_t)1.0 - sin_theta_squared);
    float_t cos_theta = sqrt(cos_theta_squared);

    *pdf = (float_t)1.0 / (iris_two_pi * ((float_t)1.0 - cos_theta));

    return ISTATUS_SUCCESS;
}

static
void
EmissiveSphereFree(
    _In_opt_ _Post_invalid_ void *context
    )
{
    PEMISSIVE_SPHERE sphere = (PEMISSIVE_SPHERE)context;

    MaterialRelease(sphere->sphere.materials[0]);
    MaterialRelease(sphere->sphere.materials[1]);
    EmissiveMaterialRelease(sphere->emissive_materials[0]);
    EmissiveMaterialRelease(sphere->emissive_materials[1]);
}

//
// Static Variables
//

static const SHAPE_VTABLE sphere_vtable = {
    SphereTrace,
    SphereComputeBounds,
    SphereComputeNormal,
    SphereGetTextureCoordinateMap,
    SphereGetNormalMap,
    SphereGetMaterial,
    NULL,
    NULL,
    NULL,
    SphereFree
};

static const SHAPE_VTABLE emissive_sphere_vtable = {
    SphereTrace,
    SphereComputeBounds,
    SphereComputeNormal,
    SphereGetTextureCoordinateMap,
    SphereGetNormalMap,
    SphereGetMaterial,
    EmissiveSphereGetEmissiveMaterial,
    EmissiveSphereSampleFace,
    EmissiveSphereComputePdfBySolidArea,
    EmissiveSphereFree
};

//
// Functions
//

ISTATUS
SphereAllocate(
    _In_ POINT3 center,
    _In_ float_t radius,
    _In_opt_ PMATERIAL front_material,
    _In_opt_ PMATERIAL back_material,
    _Out_ PSHAPE *shape
    )
{
    if (!PointValidate(center))
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (!isfinite(radius) || radius <= (float_t)0.0)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (shape == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    SPHERE sphere;
    sphere.center = center;
    sphere.radius_squared = radius * radius;
    sphere.materials[SPHERE_FRONT_FACE] = front_material;
    sphere.materials[SPHERE_BACK_FACE] = back_material;

    ISTATUS status = ShapeAllocate(&sphere_vtable,
                                   &sphere,
                                   sizeof(SPHERE),
                                   alignof(SPHERE),
                                   shape);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    MaterialRetain(front_material);
    MaterialRetain(back_material);

    return ISTATUS_SUCCESS;
}

ISTATUS
EmissiveSphereAllocate(
    _In_ POINT3 center,
    _In_ float_t radius,
    _In_opt_ PMATERIAL front_material,
    _In_opt_ PMATERIAL back_material,
    _In_opt_ PEMISSIVE_MATERIAL front_emissive_material,
    _In_opt_ PEMISSIVE_MATERIAL back_emissive_material,
    _Out_ PSHAPE *shape
    )
{
    if (!PointValidate(center))
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (!isfinite(radius) || radius <= (float_t)0.0)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (shape == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_06;
    }

    if (front_emissive_material == NULL &&
        back_emissive_material == NULL)
    {
        ISTATUS status = SphereAllocate(center,
                                        radius,
                                        front_material,
                                        back_material,
                                        shape);

        return status;
    }

    EMISSIVE_SPHERE sphere;
    sphere.sphere.center = center;
    sphere.sphere.radius_squared = radius * radius;
    sphere.sphere.materials[SPHERE_FRONT_FACE] = front_material;
    sphere.sphere.materials[SPHERE_BACK_FACE] = back_material;
    sphere.emissive_materials[SPHERE_FRONT_FACE] = front_emissive_material;
    sphere.emissive_materials[SPHERE_BACK_FACE] = back_emissive_material;
    sphere.radius = radius;

    ISTATUS status = ShapeAllocate(&emissive_sphere_vtable,
                                   &sphere,
                                   sizeof(EMISSIVE_SPHERE),
                                   alignof(EMISSIVE_SPHERE),
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