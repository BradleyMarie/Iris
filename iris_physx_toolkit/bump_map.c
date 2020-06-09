/*++

Copyright (c) 2020 Brad Weinberger

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

ISTATUS
BumpMapCompute(
    _In_ const void *context,
    _In_ POINT3 hit_point,
    _In_ VECTOR3 geometry_normal,
    _In_ const void *additional_data,
    _In_ const void *texture_coordinates,
    _Out_ PVECTOR3 shading_normal
    )
{
    PCBUMP_MAP bump_map = (PBUMP_MAP)context;
    PCUV_TEXTURE_COORDINATE uv_coordinates =
        (PCUV_TEXTURE_COORDINATE)texture_coordinates;

    float_t displacement;
    ISTATUS status = FloatTextureSample(bump_map->texture,
                                        hit_point,
                                        NULL,
                                        texture_coordinates,
                                        &displacement);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    POINT3 displaced_hit_point0 =
        PointVectorAddScaled(hit_point, geometry_normal, displacement);

    float_t modified_u =
        fabs(uv_coordinates->du_dx) + fabs(uv_coordinates->du_dy);

    if (modified_u == (float_t)0.0f)
    {
        modified_u = MINIMUM_DERIVATIVE;
    }
    else
    {
        modified_u *= (float_t)0.5;
    }

    UV_TEXTURE_COORDINATE modified_coords;
    modified_coords.uv[0] = uv_coordinates->uv[0] + modified_u;
    modified_coords.uv[1] = uv_coordinates->uv[1];
    modified_coords.du_dy = uv_coordinates->du_dy;
    modified_coords.dv_dx = uv_coordinates->dv_dx;
    modified_coords.dv_dy = uv_coordinates->dv_dy;

    POINT3 hit_point1 =
        PointVectorAddScaled(hit_point, uv_coordinates->dp_du, modified_u);

    float_t displacement_u;
    status = FloatTextureSample(bump_map->texture,
                                hit_point1,
                                NULL,
                                &modified_coords,
                                &displacement_u);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    // Assume flat surface
    POINT3 displaced_hit_point1 =
        PointVectorAddScaled(hit_point1, geometry_normal, displacement);

    float_t modified_v =
        fabs(uv_coordinates->dv_dx) + fabs(uv_coordinates->dv_dy);

    if (modified_v == (float_t)0.0f)
    {
        modified_v = MINIMUM_DERIVATIVE;
    }
    else
    {
        modified_v *= (float_t)0.5;
    }

    modified_coords.uv[0] = uv_coordinates->uv[0];
    modified_coords.uv[1] = uv_coordinates->uv[1] + modified_v;

    POINT3 hit_point2 =
        PointVectorAddScaled(hit_point, uv_coordinates->dp_dv, modified_v);

    float_t displacement_v;
    status = FloatTextureSample(bump_map->texture,
                                hit_point2,
                                NULL,
                                &modified_coords,
                                &displacement_v);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    // Assume flat surface
    POINT3 displaced_hit_point2 =
        PointVectorAddScaled(hit_point2, geometry_normal, displacement);

    VECTOR3 dp_du = PointSubtract(displaced_hit_point1, displaced_hit_point0);
    VECTOR3 dp_dv = PointSubtract(displaced_hit_point2, displaced_hit_point0);

    *shading_normal = VectorCrossProduct(dp_du, dp_dv);
    *shading_normal = VectorNormalize(*shading_normal, NULL, NULL);

    return ISTATUS_SUCCESS;
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