/*++

Copyright (c) 2020 Brad Weinberger

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
    float_t image_max_u;
    float_t image_min_v;
    float_t image_max_v;
    float_t lens_min_u;
    float_t lens_max_u;
    float_t lens_min_v;
    float_t lens_max_v;
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
    assert(camera->lens_min_u <= lens_u);
    assert(lens_u <= camera->lens_max_u);
    assert(camera->lens_min_v <= lens_v);
    assert(lens_v <= camera->lens_max_v);
    assert(isfinite(dimage_u_dx));
    assert(isfinite(dimage_v_dy));
    assert(ray_differential != NULL);

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