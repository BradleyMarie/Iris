/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    orthographic_camera.c

Abstract:

    Implements an orthographic camera.

--*/

#include <stdalign.h>

#include "iris_camera_toolkit/orthographic_camera.h"

//
// Types
//

typedef struct _ORTHOGRAPHIC_CAMERA {
    VECTOR3 direction;
    POINT3 frame_corner;
    VECTOR3 frame_width;
    VECTOR3 frame_height;
} ORTHOGRAPHIC_CAMERA, *PORTHOGRAPHIC_CAMERA;

typedef const ORTHOGRAPHIC_CAMERA *PCORTHOGRAPHIC_CAMERA;

//
// Static Functions
//

static
ISTATUS
OrthographicCameraGenerateRay(
    _In_ const void *context,
    _In_ float_t image_u,
    _In_ float_t image_v,
    _In_ float_t lens_u,
    _In_ float_t lens_v,
    _Out_ PRAY ray
    )
{
    PCORTHOGRAPHIC_CAMERA orthographic_camera = (PCORTHOGRAPHIC_CAMERA)context;

    POINT3 frame_origin = PointVectorAddScaled(orthographic_camera->frame_corner,
                                               orthographic_camera->frame_width,
                                               image_u);

    frame_origin = PointVectorAddScaled(frame_origin,
                                        orthographic_camera->frame_height,
                                        image_v);

    *ray = RayCreate(frame_origin, orthographic_camera->direction);

    return ISTATUS_SUCCESS;
}

//
// Static Variables
//

static const CAMERA_VTABLE orthographic_camera_vtable = {
    OrthographicCameraGenerateRay,
    NULL
};

//
// Functions
//

ISTATUS
OrthographicCameraAllocate(
    _In_ POINT3 location,
    _In_ VECTOR3 direction,
    _In_ VECTOR3 up,
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

    if (!isfinite(frame_width) || frame_width <= (float_t)0.0)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    if (!isfinite(frame_height) || frame_height <= (float_t)0.0)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    if (camera == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_05;
    }

    direction = VectorNormalize(direction, NULL, NULL);
    up = VectorNormalize(up, NULL, NULL);

    VECTOR3 image_plane_u = VectorCrossProduct(up, direction);
    image_plane_u = VectorNormalize(image_plane_u, NULL, NULL);
    VECTOR3 frame_width_vector = VectorScale(image_plane_u, frame_width);

    VECTOR3 image_plane_v = VectorCrossProduct(image_plane_u, direction);
    image_plane_v = VectorNormalize(image_plane_v, NULL, NULL);
    VECTOR3 frame_height_vector = VectorScale(image_plane_v, frame_height);

    POINT3 frame_corner = PointVectorAddScaled(location,
                                               frame_width_vector,
                                               (float_t)-0.5);
    frame_corner = PointVectorAddScaled(frame_corner,
                                        frame_height_vector,
                                        (float_t)-0.5);

    ORTHOGRAPHIC_CAMERA orthographic_camera;
    orthographic_camera.direction = direction;
    orthographic_camera.frame_corner = frame_corner;
    orthographic_camera.frame_width = frame_width_vector;
    orthographic_camera.frame_height = frame_height_vector;

    ISTATUS status = CameraAllocate(&orthographic_camera_vtable,
                                    (float_t)0.0,
                                    (float_t)1.0,
                                    (float_t)0.0,
                                    (float_t)1.0,
                                    (float_t)0.0,
                                    (float_t)0.0,
                                    (float_t)0.0,
                                    (float_t)0.0,
                                    &orthographic_camera,
                                    sizeof(ORTHOGRAPHIC_CAMERA),
                                    alignof(ORTHOGRAPHIC_CAMERA),
                                    camera);

    return status;
}