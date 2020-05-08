/*++

Copyright (c) 2020 Brad Weinberger

Module Name:

    uv_texture_coordinate.h

Abstract:

    Texture coordinate emitted by shapes that use UV texture mapping.

--*/

#ifndef _IRIS_PHYSX_TOOLKIT_UV_TEXTURE_COORDINATE_
#define _IRIS_PHYSX_TOOLKIT_UV_TEXTURE_COORDINATE_

#include "iris_physx/iris_physx.h"

//
// Types
//

typedef struct _UV_TEXTURE_COORDINATE {
    float_t uv[2];
    VECTOR3 dpdu;
    VECTOR3 dpdv;
} UV_TEXTURE_COORDINATE, *PUV_TEXTURE_COORDINATE;

typedef const UV_TEXTURE_COORDINATE *PCUV_TEXTURE_COORDINATE;

#endif // _IRIS_PHYSX_TOOLKIT_UV_TEXTURE_COORDINATE_