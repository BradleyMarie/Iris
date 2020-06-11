/*++

Copyright (c) 2020 Brad Weinberger

Module Name:

    camera_vtable.h

Abstract:

    The vtable for a camera.

--*/

#ifndef _IRIS_CAMERA_CAMERA_VTABLE_
#define _IRIS_CAMERA_CAMERA_VTABLE_

#include "common/free_routine.h"
#include "iris_advanced/iris_advanced.h"

typedef
ISTATUS
(*PCAMERA_GENERATE_RAY_DIFFERENTIAL_ROUTINE)(
    _In_ const void *context,
    _In_ float_t image_u,
    _In_ float_t image_v,
    _In_ float_t lens_u,
    _In_ float_t lens_v,
    _In_ float_t dimage_u_dx,
    _In_ float_t dimage_v_dy,
    _Out_ PRAY_DIFFERENTIAL ray_differential
    );

typedef struct _CAMERA_VTABLE {
    PCAMERA_GENERATE_RAY_DIFFERENTIAL_ROUTINE generate_ray_differential_routine;
    PFREE_ROUTINE free_routine;
} CAMERA_VTABLE, *PCAMERA_VTABLE;

typedef const CAMERA_VTABLE *PCCAMERA_VTABLE;

#endif // _IRIS_CAMERA_CAMERA_VTABLE_