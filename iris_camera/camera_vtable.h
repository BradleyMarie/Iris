/*++

Copyright (c) 2019 Brad Weinberger

Module Name:

    camera_vtable.h

Abstract:

    The vtable for a camera.

--*/

#ifndef _IRIS_CAMERA_CAMERA_VTABLE_
#define _IRIS_CAMERA_CAMERA_VTABLE_

#include "common/free_routine.h"
#include "iris/iris.h"

typedef
ISTATUS
(*PCAMERA_GENERATE_RAY_ROUTINE)(
    _In_ const void *context,
    _In_ float_t image_u,
    _In_ float_t image_v,
    _In_ float_t lens_u,
    _In_ float_t lens_v,
    _Out_ PRAY ray
    );

typedef struct _CAMERA_VTABLE {
    PCAMERA_GENERATE_RAY_ROUTINE generate_ray_routine;
    PFREE_ROUTINE free_routine;
} CAMERA_VTABLE, *PCAMERA_VTABLE;

typedef const CAMERA_VTABLE *PCCAMERA_VTABLE;

#endif // _IRIS_CAMERA_CAMERA_VTABLE_