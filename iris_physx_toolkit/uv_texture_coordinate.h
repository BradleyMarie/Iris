/*++

Copyright (c) 2021 Brad Weinberger

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
    VECTOR3 dmodel_hit_point_du;
    VECTOR3 dmodel_hit_point_dv;
    VECTOR3 dworld_hit_point_du;
    VECTOR3 dworld_hit_point_dv;
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
    _In_ PCMATRIX model_to_world,
    _In_ VECTOR3 dmodel_hit_point_du,
    _In_ VECTOR3 dmodel_hit_point_dv,
    _In_ VECTOR3 dmodel_hit_point_dx,
    _In_ VECTOR3 dmodel_hit_point_dy
    )
{
    assert(coordinate != NULL);
    assert(uv != NULL);
    assert(isfinite(uv[0]) && isfinite(uv[1]));
    assert(VectorValidate(dmodel_hit_point_du));
    assert(VectorValidate(dmodel_hit_point_dv));
    assert(VectorValidate(dmodel_hit_point_dx));
    assert(VectorValidate(dmodel_hit_point_dy));

    coordinate->uv[0] = uv[0];
    coordinate->uv[1] = uv[1];

    VECTOR_AXIS diminished_axis = VectorDiminishedAxis(dmodel_hit_point_du);

    float_t a[2][2];
    float_t b_x[2];
    float_t b_y[2];
    switch (diminished_axis)
    {
        case VECTOR_X_AXIS:
            a[0][0] = dmodel_hit_point_du.y;
            a[0][1] = dmodel_hit_point_dv.y;
            a[1][0] = dmodel_hit_point_du.z;
            a[1][1] = dmodel_hit_point_dv.z;
            b_x[0] = dmodel_hit_point_dx.y;
            b_x[1] = dmodel_hit_point_dx.z;
            b_y[0] = dmodel_hit_point_dy.y;
            b_y[1] = dmodel_hit_point_dy.z;
            break;
        case VECTOR_Y_AXIS:
            a[0][0] = dmodel_hit_point_du.x;
            a[0][1] = dmodel_hit_point_dv.x;
            a[1][0] = dmodel_hit_point_du.z;
            a[1][1] = dmodel_hit_point_dv.z;
            b_x[0] = dmodel_hit_point_dx.x;
            b_x[1] = dmodel_hit_point_dx.z;
            b_y[0] = dmodel_hit_point_dy.x;
            b_y[1] = dmodel_hit_point_dy.z;
            break;
        case VECTOR_Z_AXIS:
            a[0][0] = dmodel_hit_point_du.x;
            a[0][1] = dmodel_hit_point_dv.x;
            a[1][0] = dmodel_hit_point_du.y;
            a[1][1] = dmodel_hit_point_dv.y;
            b_x[0] = dmodel_hit_point_dx.x;
            b_x[1] = dmodel_hit_point_dx.y;
            b_y[0] = dmodel_hit_point_dy.x;
            b_y[1] = dmodel_hit_point_dy.y;
            break;
    }

    float_t determinant = a[0][0] * a[1][1] - a[0][1] * a[1][0];
    if (fabs(determinant) < (float_t)0.0000001)
    {
        coordinate->has_derivatives = false;
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

    coordinate->dmodel_hit_point_du = dmodel_hit_point_du;
    coordinate->dmodel_hit_point_dv = dmodel_hit_point_dv;
    coordinate->dworld_hit_point_du =
        VectorMatrixTransposedMultiply(model_to_world, dmodel_hit_point_du);
    coordinate->dworld_hit_point_dv =
        VectorMatrixTransposedMultiply(model_to_world, dmodel_hit_point_dv);
    coordinate->has_derivatives = true;
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