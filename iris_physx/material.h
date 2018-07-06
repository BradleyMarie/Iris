/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    material.h

Abstract:

    A material computes a BRDF and the surface normal to be used to calculate
    lambertian falloff from the parameters generated during a successful 
    intersection.

--*/

#ifndef _IRIS_PHYSX_MATERIAL_
#define _IRIS_PHYSX_MATERIAL_

#include "iris_physx/brdf_allocator.h"

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
    _Inout_ PBRDF_ALLOCATOR allocator,
    _Out_ PVECTOR3 world_shading_normal,
    _Out_ PCBRDF *brdf
    );

typedef struct _MATERIAL_VTABLE {
    PMATERIAL_SAMPLE_ROUTINE sample_routine;
    PFREE_ROUTINE free_routine;
} MATERIAL_VTABLE, *PMATERIAL_VTABLE;

typedef const MATERIAL_VTABLE *PCMATERIAL_VTABLE;

typedef struct _MATERIAL MATERIAL, *PMATERIAL;
typedef const MATERIAL *PCMATERIAL;

//
// Functions
//

ISTATUS
MaterialAllocate(
    _In_ PCMATERIAL_VTABLE vtable,
    _In_reads_bytes_opt_(data_size) const void *data,
    _In_ size_t data_size,
    _In_ size_t data_alignment,
    _Out_ PMATERIAL *material
    );

ISTATUS
MaterialSample(
    _In_ PCMATERIAL material,
    _In_ POINT3 model_hit_point,
    _In_ VECTOR3 world_surface_normal,
    _In_ const void *additional_data,
    _Inout_ PBRDF_ALLOCATOR allocator,
    _Out_ PVECTOR3 world_shading_normal,
    _Out_ PCBRDF *brdf
    );

void
MaterialRetain(
    _In_opt_ PMATERIAL material
    );

void
MaterialRelease(
    _In_opt_ _Post_invalid_ PMATERIAL material
    );

#endif // _IRIS_PHYSX_MATERIAL_
