/*++

Copyright (c) 2021 Brad Weinberger

Module Name:

    material.h

Abstract:

    A material computes a BSDF and the surface normal to be used to calculate
    lambertian falloff from the parameters generated during a successful 
    intersection.

--*/

#ifndef _IRIS_PHYSX_MATERIAL_
#define _IRIS_PHYSX_MATERIAL_

#include "iris_physx/material_vtable.h"

//
// Types
//

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
    _In_ PCINTERSECTION intersection,
    _In_ const void *additional_data,
    _In_ const void *texture_coordinates,
    _Inout_ PBSDF_ALLOCATOR bsdf_allocator,
    _Inout_ PREFLECTOR_COMPOSITOR reflector_compositor,
    _Out_ PCBSDF *bsdf
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
