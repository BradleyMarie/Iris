/*++

Copyright (c) 2021 Brad Weinberger

Module Name:

    material_vtable.h

Abstract:

    The vtable for a material.

--*/

#ifndef _IRIS_PHYSX_MATERIAL_VTABLE_
#define _IRIS_PHYSX_MATERIAL_VTABLE_

#include "iris_physx/bsdf_allocator.h"
#include "iris_physx/reflector_compositor.h"

//
// Types
//

typedef
ISTATUS
(*PMATERIAL_SAMPLE_ROUTINE)(
    _In_ const void *context,
    _In_ PCINTERSECTION intersection,
    _In_ const void *additional_data,
    _In_ const void *texture_coordinates,
    _Inout_ PBSDF_ALLOCATOR bsdf_allocator,
    _Inout_ PREFLECTOR_COMPOSITOR reflector_compositor,
    _Out_ PCBSDF *bsdf
    );

typedef struct _MATERIAL_VTABLE {
    PMATERIAL_SAMPLE_ROUTINE sample_routine;
    PFREE_ROUTINE free_routine;
} MATERIAL_VTABLE, *PMATERIAL_VTABLE;

typedef const MATERIAL_VTABLE *PCMATERIAL_VTABLE;

#endif // _IRIS_PHYSX_MATERIAL_VTABLE_