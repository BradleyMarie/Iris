/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    camera.h

Abstract:

    Generates the ray to be traced from UV coordinates on the framebuffer and
    lens. The generated ray is not guaranteed to be normalized.

--*/

#ifndef _IRIS_CAMERA_CAMERA_
#define _IRIS_CAMERA_CAMERA_

#include "iris_advanced/iris_advanced.h"

//
// Types
//

ISTATUS
(*PCAMERA_GENERATE_RAY_ROUTINE)(
    _In_ const void *context,
    _In_ float_t pixel_u,
    _In_ float_t pixel_v,
    _In_ float_t lens_u,
    _In_ float_t lens_v,
    _Out_ PRAY ray
    );

typedef struct _CAMERA CAMERA, *PCAMERA;
typedef const CAMERA *PCCAMERA;

typedef
ISTATUS
(*PCAMERA_LIFETIME_ROUTINE)(
    _Inout_opt_ void *context,
    _In_ PCCAMERA camera
    );

//
// Functions
//

ISTATUS
CameraCreate(
    _In_ PCAMERA_GENERATE_RAY_ROUTINE generate_ray_routine,
    _In_ float_t pixel_min_u,
    _In_ float_t pixel_max_u,
    _In_ float_t pixel_min_v,
    _In_ float_t pixel_max_v,
    _In_ float_t lens_min_u,
    _In_ float_t lens_max_u,
    _In_ float_t lens_min_v,
    _In_ float_t lens_max_v,
    _In_ const void *camera_context,
    _In_ PCAMERA_LIFETIME_ROUTINE callback,
    _In_ void *callback_context
    );

#endif // _IRIS_CAMERA_CAMERA_