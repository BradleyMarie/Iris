/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    sphere.c

Abstract:

    Implements a sphere.

--*/

#include <stdalign.h>

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

//
// Static Functions
//

static
ISTATUS
SphereTrace(
    _In_ const void *context,
    _In_ PCRAY ray,
    _In_ PSHAPE_HIT_ALLOCATOR allocator,
    _Out_ PHIT *hit
    )
{
    PCSPHERE sphere = (PCSPHERE)context;

    VECTOR3 to_center = PointSubtract(ray->origin, sphere->center);
    float_t distance_to_chord_midpoint = VectorDotProduct(to_center,
                                                          ray->direction);

#if defined(ONE_SIDED_GEOMETRY) && !defined(CONSTRUCTIVE_SOLID_GEOMETRY) 
    // Either the sphere is behind the ray or the ray starts inside the sphere.
    // No intersections are possible.
    if (distance_to_chord_midpoint < (float_t)0.0)
    {
        return ISTATUS_SUCCESS;
    }
#endif // defined(ONE_SIDED_GEOMETRY) && !defined(CONSTRUCTIVE_SOLID_GEOMETRY) 
        
    float_t distance_to_center_squared = VectorDotProduct(to_center, to_center);

#if defined(ONE_SIDED_GEOMETRY) && !defined(CONSTRUCTIVE_SOLID_GEOMETRY) 
    // Ray begins inside the sphere. No intersections are possible.
    if (distance_to_center_squared < sphere->radius_squared)
    {
        return ISTATUS_SUCCESS;
    }
#endif // defined(ONE_SIDED_GEOMETRY) && !defined(CONSTRUCTIVE_SOLID_GEOMETRY) 

    float_t distance_from_chord_to_center_squared = 
        distance_to_chord_midpoint * distance_to_chord_midpoint -
        distance_to_center_squared * distance_to_center_squared;
    
    // The ray completely misses the sphere. No intersections are possible.
    if (sphere->radius_squared < distance_from_chord_to_center_squared)
    {
        return ISTATUS_SUCCESS;
    }

    float_t half_chord_length = 
        sqrt(sphere->radius_squared - distance_from_chord_to_center_squared);

#if !defined(ONE_SIDED_GEOMETRY) || defined(CONSTRUCTIVE_SOLID_GEOMETRY) 
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

#ifdef CONSTRUCTIVE_SOLID_GEOMETRY
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
#endif // CONSTRUCTIVE_SOLID_GEOMETRY

        return status;
    }
#endif // !defined(ONE_SIDED_GEOMETRY) || defined(CONSTRUCTIVE_SOLID_GEOMETRY) 

    // Ray intersects sphere and originates outside the sphere.
#ifdef CONSTRUCTIVE_SOLID_GEOMETRY
    {
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
    }
#endif // CONSTRUCTIVE_SOLID_GEOMETRY

    float_t closer_hit = distance_to_chord_midpoint - half_chord_length;
    ISTATUS status = ShapeHitAllocatorAllocate(allocator,
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
SphereCheckBounds(
    _In_ const void *context,
    _In_opt_ PCMATRIX model_to_world,
    _In_ BOUNDING_BOX world_bounds,
    _Out_ bool *contains
    )
{
    PCSPHERE sphere = (PCSPHERE)context;

    float_t radius = sqrt(sphere->radius_squared);
    VECTOR3 to_bound = VectorCreate(radius, radius, radius);

    POINT3 lower_bound = PointVectorSubtract(sphere->center, to_bound);
    POINT3 upper_bound = PointVectorAdd(sphere->center, to_bound);

    BOUNDING_BOX object_bounds = BoundingBoxCreate(lower_bound, upper_bound);
    object_bounds = BoundingBoxTransform(model_to_world, object_bounds);

    *contains = BoundingBoxOverlaps(world_bounds, object_bounds);

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
// Static Variables
//

static const SHAPE_VTABLE sphere_vtable = {
    SphereTrace,
    SphereCheckBounds,
    SphereComputeNormal,
    SphereGetMaterial,
    NULL,
    NULL,
    SphereFree
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