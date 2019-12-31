/*++

Copyright (c) 2019 Brad Weinberger

Module Name:

    reflector_texture_vtable.h

Abstract:

    The vtable for a REFLECTOR_TEXTURE.

--*/

#ifndef _IRIS_PHYSX_TOOLKIT_REFLECTOR_TEXTURE_VTABLE_
#define _IRIS_PHYSX_TOOLKIT_REFLECTOR_TEXTURE_VTABLE_

#include "iris_physx/iris_physx.h"

#if __cplusplus
extern "C" {
#endif // __cplusplus

//
// Types
//

typedef
ISTATUS
(*PREFLECTOR_TEXTURE_SAMPLE_ROUTINE)(
    _In_ const void *context,
    _In_ POINT3 model_hit_point,
    _In_ const void *additional_data,
    _In_ const void *texture_coordinates,
    _Inout_ PREFLECTOR_COMPOSITOR reflector_compositor,
    _Out_ PCREFLECTOR *value
    );

typedef struct _REFLECTOR_TEXTURE_VTABLE {
    PREFLECTOR_TEXTURE_SAMPLE_ROUTINE sample_routine;
    PFREE_ROUTINE free_routine;
} REFLECTOR_TEXTURE_VTABLE, *PREFLECTOR_TEXTURE_VTABLE;

typedef const REFLECTOR_TEXTURE_VTABLE *PCREFLECTOR_TEXTURE_VTABLE;

#if __cplusplus
}
#endif // __cplusplus

#endif // _IRIS_PHYSX_TOOLKIT_REFLECTOR_TEXTURE_VTABLE_