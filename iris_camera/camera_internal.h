/*++

Copyright (c) 2021 Brad Weinberger

Module Name:

    camera_internal.h

Abstract:

    The internal routines for a camera.

--*/

#ifndef _IRIS_CAMERA_CAMERA_INTERNAL_
#define _IRIS_CAMERA_CAMERA_INTERNAL_

#include "iris_camera/camera_vtable.h"

//
// Types
//

struct _CAMERA {
    PCCAMERA_VTABLE vtable;
    void *data;
    float_t image_min_u;
    float_t image_delta_u;
    float_t image_min_v;
    float_t image_delta_v;
    float_t lens_min_u;
    float_t lens_delta_u;
    float_t lens_min_v;
    float_t lens_delta_v;
};

//
// Functions
//

static
inline
ISTATUS
CameraGenerateRayDifferential(
    _In_ const struct _CAMERA *camera,
    _In_ float_t image_u,
    _In_ float_t image_v,
    _In_ float_t lens_u,
    _In_ float_t lens_v,
    _In_ float_t dimage_u_dx,
    _In_ float_t dimage_v_dy,
    _Out_ PRAY_DIFFERENTIAL ray_differential
    )
{
    assert(camera != NULL);
    assert((float_t)0.0 <= image_u && image_u <= (float_t)1.0);
    assert((float_t)0.0 <= image_v && image_v <= (float_t)1.0);
    assert((float_t)0.0 <= lens_u && lens_u <= (float_t)1.0);
    assert((float_t)0.0 <= lens_v && lens_v <= (float_t)1.0);
    assert(isfinite(dimage_u_dx));
    assert(isfinite(dimage_v_dy));
    assert(ray_differential != NULL);

    image_u = fma(image_u, camera->image_delta_u, camera->image_min_u);
    image_v = fma(image_v, camera->image_delta_v, camera->image_min_v);
    dimage_u_dx = fma(dimage_u_dx, camera->image_delta_u, camera->image_min_u);
    dimage_v_dy = fma(dimage_v_dy, camera->image_delta_v, camera->image_min_v);
    lens_u = fma(lens_u, camera->lens_delta_u, camera->lens_min_u);
    lens_v = fma(lens_v, camera->lens_delta_v, camera->lens_min_v);

    RAY ray;
    ISTATUS status = camera->vtable->generate_ray_routine(camera->data,
                                                          image_u,
                                                          image_v,
                                                          lens_u,
                                                          lens_v,
                                                          &ray);


    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    RAY rx;
    status = camera->vtable->generate_ray_routine(camera->data,
                                                  image_u + dimage_u_dx,
                                                  image_v,
                                                  lens_u,
                                                  lens_v,
                                                  &rx);


    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    RAY ry;
    status = camera->vtable->generate_ray_routine(camera->data,
                                                  image_u,
                                                  image_v + dimage_v_dy,
                                                  lens_u,
                                                  lens_v,
                                                  &ry);


    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    *ray_differential = RayDifferentialCreate(ray, rx, ry);

    return ISTATUS_SUCCESS;
}

#endif // _IRIS_CAMERA_CAMERA_INTERNAL_