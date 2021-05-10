/*++

Copyright (c) 2021 Brad Weinberger

Module Name:

    spectrum_texture_vtable.h

Abstract:

    The vtable for a SPECTRUM_TEXTURE.

--*/

#ifndef _IRIS_PHYSX_TOOLKIT_SPECTRUM_TEXTURE_VTABLE_
#define _IRIS_PHYSX_TOOLKIT_SPECTRUM_TEXTURE_VTABLE_

#include "iris_physx/iris_physx.h"

#if __cplusplus
extern "C" {
#endif // __cplusplus

//
// Types
//

typedef
ISTATUS
(*PSPECTRUM_TEXTURE_SAMPLE_ROUTINE)(
    _In_ const void *context,
    _In_ VECTOR3 direction,
    _In_opt_ VECTOR3 dxdy[2],
    _Inout_ PSPECTRUM_COMPOSITOR spectrum_compositor,
    _Out_ PCSPECTRUM *value
    );

typedef struct _SPECTRUM_TEXTURE_VTABLE {
    PSPECTRUM_TEXTURE_SAMPLE_ROUTINE sample_routine;
    PFREE_ROUTINE free_routine;
} SPECTRUM_TEXTURE_VTABLE, *PSPECTRUM_TEXTURE_VTABLE;

typedef const SPECTRUM_TEXTURE_VTABLE *PCSPECTRUM_TEXTURE_VTABLE;

#if __cplusplus
}
#endif // __cplusplus

#endif // _IRIS_PHYSX_TOOLKIT_SPECTRUM_TEXTURE_VTABLE_