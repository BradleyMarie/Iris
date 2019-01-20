/*++

Copyright (c) 2019 Brad Weinberger

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

ISTATUS
CameraGenerateRay(
    _In_ const struct _CAMERA *camera,
    _In_ float_t image_u,
    _In_ float_t image_v,
    _In_ float_t lens_u,
    _In_ float_t lens_v,
    _Out_ PRAY ray
    )
{
    assert(camera != NULL);
    assert(camera->image_min_u <= image_u);
    assert(image_u <= camera->image_max_u);
    assert(camera->image_min_v <= image_v);
    assert(image_v <= camera->image_max_v);
    assert(camera->lens_min_u <= lens_u);
    assert(lens_u <= camera->lens_max_u);
    assert(camera->lens_min_v <= lens_v);
    assert(lens_v <= camera->lens_max_v);
    assert(ray != NULL);

    ISTATUS status = camera->vtable->generate_ray_routine(camera->data,
                                                          image_u,
                                                          image_v,
                                                          lens_u,
                                                          lens_v,
                                                          ray);

    return status;
}

#endif // _IRIS_CAMERA_CAMERA_INTERNAL_