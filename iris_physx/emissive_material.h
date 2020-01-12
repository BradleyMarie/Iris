/*++

Copyright (c) 2019 Brad Weinberger

Module Name:

    emissive_material.h

Abstract:

    A emissive material maps a location on an emissive surface to a spectrum.

--*/

#ifndef _IRIS_PHYSX_EMISSIVE_MATERIAL_
#define _IRIS_PHYSX_EMISSIVE_MATERIAL_

#include "iris_physx/emissive_material_vtable.h"

//
// Types
//

typedef struct _EMISSIVE_MATERIAL EMISSIVE_MATERIAL, *PEMISSIVE_MATERIAL;
typedef const EMISSIVE_MATERIAL *PCEMISSIVE_MATERIAL;

//
// Functions
//

ISTATUS
EmissiveMaterialAllocate(
    _In_ PCEMISSIVE_MATERIAL_VTABLE vtable,
    _In_reads_bytes_opt_(data_size) const void *data,
    _In_ size_t data_size,
    _In_ size_t data_alignment,
    _Out_ PEMISSIVE_MATERIAL *emissive_material
    );

ISTATUS
EmissiveMaterialCacheColors(
    _In_opt_ PCEMISSIVE_MATERIAL emissive_material,
    _In_ PCOLOR_CACHE color_cache
    );

void
EmissiveMaterialRetain(
    _In_opt_ PEMISSIVE_MATERIAL emissive_material
    );

void
EmissiveMaterialRelease(
    _In_opt_ _Post_invalid_ PEMISSIVE_MATERIAL emissive_material
    );

#endif // _IRIS_PHYSX_EMISSIVE_MATERIAL_
