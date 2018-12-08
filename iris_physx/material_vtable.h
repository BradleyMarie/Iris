/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    material_vtable.h

Abstract:

    The vtable for a material.

--*/

#ifndef _IRIS_PHYSX_MATERIAL_VTABLE_
#define _IRIS_PHYSX_MATERIAL_VTABLE_

#include "iris_physx/brdf_allocator.h"
#include "iris_physx/reflector_allocator.h"

//
// Types
//

typedef
ISTATUS
(*PMATERIAL_SAMPLE_ROUTINE)(
    _In_ const void *context,
    _In_ POINT3 model_hit_point,
    _In_ VECTOR3 world_surface_normal,
    _In_ const void *additional_data,
    _Inout_ PBRDF_ALLOCATOR brdf_allocator,
    _Inout_ PREFLECTOR_ALLOCATOR reflector_allocator,
    _Out_ PVECTOR3 world_shading_normal,
    _Out_ PCBRDF *brdf
    );

typedef struct _MATERIAL_VTABLE {
    PMATERIAL_SAMPLE_ROUTINE sample_routine;
    PFREE_ROUTINE free_routine;
} MATERIAL_VTABLE, *PMATERIAL_VTABLE;

typedef const MATERIAL_VTABLE *PCMATERIAL_VTABLE;

#endif // _IRIS_PHYSX_MATERIAL_VTABLE_