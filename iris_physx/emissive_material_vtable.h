/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    emissive_material_vtable.h

Abstract:

    The vtable for a emissive material.

--*/

#ifndef _IRIS_PHYSX_EMISSIVE_MATERIAL_VTABLE_
#define _IRIS_PHYSX_EMISSIVE_MATERIAL_VTABLE_

#include "iris/iris.h"
#include "iris_physx/spectrum.h"

//
// Types
//

typedef
ISTATUS
(*PEMISSIVE_MATERIAL_SAMPLE_ROUTINE)(
    _In_ const void *context,
    _In_ POINT3 model_hit_point,
    _In_ const void *additional_data,
    _Out_ PCSPECTRUM *spectrum
    );

typedef struct _EMISSIVE_MATERIAL_VTABLE {
    PEMISSIVE_MATERIAL_SAMPLE_ROUTINE sample_routine;
    PFREE_ROUTINE free_routine;
} EMISSIVE_MATERIAL_VTABLE, *PEMISSIVE_MATERIAL_VTABLE;

typedef const EMISSIVE_MATERIAL_VTABLE *PCEMISSIVE_MATERIAL_VTABLE;

#endif // _IRIS_PHYSX_EMISSIVE_MATERIAL_VTABLE_