/*++

Copyright (c) 2021 Brad Weinberger

Module Name:

    bump_map.c

Abstract:

    Implements bump mapping

--*/

#include <stdalign.h>

#include "iris_physx_toolkit/bump_map.h"
#include "iris_physx_toolkit/uv_texture_coordinate.h"

//
// Defines
//

#define MINIMUM_DERIVATIVE ((float_t)0.0005f)

//
// Types
//

typedef struct _BUMP_MAP {
    PFLOAT_TEXTURE texture;
} BUMP_MAP, *PBUMP_MAP;

typedef const BUMP_MAP *PCBUMP_MAP;

//
// Static Functions
//

static
ISTATUS
BumpMapComputeWithTextureCoordinates(
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
    PCBUMP_MAP bump_map = (PBUMP_MAP)context;
    PCUV_TEXTURE_COORDINATE uv_coordinates =
        (PCUV_TEXTURE_COORDINATE)texture_coordinates;

    float_t displacement;
    ISTATUS status = FloatTextureSample(bump_map->texture,
                                        intersection,
                                        NULL,
                                        texture_coordinates,
                                        &displacement);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    float_t du =
        fabs(uv_coordinates->du_dx) + fabs(uv_coordinates->du_dy);

    if (du == (float_t)0.0)
    {
        du = MINIMUM_DERIVATIVE;
    }
    else
    {
        du *= (float_t)0.5;
    }

    UV_TEXTURE_COORDINATE modified_coords = *uv_coordinates;
    modified_coords.uv[0] = uv_coordinates->uv[0] + du;

    // TODO: Fix intersection
    INTERSECTION local_intersection = *intersection;

    float_t displacement_u;
    status = FloatTextureSample(bump_map->texture,
                                &local_intersection,
                                NULL,
                                &modified_coords,
                                &displacement_u);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    float_t dv =
        fabs(uv_coordinates->dv_dx) + fabs(uv_coordinates->dv_dy);

    if (dv == (float_t)0.0)
    {
        dv = MINIMUM_DERIVATIVE;
    }
    else
    {
        dv *= (float_t)0.5;
    }

    modified_coords.uv[0] = uv_coordinates->uv[0];
    modified_coords.uv[1] = uv_coordinates->uv[1] + dv;

    // TODO: Fix intersection

    float_t displacement_v;
    status = FloatTextureSample(bump_map->texture,
                                &local_intersection,
                                NULL,
                                &modified_coords,
                                &displacement_v);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    displacement_u = (displacement_u - displacement) / du;
    VECTOR3 dp_du = VectorAdd(uv_coordinates->dmodel_hit_point_du,
                              VectorScale(model_geometry_normal, displacement_u));

    displacement_v = (displacement_v - displacement) / dv;
    VECTOR3 dp_dv = VectorAdd(uv_coordinates->dmodel_hit_point_dv,
                              VectorScale(model_geometry_normal, displacement_v));

    *shading_normal = VectorCrossProduct(dp_du, dp_dv);
    *shading_normal = VectorNormalize(*shading_normal, NULL, NULL);

    if (VectorDotProduct(model_geometry_normal, *shading_normal) < (float_t)0.0)
    {
        *shading_normal = VectorNegate(*shading_normal);
    }

    *coordinate_space = NORMAL_MODEL_COORDINATE_SPACE;

    return ISTATUS_SUCCESS;
}

static
ISTATUS
BumpMapComputeWithoutTextureCoordinates(
    _In_ const void *context,
    _In_ PCINTERSECTION intersection,
    _In_ VECTOR3 model_geometry_normal,
    _In_ VECTOR3 world_geometry_normal,
    _In_ const void *additional_data,
    _Out_ PVECTOR3 shading_normal,
    _Out_ PNORMAL_COORDINATE_SPACE coordinate_space
    )
{
    PCBUMP_MAP bump_map = (PBUMP_MAP)context;

    float_t displacement;
    ISTATUS status = FloatTextureSample(bump_map->texture,
                                        intersection,
                                        NULL,
                                        NULL,
                                        &displacement);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    INTERSECTION local_intersection = *intersection;
    local_intersection.model_hit_point =
        PointVectorAddScaled(local_intersection.model_hit_point,
                             local_intersection.model_dp_dx,
                             (float_t)0.5);
    local_intersection.world_hit_point =
        PointVectorAddScaled(local_intersection.world_hit_point,
                             local_intersection.world_dp_dx,
                             (float_t)0.5);

    float_t displacement_x;
    status = FloatTextureSample(bump_map->texture,
                                &local_intersection,
                                NULL,
                                NULL,
                                &displacement_x);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    displacement_x = (displacement_x - displacement) / (float_t)0.5;

    local_intersection.model_hit_point =
        PointVectorAddScaled(intersection->model_hit_point,
                             local_intersection.model_dp_dy,
                             (float_t)0.5);
    local_intersection.world_hit_point =
        PointVectorAddScaled(intersection->world_hit_point,
                             local_intersection.world_dp_dy,
                             (float_t)0.5);

    float_t displacement_y;
    status = FloatTextureSample(bump_map->texture,
                                &local_intersection,
                                NULL,
                                NULL,
                                &displacement_y);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    displacement_y = (displacement_y - displacement) / (float_t)0.5;

    VECTOR3 dp_dx = VectorAddScaled(intersection->world_dp_dx,
                                    world_geometry_normal,
                                    displacement_x);

    VECTOR3 dp_dy = VectorAddScaled(intersection->world_dp_dy,
                                    world_geometry_normal,
                                    displacement_y);

    *shading_normal = VectorCrossProduct(dp_dx, dp_dy);
    *shading_normal = VectorNormalize(*shading_normal, NULL, NULL);

    if (VectorDotProduct(world_geometry_normal, *shading_normal) < (float_t)0.0)
    {
        *shading_normal = VectorNegate(*shading_normal);
    }

    *coordinate_space = NORMAL_WORLD_COORDINATE_SPACE;

    return ISTATUS_SUCCESS;
}

static
ISTATUS
BumpMapCompute(
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
    if (!intersection->has_derivatives)
    {
        *shading_normal = world_geometry_normal;
        *coordinate_space = NORMAL_WORLD_COORDINATE_SPACE;
        return ISTATUS_SUCCESS;
    }

    if (texture_coordinates == NULL)
    {
        ISTATUS status =
            BumpMapComputeWithoutTextureCoordinates(context,
                                                    intersection,
                                                    model_geometry_normal,
                                                    world_geometry_normal,
                                                    additional_data,
                                                    shading_normal,
                                                    coordinate_space);

        return status;
    }

    ISTATUS status =
        BumpMapComputeWithTextureCoordinates(context,
                                             intersection,
                                             model_geometry_normal,
                                             world_geometry_normal,
                                             additional_data,
                                             texture_coordinates,
                                             shading_normal,
                                             coordinate_space);

    return status;
}

static
void
BumpMapFree(
    _In_opt_ _Post_invalid_ void *context
    )
{
    PBUMP_MAP bump_map = (PBUMP_MAP)context;

    FloatTextureRelease(bump_map->texture);
}

//
// Static Variables
//

static const NORMAL_MAP_VTABLE bump_map_vtable = {
    BumpMapCompute,
    BumpMapFree
};

//
// Functions
//

ISTATUS
BumpMapAllocate(
    _In_ PFLOAT_TEXTURE texture,
    _Out_ PNORMAL_MAP *normal_map
    )
{
    if (texture == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (normal_map == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    BUMP_MAP bump_map;
    bump_map.texture = texture;

    ISTATUS status = NormalMapAllocate(&bump_map_vtable,
                                       &bump_map,
                                       sizeof(BUMP_MAP),
                                       alignof(BUMP_MAP),
                                       normal_map);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    FloatTextureRetain(texture);

    return ISTATUS_SUCCESS;
}