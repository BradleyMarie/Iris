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

//
// Functions
//

ISTATUS
CameraAllocate(
    _In_ PCAMERA_GENERATE_RAY_ROUTINE generate_ray_routine,
    _In_ float_t pixel_min_u,
    _In_ float_t pixel_max_u,
    _In_ float_t pixel_min_v,
    _In_ float_t pixel_max_v,
    _In_ float_t lens_min_u,
    _In_ float_t lens_max_u,
    _In_ float_t lens_min_v,
    _In_ float_t lens_max_v,
    _In_reads_bytes_opt_(data_size) const void *data,
    _In_ size_t data_size,
    _In_ size_t data_alignment,
    _Out_ PCAMERA camera
    );

void
CameraFree(
    _In_opt_ _Post_invalid_ PCAMERA camera
    );

#endif // _IRIS_CAMERA_CAMERA_