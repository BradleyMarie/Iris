/*++

Copyright (c) 2020 Brad Weinberger

Module Name:

    thin_lens_camera.c

Abstract:

    Implements a thin lens camera.

--*/

#include <stdalign.h>

#include "iris_camera_toolkit/thin_lens_camera.h"

//
// Types
//

typedef struct _THIN_LENS_CAMERA {
    POINT3 location;
    VECTOR3 lens_radius_u;
    VECTOR3 lens_radius_v;
    POINT3 frame_corner;
    VECTOR3 frame_width;
    VECTOR3 frame_height;
} THIN_LENS_CAMERA, *PTHIN_LENS_CAMERA;

typedef const THIN_LENS_CAMERA *PCTHIN_LENS_CAMERA;

//
// Static Functions
//

static
ISTATUS
ThinLensCameraGenerateRay(
    _In_ const void *context,
    _In_ float_t image_u,
    _In_ float_t image_v,
    _In_ float_t radius_squared,
    _In_ float_t theta,
    _Out_ PRAY ray
    )
{
    PCTHIN_LENS_CAMERA thin_lens_camera = (PCTHIN_LENS_CAMERA)context;

    POINT3 frame_origin = PointVectorAddScaled(thin_lens_camera->frame_corner,
                                               thin_lens_camera->frame_width,
                                               image_u);

    frame_origin = PointVectorAddScaled(thin_lens_camera->frame_corner,
                                        thin_lens_camera->frame_height,
                                        image_v);

    float_t radius = sqrt(radius_squared);

    float_t sin_theta, cos_theta;
    SinCos(theta, &sin_theta, &cos_theta);

    float_t lens_u = radius * cos_theta;
    float_t lens_v = radius * sin_theta;

    POINT3 ray_origin = PointVectorAddScaled(thin_lens_camera->location,
                                             thin_lens_camera->lens_radius_u,
                                             lens_u);

    ray_origin = PointVectorAddScaled(ray_origin,
                                      thin_lens_camera->lens_radius_v,
                                      lens_v);

    VECTOR3 camera_direction = PointSubtract(ray_origin,
                                             frame_origin);

    *ray = RayCreate(ray_origin, camera_direction);

    return ISTATUS_SUCCESS;
}

//
// Static Variables
//

static const CAMERA_VTABLE thin_lens_camera_vtable = {
    ThinLensCameraGenerateRay,
    NULL
};

//
// Functions
//

ISTATUS
ThinLensCameraAllocate(
    _In_ POINT3 location,
    _In_ VECTOR3 direction,
    _In_ VECTOR3 up,
    _In_ float_t focal_length,
    _In_ float_t f_number,
    _In_ float_t frame_width,
    _In_ float_t frame_height,
    _Out_ PCAMERA *camera
    )
{
    if (!PointValidate(location))
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (!VectorValidate(direction))
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (!VectorValidate(up))
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (!isfinite(focal_length) || focal_length <= (float_t)0.0)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    if (!isfinite(focal_length) || f_number <= (float_t)0.0)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    if (!isfinite(frame_width) || frame_width <= (float_t)0.0)
    {
        return ISTATUS_INVALID_ARGUMENT_05;
    }

    if (!isfinite(frame_height) || frame_height <= (float_t)0.0)
    {
        return ISTATUS_INVALID_ARGUMENT_06;
    }

    if (camera == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_07;
    }

    direction = VectorNormalize(direction, NULL, NULL);
    up = VectorNormalize(up, NULL, NULL);

    VECTOR3 image_plane_u = VectorCrossProduct(direction, up);
    image_plane_u = VectorNormalize(image_plane_u, NULL, NULL);
    VECTOR3 frame_width_vector = VectorScale(image_plane_u, frame_width);

    VECTOR3 image_plane_v = VectorCrossProduct(direction, image_plane_u);
    image_plane_v = VectorNormalize(image_plane_v, NULL, NULL);
    VECTOR3 frame_height_vector = VectorScale(image_plane_v, frame_height);

    POINT3 frame_corner = PointVectorAddScaled(location,
                                               direction,
                                               -focal_length);
    frame_corner = PointVectorAddScaled(frame_corner,
                                        frame_width_vector,
                                        (float_t)-0.5);
    frame_corner = PointVectorAddScaled(frame_corner,
                                        frame_height_vector,
                                        (float_t)-0.5);

    float_t aperture = focal_length / f_number;
    float_t aperture_radius = (float_t)0.5 * aperture;

    VECTOR3 lens_radius_u = VectorScale(image_plane_u, aperture_radius);
    VECTOR3 lens_radius_v = VectorScale(image_plane_v, aperture_radius);

    THIN_LENS_CAMERA thin_lens_camera;
    thin_lens_camera.location = location;
    thin_lens_camera.lens_radius_u = lens_radius_u;
    thin_lens_camera.lens_radius_v = lens_radius_v;
    thin_lens_camera.frame_corner = frame_corner;
    thin_lens_camera.frame_width = frame_width_vector;
    thin_lens_camera.frame_height = frame_height_vector;

    ISTATUS status = CameraAllocate(&thin_lens_camera_vtable,
                                    (float_t)0.0,
                                    (float_t)1.0,
                                    (float_t)0.0,
                                    (float_t)1.0,
                                    (float_t)0.0,
                                    (float_t)1.0,
                                    (float_t)0.0,
                                    iris_two_pi,
                                    &thin_lens_camera,
                                    sizeof(THIN_LENS_CAMERA),
                                    alignof(THIN_LENS_CAMERA),
                                    camera);

    return status;
}