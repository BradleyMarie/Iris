/*++

Copyright (c) 2021 Brad Weinberger

Module Name:

    pinhole_camera.c

Abstract:

    Implements a pinhole camera.

--*/

#include <stdalign.h>

#include "iris_camera_toolkit/pinhole_camera.h"

//
// Types
//

typedef struct _PINHOLE_CAMERA {
    POINT3 location;
    POINT3 frame_corner;
    VECTOR3 frame_width;
    VECTOR3 frame_height;
} PINHOLE_CAMERA, *PPINHOLE_CAMERA;

typedef const PINHOLE_CAMERA *PCPINHOLE_CAMERA;

//
// Static Functions
//

static
ISTATUS
PinholeCameraGenerateRay(
    _In_ const void *context,
    _In_ float_t image_u,
    _In_ float_t image_v,
    _In_ float_t lens_u,
    _In_ float_t lens_v,
    _Out_ PRAY ray
    )
{
    PCPINHOLE_CAMERA pinhole_camera = (PCPINHOLE_CAMERA)context;

    POINT3 frame_origin = PointVectorAddScaled(pinhole_camera->frame_corner,
                                               pinhole_camera->frame_width,
                                               image_u);

    frame_origin = PointVectorAddScaled(frame_origin,
                                        pinhole_camera->frame_height,
                                        image_v);

    VECTOR3 camera_direction = PointSubtract(pinhole_camera->location,
                                             frame_origin);

    *ray = RayCreate(pinhole_camera->location, camera_direction);

    return ISTATUS_SUCCESS;
}

//
// Static Variables
//

static const CAMERA_VTABLE pinhole_camera_vtable = {
    PinholeCameraGenerateRay,
    NULL
};

//
// Functions
//

ISTATUS
PinholeCameraAllocate(
    _In_ POINT3 location,
    _In_ VECTOR3 direction,
    _In_ VECTOR3 up,
    _In_ float_t focal_length,
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

    if (!isfinite(frame_width) || frame_width <= (float_t)0.0)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    if (!isfinite(frame_height) || frame_height <= (float_t)0.0)
    {
        return ISTATUS_INVALID_ARGUMENT_05;
    }

    if (camera == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_06;
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

    PINHOLE_CAMERA pinhole_camera;
    pinhole_camera.location = location;
    pinhole_camera.frame_corner = frame_corner;
    pinhole_camera.frame_width = frame_width_vector;
    pinhole_camera.frame_height = frame_height_vector;

    ISTATUS status = CameraAllocate(&pinhole_camera_vtable,
                                    (float_t)0.0,
                                    (float_t)1.0,
                                    (float_t)0.0,
                                    (float_t)1.0,
                                    (float_t)0.0,
                                    (float_t)0.0,
                                    (float_t)0.0,
                                    (float_t)0.0,
                                    &pinhole_camera,
                                    sizeof(PINHOLE_CAMERA),
                                    alignof(PINHOLE_CAMERA),
                                    camera);

    return status;
}