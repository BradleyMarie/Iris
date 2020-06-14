/*++

Copyright (c) 2020 Brad Weinberger

Module Name:

    uv_texture_coordinate.h

Abstract:

    Texture coordinate emitted by shapes that use UV texture mapping.

--*/

#ifndef _IRIS_PHYSX_TOOLKIT_UV_TEXTURE_COORDINATE_
#define _IRIS_PHYSX_TOOLKIT_UV_TEXTURE_COORDINATE_

#include "iris_physx/iris_physx.h"

//
// Types
//

typedef struct _UV_TEXTURE_COORDINATE {
    float_t uv[2];
    VECTOR3 dp_du;
    VECTOR3 dp_dv;
    float_t du_dx;
    float_t du_dy;
    float_t dv_dx;
    float_t dv_dy;
    bool has_derivatives;
} UV_TEXTURE_COORDINATE, *PUV_TEXTURE_COORDINATE;

typedef const UV_TEXTURE_COORDINATE *PCUV_TEXTURE_COORDINATE;

//
// Functions
//

static
inline
void
UVTextureCoordinateInitialize(
    _Inout_ PUV_TEXTURE_COORDINATE coordinate,
    _In_ const float uv[2],
    _In_ VECTOR3 dp_du,
    _In_ VECTOR3 dp_dv,
    _In_ RAY_DIFFERENTIAL ray_differential,
    _In_ POINT3 hit_point,
    _In_ VECTOR3 surface_normal
    )
{
    assert(coordinate != NULL);
    assert(uv != NULL);
    assert(isfinite(uv[0]) && isfinite(uv[1]));
    assert(VectorValidate(dp_du));
    assert(VectorValidate(dp_dv));
    assert(RayDifferentialValidate(ray_differential));
    assert(ray_differential.has_differentials);
    assert(VectorValidate(surface_normal));

    coordinate->uv[0] = uv[0];
    coordinate->uv[1] = uv[1];
    coordinate->dp_du = dp_du;
    coordinate->dp_dv = dp_dv;
    coordinate->has_derivatives = true;

    VECTOR3 to_origin = VectorCreate(hit_point.x, hit_point.y, hit_point.z);
    float_t distance = VectorDotProduct(surface_normal, to_origin);

    to_origin = VectorCreate(ray_differential.rx.origin.x,
                             ray_differential.rx.origin.y,
                             ray_differential.rx.origin.z);
    float_t tx =
        -(VectorDotProduct(surface_normal, to_origin) - distance) /
        VectorDotProduct(surface_normal, ray_differential.rx.direction);

    if (!isfinite(tx))
    {
        coordinate->du_dx = (float_t)0.0;
        coordinate->dv_dx = (float_t)0.0;
        coordinate->du_dy = (float_t)0.0;
        coordinate->dv_dy = (float_t)0.0;
        return;
    }

    POINT3 px = RayEndpoint(ray_differential.rx, tx);

    to_origin = VectorCreate(ray_differential.ry.origin.x,
                             ray_differential.ry.origin.y,
                             ray_differential.ry.origin.z);
    float_t ty =
        -(VectorDotProduct(surface_normal, to_origin) - distance) /
        VectorDotProduct(surface_normal, ray_differential.ry.direction);

    if (!isfinite(ty))
    {
        coordinate->du_dx = (float_t)0.0;
        coordinate->dv_dx = (float_t)0.0;
        coordinate->du_dy = (float_t)0.0;
        coordinate->dv_dy = (float_t)0.0;
        return;
    }

    POINT3 py = RayEndpoint(ray_differential.ry, ty);

    VECTOR_AXIS dominant_axis = VectorDominantAxis(surface_normal);

    float_t a[2][2];
    float_t b_x[2];
    float_t b_y[2];
    switch (dominant_axis)
    {
        case VECTOR_X_AXIS:
            a[0][0] = dp_du.y;
            a[0][1] = dp_dv.y;
            a[1][0] = dp_du.z;
            a[1][1] = dp_dv.z;
            b_x[0] = px.y - hit_point.y;
            b_x[1] = px.z - hit_point.z;
            b_y[0] = py.y - hit_point.y;
            b_y[1] = py.z - hit_point.z;
            break;
        case VECTOR_Y_AXIS:
            a[0][0] = dp_du.x;
            a[0][1] = dp_dv.x;
            a[1][0] = dp_du.z;
            a[1][1] = dp_dv.z;
            b_x[0] = px.x - hit_point.x;
            b_x[1] = px.z - hit_point.z;
            b_y[0] = py.x - hit_point.x;
            b_y[1] = py.z - hit_point.z;
            break;
        case VECTOR_Z_AXIS:
            a[0][0] = dp_du.x;
            a[0][1] = dp_dv.x;
            a[1][0] = dp_du.y;
            a[1][1] = dp_dv.y;
            b_x[0] = px.x - hit_point.x;
            b_x[1] = px.y - hit_point.y;
            b_y[0] = py.x - hit_point.x;
            b_y[1] = py.y - hit_point.y;
            break;
    }

    float_t determinant = a[0][0] * a[1][1] - a[0][1] * a[1][0];
    if (fabs(determinant) < (float_t)0.0000001)
    {
        coordinate->du_dx = (float_t)0.0;
        coordinate->dv_dx = (float_t)0.0;
        coordinate->du_dy = (float_t)0.0;
        coordinate->dv_dy = (float_t)0.0;
        return;
    }

    float_t du_dx = (a[1][1] * b_x[0] - a[0][1] * b_x[1]) / determinant;
    float_t dv_dx = (a[0][0] * b_x[1] - a[1][0] * b_x[0]) / determinant;

    if (!isfinite(du_dx) || !isfinite(dv_dx))
    {
        coordinate->du_dx = (float_t)0.0;
        coordinate->dv_dx = (float_t)0.0;
    }
    else
    {
        coordinate->du_dx = du_dx;
        coordinate->dv_dx = dv_dx;
    }

    float_t du_dy = (a[1][1] * b_y[0] - a[0][1] * b_y[1]) / determinant;
    float_t dv_dy = (a[0][0] * b_y[1] - a[1][0] * b_y[0]) / determinant;

    if (!isfinite(du_dy) || !isfinite(dv_dy))
    {
        coordinate->du_dy = (float_t)0.0;
        coordinate->dv_dy = (float_t)0.0;
    }
    else
    {
        coordinate->du_dy = du_dy;
        coordinate->dv_dy = dv_dy;
    }
}

static
inline
void
UVTextureCoordinateInitializeWithoutDerivatives(
    _Inout_ PUV_TEXTURE_COORDINATE coordinate,
    _In_ const float uv[2]
    )
{
    assert(coordinate != NULL);
    assert(uv != NULL);
    assert(isfinite(uv[0]) && isfinite(uv[1]));

    coordinate->uv[0] = uv[0];
    coordinate->uv[1] = uv[1];
    coordinate->has_derivatives = false;
}

#endif // _IRIS_PHYSX_TOOLKIT_UV_TEXTURE_COORDINATE_