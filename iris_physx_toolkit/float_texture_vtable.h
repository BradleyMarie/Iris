/*++

Copyright (c) 2021 Brad Weinberger

Module Name:

    float_texture_vtable.h

Abstract:

    The vtable for a FLOAT_TEXTURE.

--*/

#ifndef _IRIS_PHYSX_TOOLKIT_FLOAT_TEXTURE_VTABLE_
#define _IRIS_PHYSX_TOOLKIT_FLOAT_TEXTURE_VTABLE_

#include "iris_physx/iris_physx.h"

#if __cplusplus
extern "C" {
#endif // __cplusplus

//
// Types
//

typedef
ISTATUS
(*PFLOAT_TEXTURE_SAMPLE_ROUTINE)(
    _In_ const void *context,
    _In_ POINT3 model_hit_point,
    _In_ const void *additional_data,
    _In_ const void *texture_coordinates,
    _Out_ float_t *value
    );

typedef struct _FLOAT_TEXTURE_VTABLE {
    PFLOAT_TEXTURE_SAMPLE_ROUTINE sample_routine;
    PFREE_ROUTINE free_routine;
} FLOAT_TEXTURE_VTABLE, *PFLOAT_TEXTURE_VTABLE;

typedef const FLOAT_TEXTURE_VTABLE *PCFLOAT_TEXTURE_VTABLE;

#if __cplusplus
}
#endif // __cplusplus

#endif // _IRIS_PHYSX_TOOLKIT_FLOAT_TEXTURE_VTABLE_